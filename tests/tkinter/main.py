import os
import platform
import sys

import tkinter as tk
from PIL import Image, ImageTk

import pybindcef

class TkCefBrowser:
    def __init__(self, root, url="https://youtube.com"):
        self.root = root
        self.root.title("Tkinter CEF Integrated Browser")

        base = os.path.dirname(os.path.abspath(__file__))
        ext = ".exe" if platform.system() == "Windows" else ""
        worker_exe = os.path.join(base, f"cef_worker{ext}")
        res_dir = os.path.join(base, "Resources")
        pybindcef.initialize(worker_exe, res_dir)

        self.canvas = tk.Canvas(root, width=1200, height=800, bg="black", highlightthickness=0)
        self.canvas.pack(fill=tk.BOTH, expand=True)

        self.photo = None
        self.image_id = None 

        pybindcef.create_browser(url, self.on_paint)

        self.canvas.bind("<Configure>", self.on_resize)
        self.canvas.bind("<Motion>", self.on_mouse_move)
        self.canvas.bind("<Button-1>", lambda e: self.on_mouse_click(e, 0, False))
        self.canvas.bind("<ButtonRelease-1>", lambda e: self.on_mouse_click(e, 0, True))
        self.canvas.bind("<Button-3>", lambda e: self.on_mouse_click(e, 2, False))
        self.canvas.bind("<ButtonRelease-3>", lambda e: self.on_mouse_click(e, 2, True))

        self.canvas.bind("<MouseWheel>", self.on_mouse_wheel)
        self.canvas.bind("<Button-4>", self.on_mouse_wheel)
        self.canvas.bind("<Button-5>", self.on_mouse_wheel)

        self.root.bind("<KeyPress>", self.on_key_press)
        self.root.bind("<KeyRelease>", self.on_key_release)

        self.update_loop()

    def _get_vk_code(self, event):
        """Maps Tkinter keys to VK codes for CEF"""
        if len(event.char) == 1:
            char_code = ord(event.char)
            if 97 <= char_code <= 122: return char_code - 32

        mapping = {
            "BackSpace": 8, "Tab": 9, "Return": 13, "Escape": 27, "space": 32,
            "Delete": 46, "Up": 38, "Down": 40, "Left": 37, "Right": 39,
            "period": 190, "comma": 188, "minus": 189, "equal": 187
        }
        return mapping.get(event.keysym, event.keycode)

    def on_key_press(self, event):
        vk = self._get_vk_code(event)
        native = 0 if sys.platform.startswith("linux") else vk
        pybindcef.send_key_event(vk, native, 0, 0)
        if event.char:
            pybindcef.send_key_event(ord(event.char), native, 0, 2)
        return "break"

    def on_key_release(self, event):
        vk = self._get_vk_code(event)
        native = 0 if sys.platform.startswith("linux") else vk
        pybindcef.send_key_event(vk, native, 0, 1)
        return "break"

    def on_mouse_move(self, event):
        pybindcef.send_mouse_event(event.x, event.y, 0, False, 0) # 0 = Move

    def on_mouse_click(self, event, button, is_up):
        self.canvas.focus_set()
        pybindcef.set_focus(True)
        pybindcef.send_mouse_event(event.x, event.y, 1, is_up, button) # 1 = Click

    def on_mouse_wheel(self, event):
        delta = event.delta if event.delta != 0 else (120 if event.num == 4 else -120)
        pybindcef.send_mouse_wheel(event.x, event.y, 0, delta)

    def on_paint(self, buffer, width, height):
        img = Image.frombytes("RGBA", (width, height), buffer, "raw", "BGRA")
        self.photo = ImageTk.PhotoImage(image=img)

        if self.image_id is None:
            self.image_id = self.canvas.create_image(0, 0, image=self.photo, anchor=tk.NW)
        else:
            self.canvas.itemconfig(self.image_id, image=self.photo)

    def on_resize(self, event):
        if event.width > 1 and event.height > 1:
            pybindcef.resize(event.width, event.height)

    def update_loop(self):
        pybindcef.do_work()
        self.root.after(1, self.update_loop)

if __name__ == "__main__":
    root = tk.Tk()
    browser = TkCefBrowser(root)

    def shutdown():
        pybindcef.shutdown()
        root.destroy()

    root.protocol("WM_DELETE_WINDOW", shutdown)
    root.mainloop()