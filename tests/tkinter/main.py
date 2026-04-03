import tkinter as tk
from PIL import Image, ImageTk
import pybindcef
import os

class TkCefTest:
    def __init__(self, root):
        self.root = root
        self.root.title("Tkinter CEF Test")

        res_path = os.path.abspath("./Resources")
        sub_path = os.path.abspath("./cef_worker")
        pybindcef.initialize(sub_path, res_path)

        self.canvas = tk.Canvas(root, width=800, height=600, bg="black")
        self.canvas.pack(fill=tk.BOTH, expand=True)
        self.photo = None

        pybindcef.create_browser("https://www.testufo.com", self.on_paint)

        self.canvas.bind("<Configure>", self.on_resize)

        self.update_loop()

    def on_paint(self, buffer, width, height):
        img = Image.frombytes("RGBA", (width, height), buffer, "raw", "BGRA")
        self.photo = ImageTk.PhotoImage(image=img)
        self.canvas.create_image(0, 0, image=self.photo, anchor=tk.NW)

    def on_resize(self, event):
        pybindcef.resize(event.width, event.height)

    def update_loop(self):
        pybindcef.do_work()
        self.root.after(0, self.update_loop)

if __name__ == "__main__":
    root = tk.Tk()
    app = TkCefTest(root)
    root.protocol("WM_DELETE_WINDOW", lambda: (pybindcef.shutdown(), root.destroy()))
    root.mainloop()
