import os
import sys
import pybindcef
from kivy.config import Config

Config.set('graphics', 'width', '800')
Config.set('graphics', 'maxfps', '60')
Config.set('graphics', 'height', '600')

from kivy.app import App
from kivy.uix.widget import Widget
from kivy.properties import ObjectProperty
from kivy.graphics import Rectangle, Color, Callback, PushMatrix, PopMatrix, Scale, Translate
from kivy.graphics.texture import Texture
from kivy.clock import Clock
from kivy.core.window import Window


class CefBrowser(Widget):

    tex = ObjectProperty()

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        
        self.mapped = False 
        self.current_handle = 0
        self.tex = Texture.create(size=(800, 600), colorfmt='bgra')

        with self.canvas:
            Color(1, 1, 1, 1)
            PushMatrix()
            self.translate = Translate(0, 600)
            self.scale = Scale(1, -1, 1)

            Callback(self._lock_gpu)
            self.rect = Rectangle(texture=self.tex, pos=self.pos, size=(800, 600))
            Callback(self._unlock_gpu)
            PopMatrix()

        self.bind(pos=self.update_rect, size=self.update_rect)

        pybindcef.create_browser("https://youtu.be/QDia3e12czc?si=oTKjnrpa_gXZ4mO-", self.on_paint, self.on_gpu_paint, False, 60)

        Clock.schedule_interval(self.update_cef, 0)

        Clock.schedule_once(lambda dt: pybindcef.set_focus(True), 1.0)

        Window.bind(on_touch_down=self.on_ceftouch_down)
        Window.bind(mouse_pos=self.on_ceftouch_move)
        Window.bind(on_touch_up=self.on_ceftouch_up)
        Window.bind(on_key_down=self._on_keyboard_down)
        Window.bind(on_key_up=self._on_keyboard_up)

    def _on_keyboard_down(self, window, key, scancode, codepoint, modifiers):

        pybindcef.send_key_event(key, 0, 0)

        if codepoint:
            pybindcef.send_key_event(ord(codepoint), 0, 2) # 2 = CHAR
            
        return True

    def _on_keyboard_up(self, window, key, scancode):
        pybindcef.send_key_event(key, 0, 1)
        return True

    def on_ceftouch_down(self, instance, touch):
        if self.collide_point(*touch.pos):
            pybindcef.set_focus(True)
            self._dispatch(touch.x, touch.y, 0, False)
            self._dispatch(touch.x, touch.y, 1, False)
            return True

    def on_ceftouch_move(self, instance, pos):
        if self.collide_point(*pos):
            pybindcef.set_focus(True)
            self._dispatch(pos[0], pos[1], 0, False)
            return True

    def on_ceftouch_up(self, instance, touch):
        if self.collide_point(*touch.pos):
            pybindcef.set_focus(True)
            self._dispatch(touch.x, touch.y, 1, True)
            return True

    def _dispatch(self, x, y, event_type, is_up):
        cef_x = int(x - self.x)
        cef_y = int(self.height - (y - self.y))
        pybindcef.send_mouse_event(cef_x, cef_y, event_type, is_up)

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

    def on_paint(self, buffer_view, width, height):
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


class MainApp(App):
    def build(self):
        base = os.path.dirname(os.path.abspath(__file__))
        worker_exe = os.path.join(base, "cef_worker.exe")
        res_dir = os.path.join(base, "Resources")

        pybindcef.initialize(worker_exe, res_dir)
        
        browser = CefBrowser()
        Clock.schedule_once(lambda dt: pybindcef.init_graphics(), 0)
        
        return browser

    def on_pause(self):
        pybindcef.shutdown()

    def on_stop(self):
        pybindcef.shutdown()

if __name__ == '__main__':
    MainApp().run()