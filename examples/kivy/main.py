import os
import pybindcef
from kivy.config import Config

Config.set("graphics", "width", "1200")
Config.set("graphics", "height", "800")
Config.set("graphics", "maxfps", "60")
Config.set("input", "mouse", "mouse,multitouch_on_demand")

from carbonkivy.app import CarbonApp
from kivy.lang import Builder
from kivy import platform
from kivy.core.window import Window
from cef_webview import CefWebView, init_cef  # noqa: E402

app_kv = """
Screen:

    BoxLayout:
        orientation: "vertical"
        size_hint: [1, 1]

        CBoxLayout:
            adaptive: [False, True]
            padding: [dp(16), dp(12)]
            spacing: dp(8)

            CButtonGhost:
                id: btn_back
                icon: "chevron--left"
                on_press: browser.go_back()

            CButtonGhost:
                id: btn_fwd
                icon: "chevron--right"
                on_press: browser.go_forward()

            CButtonGhost:
                icon: "renew"
                on_press: browser.reload()

            CButtonGhost:
                icon: "close"
                on_press: browser.stop()

            CTextInputLayout:
                size_hint_x: 1
                CTextInput:
                    id: url_input
                    text: "https://google.com"
                    on_text_validate: browser.load(self.text)
                    multiline: False
                CTextInputHelperText:
                    text: "Enter URL and press Enter"
                CTextInputTrailingIconButton:
                    icon: "search"
                    on_press: browser.load(url_input.text)

            CButtonGhost:
                icon: "search"
                on_press: browser.find(find_input.text)

            CTextInputLayout:
                size_hint_x: 0.3
                CTextInput:
                    id: find_input
                    hint_text: "Find in page…"
                    on_text_validate: browser.find(self.text)
                    multiline: False

            CButtonGhost:
                icon: "debug"
                on_press: browser.open_dev_tools()

        CBoxLayout:
            adaptive: [False, True]
            padding: [dp(16), dp(4)]
            spacing: dp(16)

            CLabelNeutral:
                id: title_label
                text: "Browser in CarbonKivy"
                style: "body_compact_01"
                pos_hint: {"center_y": 0.5}

            CLabelNeutral:
                id: loading_label
                text: " "
                style: "body_compact_01"
                pos_hint: {"center_y": 0.5}

        CefWebView:
            id: browser
            size_hint: 1, 1
"""


# ---------------------------------------------------------------------------
# App
# ---------------------------------------------------------------------------
class MainApp(CarbonApp):

    def build(self):
        base = os.path.dirname(os.path.abspath(__file__))

        worker_exe = pybindcef.WORKER_EXE
        res_dir = pybindcef.RESOURCES_DIR
        init_cef(worker_exe, res_dir, base_dir=base)

        root = Builder.load_string(app_kv)
        browser: CefWebView = root.ids.browser

        def on_title(title):
            root.ids.title_label.text = title

        def on_address(url):
            root.ids.url_input.text = url

        def on_loading_state(is_loading, can_back, can_fwd):
            root.ids.loading_label.text = "Loading…" if is_loading else ""
            root.ids.btn_back.disabled = not can_back
            root.ids.btn_fwd.disabled = not can_fwd

        def on_load_error(code, text, url):
            print(f"[CEF] Load error {code} on {url}: {text}")

        def on_popup(url, frame, disposition, user_gesture):
            browser.load(url)

        def on_before_download(name, url, mime):
            downloads = os.path.join(os.path.expanduser("~"), "Downloads", name)
            print(f"[CEF] Downloading → {downloads}")
            return downloads

        def on_download_updated(path, total, recv, done, canceled):
            if done:
                print(f"[CEF] Download complete: {path}")
            elif canceled:
                print(f"[CEF] Download canceled: {path}")

        def on_console(level, msg, src, line):
            labels = {0: "DBG", 1: "INF", 2: "WRN", 3: "ERR"}
            print(f"[JS {labels.get(level, '?')}] {src}:{line}  {msg}")
            return False

        def on_find_result(identifier, count, final):
            if final:
                root.ids.find_input.hint_text = f"{count} match(es)"

        def on_full_screen_mode_change(mode):
            if mode:
                Window.add_widget(browser)
            else:
                Window.remove_widget(browser)

        browser._cb_title_change = on_title
        browser._cb_address_change = on_address
        browser._cb_loading_state_change = on_loading_state
        browser._cb_load_error = on_load_error
        browser._cb_before_popup = on_popup
        browser._cb_before_download = on_before_download
        browser._cb_download_updated = on_download_updated
        browser._cb_console_message = on_console
        browser._cb_find_result = on_find_result
        browser._cb_fullscreen_mode_change = on_full_screen_mode_change

        return root

    def on_pause(self):
        pybindcef.shutdown()

    def on_stop(self):
        pybindcef.shutdown()


if __name__ == "__main__":
    MainApp().run()
