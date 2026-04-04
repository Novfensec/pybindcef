#include "include/cef_app.h"
#include "bridge.h"
#include "platform_utils.h"
#include "common/identifiers.h"
#include "common/gpu_mapper.h"
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <iostream>

namespace py = pybind11;

CefRefPtr<MainHandler> g_handler;
CefRefPtr<CefBrowser> g_browser;

PYBIND11_MODULE(pybindcef, m) {
    m.def("initialize", [](std::string sub_path, std::string res_path) {
        return platform_initialize_cef(sub_path, res_path);
    });

    m.def("create_browser", [](std::string url, MainHandler::PaintCallback cb, MainHandler::AccelPaintCallback acb, bool shared_texture_enabled, int fps) {
        CefWindowInfo window_info;
        window_info.SetAsWindowless(0);
        window_info.shared_texture_enabled = shared_texture_enabled;
        
        CefBrowserSettings settings;
        settings.windowless_frame_rate = fps;
        g_handler = new MainHandler(cb, acb);

        CefBrowserHost::CreateBrowser(window_info, g_handler, url, settings, nullptr, nullptr);
    },
    py::arg("url"),
    py::arg("on_cpu_paint") = nullptr,
    py::arg("on_gpu_paint") = nullptr,
    py::arg("shared_texture_enabled") = true,
    py::arg("fps") = 60
    );

    m.def("resize", [](int w, int h) {
        if (g_handler && g_browser) {
            g_handler->width_ = w;
            g_handler->height_ = h;
            g_browser->GetHost()->WasResized();
            g_browser->GetHost()->Invalidate(PET_VIEW);
        }
    });

    m.def("do_work", []() { CefDoMessageLoopWork(); });
    m.def("shutdown", []() { CefShutdown(); });
    m.def("init_graphics", &init_graphics_bridge);
    m.def("map_gpu_texture", &platform_map_gpu_texture);
    m.def("lock_texture", &lock_texture);
    m.def("unlock_texture", &unlock_texture);

    m.def("set_focus", [](bool focused) {
        py::gil_scoped_acquire acquire;
        if (g_browser && g_browser->GetHost()) {
            g_browser->GetHost()->SetFocus(focused);
        }
    });
}
