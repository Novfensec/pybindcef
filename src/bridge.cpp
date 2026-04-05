#include "main_client.h"
#include "include/cef_app.h"
#include "common/gpu_mapper.h"
#include "platform_utils.h"
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>

namespace py = pybind11;

CefRefPtr<CefBrowser> g_browser;
CefRefPtr<MainClient> g_client;

PYBIND11_MODULE(pybindcef, m) {
    m.def("initialize", [](std::string sub_path, std::string res_path) {
        return platform_initialize_cef(sub_path, res_path);
    });

    m.def("create_browser", [](std::string url, RenderHandler::PaintCallback cb, RenderHandler::AccelPaintCallback acb, bool shared_texture_enabled, int fps) {
        CefWindowInfo window_info;
        window_info.SetAsWindowless(0);
        window_info.shared_texture_enabled = shared_texture_enabled;
        
        CefBrowserSettings settings;
        settings.windowless_frame_rate = fps;

        CefRefPtr<RenderHandler> render_handler = new RenderHandler(cb, acb);
        g_client = new MainClient(render_handler);

        CefBrowserHost::CreateBrowser(window_info, g_client, url, settings, nullptr, nullptr);
    },
    py::arg("url"),
    py::arg("on_cpu_paint") = nullptr,
    py::arg("on_gpu_paint") = nullptr,
    py::arg("shared_texture_enabled") = false,
    py::arg("fps") = 60
    );

    m.def("load_url", [](std::string url) {
        if (g_browser && g_browser->GetMainFrame()) {
            g_browser->GetMainFrame()->LoadURL(CefString(url));
        }
    });

    m.def("resize", [](int w, int h) {
        if (g_client && g_browser) {
            g_client->GetCustomRenderHandler()->width_ = w;
            g_client->GetCustomRenderHandler()->height_ = h;
            g_browser->GetHost()->WasResized();
            g_browser->GetHost()->Invalidate(PET_VIEW);
        }
    });

    m.def("do_work", []() {
        CefDoMessageLoopWork(); 
    });

    m.def("shutdown", []() {
        if (g_browser && g_browser->GetHost()) {
            g_browser->GetHost()->CloseBrowser(true);
        }
        g_browser = nullptr;
        g_client = nullptr;
        CefShutdown(); 
    });

    m.def("init_graphics", &init_graphics_bridge);
    m.def("map_gpu_texture", &platform_map_gpu_texture);
    m.def("lock_texture", &lock_texture);
    m.def("unlock_texture", &unlock_texture);

    m.def("set_focus", [](bool focused) {
        if (!g_browser || !g_browser->GetHost()) return;
        g_browser->GetHost()->SetFocus(focused);
    });

    m.def("send_mouse_event", [](int x, int y, int event_type, bool is_up) {
        if (!g_browser || !g_browser->GetHost()) return;

        CefMouseEvent mouse_event;
        mouse_event.x = x;
        mouse_event.y = y;
        mouse_event.modifiers = 0; 

        if (event_type == 0) {
            g_browser->GetHost()->SendMouseMoveEvent(mouse_event, false);
        } else {
            g_browser->GetHost()->SendMouseClickEvent(mouse_event, MBT_LEFT, is_up, 1);
        }
    });

    m.def("send_key_event", [](int key_code, uint32_t modifiers, int type) {
        if (!g_browser || !g_browser->GetHost()) return;

        CefKeyEvent event;
        event.windows_key_code = key_code;
        event.native_key_code = key_code;
        event.modifiers = modifiers;

        if (type == 0) event.type = KEYEVENT_RAWKEYDOWN;
        else if (type == 1) event.type = KEYEVENT_KEYUP;
        else if (type == 2) event.type = KEYEVENT_CHAR;

        g_browser->GetHost()->SendKeyEvent(event);
    });

    m.def("go_back", []() {
        if (g_browser && g_browser->CanGoBack()) {
            g_browser->GoBack();
        }
    });

    m.def("go_forward", []() {
        if (g_browser && g_browser->CanGoForward()) {
            g_browser->GoForward();
        }
    });

    m.def("reload", []() {
        if (g_browser) {
            g_browser->Reload();
        }
    });
}