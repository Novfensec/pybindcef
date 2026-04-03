from kivy.app import App
from kivy.uix.widget import Widget
from kivy.graphics import Rectangle, Color
from kivy.graphics.texture import Texture
from kivy.clock import Clock
import pybindcef 

import os

class CefBrowser(Widget):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.tex = Texture.create(size=(800, 600), colorfmt='bgra')
        self.tex.flip_vertical()
        
        with self.canvas:
            Color(1, 1, 1, 1)
            self.rect = Rectangle(texture=self.tex, pos=self.pos, size=(800, 600))

        pybindcef.create_browser("https://www.testufo.com/", self.on_paint)

        Clock.schedule_interval(self.update_cef, 1/60.0)

    def on_paint(self, buffer, width, height):
        try:

            self.tex.blit_buffer(buffer, colorfmt='bgra', bufferfmt='ubyte')
            self.canvas.ask_update()
        except Exception as e:
            print(f"Paint Error: {e}")

    def update_cef(self, dt):
        pybindcef.do_work()

class MainApp(App):
    def build(self):
        base = os.path.dirname(os.path.abspath(__file__))

        worker_exe = os.path.join(base, "linux", "cef_worker")

        res_dir = os.path.join(base, "Resources")

        print(f"Initializing CEF...")
        print(f"Worker: {worker_exe}")
        print(f"Resources: {res_dir}")

        pybindcef.initialize(worker_exe, res_dir)
        
        return CefBrowser()

    def on_pause(self):
        pybindcef.shutdown()

if __name__ == '__main__':
    MainApp().run()
