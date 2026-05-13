import os
import wx
from PIL import Image as PILImage

if os.name == "nt":
    cef_lib_path = os.path.abspath(r"../../pybindcef")
    if cef_lib_path not in os.environ.get('PATH', ''):
        os.environ['PATH'] = f"{cef_lib_path};{os.environ.get('PATH', '')}"
    if hasattr(os, 'add_dll_directory'):
        os.add_dll_directory(cef_lib_path)
import pybindcef


class CefBrowserPanel(wx.Panel):
    def __init__(self, parent):
        super().__init__(parent, style=wx.WANTS_CHARS | wx.FULL_REPAINT_ON_RESIZE)
        
        self.bitmap = None
        self._is_closing = False
        self.SetBackgroundStyle(wx.BG_STYLE_PAINT)

        pybindcef.create_browser(
            url="https://www.google.com",
            on_cpu_paint=self.on_cpu_paint,
            on_gpu_paint=None,
            shared_texture_enabled=False,
            fps=60
        )

        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_SET_FOCUS, self.OnGainFocus)

        self.Bind(wx.EVT_LEFT_DOWN, lambda e: self.on_mouse(e, 1, False, 0))
        self.Bind(wx.EVT_LEFT_UP, lambda e: self.on_mouse(e, 1, True, 0))
        self.Bind(wx.EVT_MOTION, lambda e: self.on_mouse(e, 0, False, 0))
        self.Bind(wx.EVT_MOUSEWHEEL, self.on_wheel)

        self.Bind(wx.EVT_KEY_DOWN, self.on_key_down)
        self.Bind(wx.EVT_KEY_DOWN, self.on_key_down)
        self.Bind(wx.EVT_KEY_UP, self.on_key_up)
        self.Bind(wx.EVT_CHAR, self.on_char)

    def OnGainFocus(self, event):
        """Ensures the blinking cursor (caret) appears immediately."""
        pybindcef.set_focus(True)
        event.Skip()

    def on_cpu_paint(self, buffer_view, width, height):
        if self._is_closing: return
        try:
            pil_img = PILImage.frombuffer("RGBA", (width, height), buffer_view, "raw", "BGRA", 0, 1)
            rgb_data = pil_img.convert("RGB").tobytes()
            wx.CallAfter(self._update_bitmap, rgb_data, width, height)
        except: pass

    def _update_bitmap(self, data, w, h):
        if self._is_closing: return
        self.bitmap = wx.Bitmap(wx.Image(w, h, data))
        self.Refresh(False)

    def OnPaint(self, event):
        dc = wx.AutoBufferedPaintDC(self)
        if self.bitmap: dc.DrawBitmap(self.bitmap, 0, 0)
        else: dc.Clear()

    def OnSize(self, event):
        w, h = self.GetClientSize()
        if w > 0 and h > 0: pybindcef.resize(w, h)
        event.Skip()

    def on_mouse(self, event, event_type, is_up, button):
        x, y = event.GetPosition()
        pybindcef.send_mouse_event(x, y, event_type, is_up, button)
        if not is_up: 
            self.SetFocus()
            pybindcef.set_focus(True)

    def on_wheel(self, event):
        x, y = event.GetPosition()
        pybindcef.send_mouse_wheel(x, y, 0, event.GetWheelRotation())

    def on_key_down(self, event):
        key = event.GetKeyCode()
        
        if key == wx.WXK_F11:
            self.GetParent().ToggleFullscreen()

        pybindcef.send_key_event(key, key, 0, 0)

        if key in [wx.WXK_BACK, wx.WXK_RETURN, wx.WXK_TAB, wx.WXK_ESCAPE, wx.WXK_DELETE]:
            return 

        event.Skip()

    def on_key_up(self, event):
        key = event.GetKeyCode()
        pybindcef.send_key_event(key, key, 0, 1)
        event.Skip()

    def on_char(self, event):
        code = event.GetUnicodeKey()

        if code != wx.WXK_NONE:
            pybindcef.send_key_event(code, code, 0, 2)
            
        event.Skip()

class MainFrame(wx.Frame):
    def __init__(self):
        super().__init__(None, title="pybindcef OSR", size=(1024, 768))
        self.is_fullscreen = False
        self.browser_panel = CefBrowserPanel(self)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.browser_panel, proportion=1, flag=wx.EXPAND)
        self.SetSizer(sizer)

        self.timer = wx.Timer(self)
        self.Bind(wx.EVT_TIMER, lambda e: pybindcef.do_work(), self.timer)
        self.timer.Start(10)

        self.Bind(wx.EVT_CLOSE, self.on_close)

        self.Layout()
        wx.CallLater(100, lambda: pybindcef.resize(*self.browser_panel.GetClientSize()))

    def ToggleFullscreen(self):
        """Logic to switch between windowed and full screen."""
        self.is_fullscreen = not self.is_fullscreen
        self.ShowFullScreen(self.is_fullscreen, wx.FULLSCREEN_ALL)

    def on_close(self, event):
        self.browser_panel._is_closing = True
        self.timer.Stop()
        pybindcef.shutdown()
        self.Destroy()

class App(wx.App):
    def OnInit(self):
        base = os.path.dirname(os.path.abspath(__file__))
        worker = os.path.join(base, "cef_worker.exe" if os.name == "nt" else "cef_worker")
        pybindcef.initialize(worker, os.path.join(base, "Resources"))
        MainFrame().Show()
        return True

if __name__ == '__main__':
    App(False).MainLoop()