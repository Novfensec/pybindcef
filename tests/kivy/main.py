import os
import sys

import pybindcef
from kivy.config import Config

Config.set('graphics', 'width', '1200')
Config.set('graphics', 'maxfps', '60')
Config.set('graphics', 'height', '800')
Config.set('input', 'mouse', 'mouse,multitouch_on_demand')

from kivy.app import App
from kivy.uix.widget import Widget
from kivy.properties import ObjectProperty
from kivy.graphics import Rectangle, Color, PushMatrix, PopMatrix, Scale, Translate
from kivy.graphics.texture import Texture
from kivy.clock import Clock
from kivy.core.window import Window
from kivy import platform
from kivy.lang import Builder

# CEF modifier flag bitmask constants
CEF_SHIFT   = 1 << 1
CEF_CTRL    = 1 << 2
CEF_ALT     = 1 << 3
CEF_META    = 1 << 7

KIVY_TO_VK = {
    # Special/control keys
    8:   8,    # Backspace
    9:   9,    # Tab
    13:  13,   # Return
    27:  27,   # Escape
    32:  32,   # Space
    127: 46,   # Delete
    273: 38,   # Up
    274: 40,   # Down
    275: 39,   # Right
    276: 37,   # Left
    278: 36,   # Home
    279: 35,   # End
    280: 33,   # Page Up
    281: 34,   # Page Down

    # Function keys
    282: 112,  # F1
    283: 113,  # F2
    284: 114,  # F3
    285: 115,  # F4
    286: 116,  # F5
    287: 117,  # F6
    288: 118,  # F7
    289: 119,  # F8
    290: 120,  # F9
    291: 121,  # F10
    292: 122,  # F11
    293: 123,  # F12

    # !! OEM symbols — these MUST be here or they collide with VK arrow/control codes
    39:  222,  # '  → VK_OEM_7   (was wrongly returning VK_RIGHT=39)
    44:  188,  # ,  → VK_OEM_COMMA
    45:  189,  # -  → VK_OEM_MINUS (was wrongly returning VK_INSERT=45)
    46:  190,  # .  → VK_OEM_PERIOD (was wrongly returning VK_DELETE=46)
    47:  191,  # /  → VK_OEM_2
    59:  186,  # ;  → VK_OEM_1
    61:  187,  # =  → VK_OEM_PLUS
    91:  219,  # [  → VK_OEM_4
    92:  220,  # \  → VK_OEM_5
    93:  221,  # ]  → VK_OEM_6
    96:  192,  # `  → VK_OEM_3
}


class CefBrowser(Widget):

    tex = ObjectProperty()

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self._current_modifiers = 0
        
        self.mapped = False 
        self.current_handle = 0
        self.tex = Texture.create(size=(800, 600), colorfmt='bgra')
        self._keyboard = Window.request_keyboard(self._on_keyboard_closed, self)
        self._keyboard.bind(on_key_down=self._on_keyboard_down)
        self._keyboard.bind(on_key_up=self._on_keyboard_up)
        with self.canvas.after:
            Color(1, 1, 1, 1)
            PushMatrix()
            self.translate = Translate(0, 600)
            self.scale = Scale(1, -1, 1)

            self.rect = Rectangle(texture=self.tex, pos=self.pos, size=(800, 600))
            PopMatrix()

        self.bind(pos=self.update_rect, size=self.update_rect)

        pybindcef.create_browser("https:/google.com", self.on_cpu_paint, self.on_gpu_paint, False, 60)

        Clock.schedule_interval(self.update_cef, 0)
        Clock.schedule_once(lambda dt: pybindcef.load_url("https://google.com"))

        Window.bind(on_touch_down=self.on_ceftouch_down)
        Window.bind(mouse_pos=self.on_cefmouse_move)
        Window.bind(on_touch_up=self.on_ceftouch_up)

        Window.bind(on_textinput=self._on_text_input)

    def _kivy_mods_to_cef(self, modifiers):
        """Convert Kivy modifier string list --> CEF bitmask."""
        flags = 0
        if 'shift'   in modifiers: flags |= CEF_SHIFT
        if 'ctrl'    in modifiers: flags |= CEF_CTRL
        if 'alt'     in modifiers: flags |= CEF_ALT
        if 'meta'    in modifiers: flags |= CEF_META
        if 'capslock' in modifiers: flags |= (1 << 0)
        if 'numlock'  in modifiers: flags |= (1 << 8)
        return flags

    def _kivy_key_to_vk(self, key):
        """Convert Kivy keycode --> Windows VK code."""
        if key in KIVY_TO_VK:
            return KIVY_TO_VK[key]
        # Letters: Kivy gives lowercase ASCII (a=97), VK needs uppercase (A=65)
        if 97 <= key <= 122:
            return key - 32
        return key

    def _on_keyboard_closed(self):
        self._keyboard = None

    def _on_text_input(self, window, text):
        """Fires ONLY for printable characters, already correctly shifted."""
        # Skip if ctrl/alt held — those are shortcuts not chars
        if self._current_modifiers & (CEF_CTRL | CEF_ALT):
            return

        for char in text:
            code = ord(char)
            if code >= 32 and code != 127:
                pybindcef.send_key_event(code, 0, self._current_modifiers, 2)

    def _on_keyboard_down(self, keyboard, keycode, text, modifiers):
        key  = keycode[0]
        vk   = self._kivy_key_to_vk(key)
        mods = self._kivy_mods_to_cef(modifiers)
        self._current_modifiers = mods

        # On Linux pass 0 as native_key — CEF resolves from windows_key_code
        # On Windows pass vk as native_key (current behavior)
        native = 0 if platform == 'linux' else vk

        pybindcef.send_key_event(vk, native, self._current_modifiers, 0)

        # Backspace/Delete/Enter etc. need an explicit CHAR too — on_textinput won't fire for these
        NEEDS_CHAR = {
            8:  8,
            13: 13,
            9:  9,
            27: 27,
        }
        if key in NEEDS_CHAR:
            pybindcef.send_key_event(NEEDS_CHAR[key], native, mods, 2)

        return True

    def _on_keyboard_up(self, keyboard, keycode):
        key = keycode[0]
        vk  = self._kivy_key_to_vk(key)
        native = 0 if platform == 'linux' else vk
        pybindcef.send_key_event(vk, native, self._current_modifiers, 1)
        return True

    def on_ceftouch_down(self, instance, touch):
        if not self.collide_point(*touch.pos):
            return False

        pybindcef.set_focus(True)

        if 'scroll' in touch.button:
            self._dispatch_wheel(touch.x, touch.y, touch.button)
            return True 

        button_map = {'left': 0, 'middle': 1, 'right': 2}
        button = button_map.get(touch.button, 0)
        self._dispatch(touch.x, touch.y, 1, False, button)

        touch.grab(self)
        return True

    def _dispatch_wheel(self, x, y, button):
        cef_x = int(x - self.x)
        cef_y = int(self.height - (y - self.y))

        step = 120

        dx, dy = 0, 0
        if button == 'scrollup':    dy = -step
        elif button == 'scrolldown': dy = step
        elif button == 'scrollleft':  dx = step
        elif button == 'scrollright': dx = -step

        pybindcef.send_mouse_wheel(cef_x, cef_y, dx, dy)

    def on_cefmouse_move(self, instance, pos):
        if self.collide_point(*pos):
            self._dispatch(pos[0], pos[1], 0, False, 0)
            return True

    def on_ceftouch_up(self, instance, touch):
        if self.collide_point(*touch.pos):
            button_map = {'left': 0, 'middle': 1, 'right': 2}
            button = button_map.get(touch.button, 0)
            
            self._dispatch(touch.x, touch.y, 1, True, button)
            touch.ungrab(self)
            return True

    def _dispatch(self, x, y, event_type, is_up, button_type):
        cef_x = int(x - self.x)
        cef_y = int(self.height - (y - self.y))
        pybindcef.send_mouse_event(cef_x, cef_y, event_type, is_up, button_type)

    def update_rect(self, *args):
        pybindcef.resize(int(self.size[0]), int(self.size[1]))
        self.rect.pos = self.pos
        self.rect.size = self.size
        self.translate.y = self.size[1]

    def on_gpu_paint(self, handle_id, width, height):
        if handle_id != self.current_handle:
            pybindcef.map_gpu_texture(handle_id, self.tex.id, width, height)
            self.current_handle = handle_id
            self.mapped = True

        self.canvas.ask_update()

    def _lock_gpu(self, instruction):
        if self.mapped:
            pybindcef.lock_texture()
            
    def _unlock_gpu(self, instruction):
        if self.mapped:
            pybindcef.unlock_texture()

    def on_cpu_paint(self, buffer_view, width, height):
        try:
            if self.tex.width != width or self.tex.height != height:
                self.tex = Texture.create(size=(width, height), colorfmt='bgra')

                self.rect.texture = self.tex

            self.tex.blit_buffer(buffer_view, colorfmt='bgra', bufferfmt='ubyte')
            self.canvas.ask_update()
        except Exception as e:
            print(f"Paint Resize Error: {e}")

    def update_cef(self, dt):
        pybindcef.do_work()

    def go_back(self):
        pybindcef.go_back()

app_kv = """
Screen:

    BoxLayout:
        orientation: "vertical"
        size_hint: [1, 1]

        BoxLayout:
            size_hint: 1, None
            height: self.minimum_height

        CefBrowser:
            id: browser
            size_hint: 1, 1
"""

class MainApp(App):
    def build(self):
        base = os.path.dirname(os.path.abspath(__file__))
        if platform == "linux":
            worker_exe = os.path.join(base, "cef_worker")
        elif platform == "win":
            worker_exe = os.path.join(base, "cef_worker")
        res_dir = os.path.join(base, "Resources")

        pybindcef.initialize(worker_exe, res_dir)
        return Builder.load_string(app_kv)

    def on_pause(self):
        pybindcef.shutdown()

    def on_stop(self):
        pybindcef.shutdown()

if __name__ == '__main__':
    MainApp().run()
