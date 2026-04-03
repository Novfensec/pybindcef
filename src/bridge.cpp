#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include "include/cef_app.h"
#include "bridge.h"
#include <iostream>

namespace py = pybind11;

CefRefPtr<KivyHandler> g_handler;
CefRefPtr<CefBrowser> g_browser;

PYBIND11_MODULE(pybindcef, m) {
    m.def("initialize", [](std::string sub_path, std::string res_path) {
        int argc = 0;
        char** argv = nullptr;
        CefMainArgs args(argc, argv);

        CefSettings settings;
        settings.no_sandbox = true;
        settings.windowless_rendering_enabled = true;
        settings.external_message_pump = false;
        settings.multi_threaded_message_loop = false;

        std::string cache_p = res_path + "/web_cache";
        CefString(&settings.cache_path).FromASCII(cache_p.c_str());
        CefString(&settings.browser_subprocess_path).FromASCII(sub_path.c_str());
        CefString(&settings.resources_dir_path).FromASCII(res_path.c_str());

        std::string locales_path = res_path + "/locales";
        CefString(&settings.locales_dir_path).FromASCII(locales_path.c_str());
        CefString(&settings.log_file).FromASCII("cef.log");

        return CefInitialize(args, settings, nullptr, nullptr);
    });

    m.def("create_browser", [](std::string url, KivyHandler::PaintCallback cb) {
        CefWindowInfo window_info;
        window_info.SetAsWindowless(0);

        CefBrowserSettings settings;
        g_handler = new KivyHandler(cb);
        
        CefBrowserHost::CreateBrowser(window_info, g_handler, url, settings, nullptr, nullptr);
    });

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
}