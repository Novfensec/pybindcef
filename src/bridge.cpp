#include "main_client.h"
#include "include/cef_app.h"
#include "common/gpu_mapper.h"
#include "platform_utils.h"
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <memory>
#include <string>

namespace py = pybind11;

//
// BrowserInstance
//
// One heap-allocated object per create_browser() call.  It owns the
// BrowserCallbacks struct (which handlers hold a raw pointer to) and the
// CefRefPtr<CefBrowser> / CefRefPtr<MainClient>.
//
// Exposed to Python as `pybindcef.Browser`.
//
struct BrowserInstance
{
    BrowserCallbacks callbacks;
    CefRefPtr<CefBrowser> browser;
    CefRefPtr<MainClient> client;

    //
    // Navigation
    //
    void load_url(const std::string &url)
    {
        if (browser && browser->GetMainFrame())
            browser->GetMainFrame()->LoadURL(CefString(url));
    }

    void go_back()
    {
        if (browser && browser->CanGoBack())
            browser->GoBack();
    }

    void go_forward()
    {
        if (browser && browser->CanGoForward())
            browser->GoForward();
    }

    void reload()
    {
        if (browser)
            browser->Reload();
    }

    void stop_load()
    {
        if (browser)
            browser->StopLoad();
    }

    bool is_loading() const
    {
        return browser ? browser->IsLoading() : false;
    }

    bool can_go_back() const
    {
        return browser ? browser->CanGoBack() : false;
    }

    bool can_go_forward() const
    {
        return browser ? browser->CanGoForward() : false;
    }

    //
    // Query
    //
    std::string get_url() const
    {
        if (browser && browser->GetMainFrame())
            return browser->GetMainFrame()->GetURL().ToString();
        return {};
    }

    //
    // Input / focus
    //
    void set_focus(bool focused)
    {
        if (browser && browser->GetHost())
            browser->GetHost()->SetFocus(focused);
    }

    void resize(int w, int h)
    {
        if (!client || !browser)
            return;
        client->GetCustomRenderHandler()->width_ = w;
        client->GetCustomRenderHandler()->height_ = h;
        browser->GetHost()->WasResized();
        browser->GetHost()->Invalidate(PET_VIEW);
    }

    void send_mouse_event(int x, int y, int event_type, bool is_up, int button_type)
    {
        if (!browser || !browser->GetHost())
            return;
        CefMouseEvent ev;
        ev.x = x;
        ev.y = y;
        ev.modifiers = 0;
        if (event_type == 0)
        {
            browser->GetHost()->SendMouseMoveEvent(ev, false);
        }
        else
        {
            browser->GetHost()->SendMouseClickEvent(
                ev, static_cast<cef_mouse_button_type_t>(button_type), is_up, 1);
        }
    }

    void send_mouse_wheel(int x, int y, int delta_x, int delta_y)
    {
        if (!browser || !browser->GetHost())
            return;
        CefMouseEvent ev;
        ev.x = x;
        ev.y = y;
        ev.modifiers = 0;
        browser->GetHost()->SendMouseWheelEvent(ev, delta_x, delta_y);
    }

    void send_key_event(int key_code, int native_key_code,
                        uint32_t modifiers, int type)
    {
        if (!browser || !browser->GetHost())
            return;
        CefKeyEvent ev;
        ev.windows_key_code = key_code;
        ev.native_key_code = native_key_code;
        ev.modifiers = modifiers;
        if (type == 0)
            ev.type = KEYEVENT_RAWKEYDOWN;
        else if (type == 1)
            ev.type = KEYEVENT_KEYUP;
        else if (type == 2)
            ev.type = KEYEVENT_CHAR;
        browser->GetHost()->SendKeyEvent(ev);
    }

    void set_zoom_level(double level)
    {
        if (browser && browser->GetHost())
            browser->GetHost()->SetZoomLevel(level);
    }

    //
    // Features
    //
    void execute_js(const std::string &code)
    {
        if (browser && browser->GetMainFrame())
            browser->GetMainFrame()->ExecuteJavaScript(
                CefString(code), browser->GetMainFrame()->GetURL(), 0);
    }

    void find(int identifier, const std::string &text,
              bool forward = true, bool case_sensitive = false)
    {
        if (browser && browser->GetHost())
            browser->GetHost()->Find(text, forward, case_sensitive, false);
    }

    void stop_find()
    {
        if (browser && browser->GetHost())
            browser->GetHost()->StopFinding(true);
    }

    void open_dev_tools()
    {
        if (!browser || !browser->GetHost())
            return;
        CefWindowInfo wi;
#if defined(_WIN32)
        wi.SetAsPopup(nullptr, "DevTools");
#endif
        CefBrowserSettings bs;
        browser->GetHost()->ShowDevTools(wi, nullptr, bs, CefPoint());
    }

    void close_dev_tools()
    {
        if (browser && browser->GetHost())
            browser->GetHost()->CloseDevTools();
    }

    void close()
    {
        if (browser && browser->GetHost())
            browser->GetHost()->CloseBrowser(true);
    }

    //
    // GPU helpers (delegated to platform layer)
    //
    void map_gpu_texture(uint64_t handle_id, uint32_t tex_id, int w, int h)
    {
        platform_map_gpu_texture(handle_id, tex_id, w, h);
    }

    void lock_texture_fn() { ::lock_texture(); }
    void unlock_texture_fn() { ::unlock_texture(); }
};

PYBIND11_MODULE(_pybindcef, m)
{

    //
    // Browser class
    //
    py::class_<BrowserInstance, std::shared_ptr<BrowserInstance>>(m, "Browser")

        .def("load_url", &BrowserInstance::load_url, py::arg("url"))
        .def("go_back", &BrowserInstance::go_back)
        .def("go_forward", &BrowserInstance::go_forward)
        .def("reload", &BrowserInstance::reload)
        .def("stop_load", &BrowserInstance::stop_load)
        .def("is_loading", &BrowserInstance::is_loading)
        .def("can_go_back", &BrowserInstance::can_go_back)
        .def("can_go_forward", &BrowserInstance::can_go_forward)

        .def("get_url", &BrowserInstance::get_url)

        .def("set_focus", &BrowserInstance::set_focus, py::arg("focused"))
        .def("resize", &BrowserInstance::resize, py::arg("w"), py::arg("h"))
        .def("send_mouse_event", &BrowserInstance::send_mouse_event,
             py::arg("x"), py::arg("y"), py::arg("event_type"),
             py::arg("is_up"), py::arg("button_type"))
        .def("send_mouse_wheel", &BrowserInstance::send_mouse_wheel,
             py::arg("x"), py::arg("y"), py::arg("delta_x"), py::arg("delta_y"))
        .def("send_key_event", &BrowserInstance::send_key_event,
             py::arg("key_code"), py::arg("native_key_code"),
             py::arg("modifiers"), py::arg("type"))
        .def("set_zoom_level", &BrowserInstance::set_zoom_level, py::arg("level"))

        .def("execute_js", &BrowserInstance::execute_js, py::arg("code"))
        .def("find", &BrowserInstance::find,
             py::arg("identifier"), py::arg("text"),
             py::arg("forward") = true, py::arg("case_sensitive") = false)
        .def("stop_find", &BrowserInstance::stop_find)
        .def("open_dev_tools", &BrowserInstance::open_dev_tools)
        .def("close_dev_tools", &BrowserInstance::close_dev_tools)
        .def("close", &BrowserInstance::close)

        .def("map_gpu_texture", &BrowserInstance::map_gpu_texture,
             py::arg("handle_id"), py::arg("tex_id"), py::arg("w"), py::arg("h"))
        .def("lock_texture", &BrowserInstance::lock_texture_fn)
        .def("unlock_texture", &BrowserInstance::unlock_texture_fn)

        // callbacks (readable/writable properties)
        // Render
        .def_property("on_cpu_paint", [](const BrowserInstance &b)
                      { return b.callbacks.on_cpu_paint; }, [](BrowserInstance &b, py::object cb)
                      { b.callbacks.on_cpu_paint = cb.is_none()
                                                       ? BrowserCallbacks::PaintCb{}
                                                       : cb.cast<BrowserCallbacks::PaintCb>(); })
        .def_property("on_gpu_paint", [](const BrowserInstance &b)
                      { return b.callbacks.on_gpu_paint; }, [](BrowserInstance &b, py::object cb)
                      { b.callbacks.on_gpu_paint = cb.is_none()
                                                       ? BrowserCallbacks::AccelCb{}
                                                       : cb.cast<BrowserCallbacks::AccelCb>(); })
        // LifeSpan
        .def_property("on_before_popup", [](const BrowserInstance &b)
                      { return b.callbacks.on_before_popup; }, [](BrowserInstance &b, py::object cb)
                      { b.callbacks.on_before_popup = cb.is_none()
                                                          ? decltype(b.callbacks.on_before_popup){}
                                                          : cb.cast<decltype(b.callbacks.on_before_popup)>(); })
        // Load
        .def_property("on_load_start", [](const BrowserInstance &b)
                      { return b.callbacks.on_load_start; }, [](BrowserInstance &b, py::object cb)
                      { b.callbacks.on_load_start = cb.is_none()
                                                        ? decltype(b.callbacks.on_load_start){}
                                                        : cb.cast<decltype(b.callbacks.on_load_start)>(); })
        .def_property("on_load_end", [](const BrowserInstance &b)
                      { return b.callbacks.on_load_end; }, [](BrowserInstance &b, py::object cb)
                      { b.callbacks.on_load_end = cb.is_none()
                                                      ? decltype(b.callbacks.on_load_end){}
                                                      : cb.cast<decltype(b.callbacks.on_load_end)>(); })
        .def_property("on_load_error", [](const BrowserInstance &b)
                      { return b.callbacks.on_load_error; }, [](BrowserInstance &b, py::object cb)
                      { b.callbacks.on_load_error = cb.is_none()
                                                        ? decltype(b.callbacks.on_load_error){}
                                                        : cb.cast<decltype(b.callbacks.on_load_error)>(); })
        // Display
        .def_property("on_address_change", [](const BrowserInstance &b)
                      { return b.callbacks.on_address_change; }, [](BrowserInstance &b, py::object cb)
                      { b.callbacks.on_address_change = cb.is_none()
                                                            ? decltype(b.callbacks.on_address_change){}
                                                            : cb.cast<decltype(b.callbacks.on_address_change)>(); })
        .def_property("on_title_change", [](const BrowserInstance &b)
                      { return b.callbacks.on_title_change; }, [](BrowserInstance &b, py::object cb)
                      { b.callbacks.on_title_change = cb.is_none()
                                                          ? decltype(b.callbacks.on_title_change){}
                                                          : cb.cast<decltype(b.callbacks.on_title_change)>(); })
        .def_property("on_loading_state_change", [](const BrowserInstance &b)
                      { return b.callbacks.on_loading_state_change; }, [](BrowserInstance &b, py::object cb)
                      { b.callbacks.on_loading_state_change = cb.is_none()
                                                                  ? decltype(b.callbacks.on_loading_state_change){}
                                                                  : cb.cast<decltype(b.callbacks.on_loading_state_change)>(); })
        .def_property("on_favicon_url_change", [](const BrowserInstance &b)
                      { return b.callbacks.on_favicon_url_change; }, [](BrowserInstance &b, py::object cb)
                      { b.callbacks.on_favicon_url_change = cb.is_none()
                                                                ? decltype(b.callbacks.on_favicon_url_change){}
                                                                : cb.cast<decltype(b.callbacks.on_favicon_url_change)>(); })
        .def_property("on_console_message", [](const BrowserInstance &b)
                      { return b.callbacks.on_console_message; }, [](BrowserInstance &b, py::object cb)
                      { b.callbacks.on_console_message = cb.is_none()
                                                             ? decltype(b.callbacks.on_console_message){}
                                                             : cb.cast<decltype(b.callbacks.on_console_message)>(); })
        // Download
        .def_property("on_before_download", [](const BrowserInstance &b)
                      { return b.callbacks.on_before_download; }, [](BrowserInstance &b, py::object cb)
                      { b.callbacks.on_before_download = cb.is_none()
                                                             ? decltype(b.callbacks.on_before_download){}
                                                             : cb.cast<decltype(b.callbacks.on_before_download)>(); })
        .def_property("on_download_updated", [](const BrowserInstance &b)
                      { return b.callbacks.on_download_updated; }, [](BrowserInstance &b, py::object cb)
                      { b.callbacks.on_download_updated = cb.is_none()
                                                              ? decltype(b.callbacks.on_download_updated){}
                                                              : cb.cast<decltype(b.callbacks.on_download_updated)>(); })
        // Context menu
        .def_property("on_context_menu", [](const BrowserInstance &b)
                      { return b.callbacks.on_context_menu; }, [](BrowserInstance &b, py::object cb)
                      { b.callbacks.on_context_menu = cb.is_none()
                                                          ? decltype(b.callbacks.on_context_menu){}
                                                          : cb.cast<decltype(b.callbacks.on_context_menu)>(); })
        // JS Dialogs
        .def_property("on_js_alert", [](const BrowserInstance &b)
                      { return b.callbacks.on_js_alert; }, [](BrowserInstance &b, py::object cb)
                      { b.callbacks.on_js_alert = cb.is_none()
                                                      ? decltype(b.callbacks.on_js_alert){}
                                                      : cb.cast<decltype(b.callbacks.on_js_alert)>(); })
        .def_property("on_js_confirm", [](const BrowserInstance &b)
                      { return b.callbacks.on_js_confirm; }, [](BrowserInstance &b, py::object cb)
                      { b.callbacks.on_js_confirm = cb.is_none()
                                                        ? decltype(b.callbacks.on_js_confirm){}
                                                        : cb.cast<decltype(b.callbacks.on_js_confirm)>(); })
        .def_property("on_js_prompt", [](const BrowserInstance &b)
                      { return b.callbacks.on_js_prompt; }, [](BrowserInstance &b, py::object cb)
                      { b.callbacks.on_js_prompt = cb.is_none()
                                                       ? decltype(b.callbacks.on_js_prompt){}
                                                       : cb.cast<decltype(b.callbacks.on_js_prompt)>(); })
        // Find
        .def_property("on_find_result", [](const BrowserInstance &b)
                      { return b.callbacks.on_find_result; }, [](BrowserInstance &b, py::object cb)
                      { b.callbacks.on_find_result = cb.is_none()
                                                         ? decltype(b.callbacks.on_find_result){}
                                                         : cb.cast<decltype(b.callbacks.on_find_result)>(); })
        // Fullscreen
        .def_property("on_fullscreen_mode_change", [](const BrowserInstance &b)
                      { return b.callbacks.on_fullscreen_mode_change; }, [](BrowserInstance &b, py::object cb)
                      { b.callbacks.on_fullscreen_mode_change = cb.is_none()
                                                                    ? decltype(b.callbacks.on_fullscreen_mode_change){}
                                                                    : cb.cast<decltype(b.callbacks.on_fullscreen_mode_change)>(); });

    //
    // Module-level functions
    //

    m.def(
        "initialize", [](const std::string &sub_path, const std::string &res_path, const std::string &cache_path = "")
        { return platform_initialize_cef(sub_path, res_path, cache_path); }, py::arg("worker_exe"), py::arg("resources_dir"), py::arg("cache_path") = "");

    m.def("create_browser", [](std::string url, py::object on_cpu_paint, py::object on_gpu_paint, bool shared_texture_enabled, int fps, py::object on_before_popup, py::object on_load_start, py::object on_load_end, py::object on_load_error, py::object on_address_change, py::object on_title_change, py::object on_loading_state_change, py::object on_favicon_url_change, py::object on_console_message, py::object on_before_download, py::object on_download_updated, py::object on_context_menu, py::object on_js_alert, py::object on_js_confirm, py::object on_js_prompt, py::object on_find_result, py::object on_fullscreen_mode_change) -> std::shared_ptr<BrowserInstance>
          {

        auto inst = std::make_shared<BrowserInstance>();

        // Helper: assign a py::object callback to a std::function field
        auto assign = [](auto& field, py::object& obj) {
            if (!obj.is_none()) field = obj.cast<std::decay_t<decltype(field)>>();
        };

        assign(inst->callbacks.on_cpu_paint,              on_cpu_paint);
        assign(inst->callbacks.on_gpu_paint,              on_gpu_paint);
        assign(inst->callbacks.on_before_popup,           on_before_popup);
        assign(inst->callbacks.on_load_start,             on_load_start);
        assign(inst->callbacks.on_load_end,               on_load_end);
        assign(inst->callbacks.on_load_error,             on_load_error);
        assign(inst->callbacks.on_address_change,         on_address_change);
        assign(inst->callbacks.on_title_change,           on_title_change);
        assign(inst->callbacks.on_loading_state_change,   on_loading_state_change);
        assign(inst->callbacks.on_favicon_url_change,     on_favicon_url_change);
        assign(inst->callbacks.on_console_message,        on_console_message);
        assign(inst->callbacks.on_before_download,        on_before_download);
        assign(inst->callbacks.on_download_updated,       on_download_updated);
        assign(inst->callbacks.on_context_menu,           on_context_menu);
        assign(inst->callbacks.on_js_alert,               on_js_alert);
        assign(inst->callbacks.on_js_confirm,             on_js_confirm);
        assign(inst->callbacks.on_js_prompt,              on_js_prompt);
        assign(inst->callbacks.on_find_result,            on_find_result);
        assign(inst->callbacks.on_fullscreen_mode_change, on_fullscreen_mode_change);

        CefWindowInfo window_info;
        window_info.SetAsWindowless(0);
        window_info.shared_texture_enabled = shared_texture_enabled;

        CefBrowserSettings settings;
        settings.windowless_frame_rate = fps;

        /*
        MainClient takes a raw pointer into inst->callbacks.
        inst lives on the heap (shared_ptr) and is kept alive by Python.
        */
        inst->client = new MainClient(&inst->callbacks);

        // Store a raw pointer to inst so LifeSpanHandler can assign inst->browser.
        // We temporarily keep a raw alias — safe because inst is alive.
        BrowserInstance* raw = inst.get();

        /*
        The LifeSpanHandler will set g_browser (kept for backward compat as in v0.1.0) AND
        we patch inst->browser in OnAfterCreated via the lifespan handler.
        To do that cleanly we need the instance pointer accessible from the
        handler.  We use the existing g_browser extern for the assignment and
        then copy it into inst right after creation in do_work (next pump).
        A simpler approach: override OnAfterCreated per-instance via a lambda
        stored in callbacks.
        */
        inst->callbacks._on_after_created = [raw](CefRefPtr<CefBrowser> browser) {
            raw->browser = browser;
        };

        CefBrowserHost::CreateBrowser(
            window_info, inst->client, url, settings, nullptr, nullptr);

        return inst; }, py::arg("url"), py::arg("on_cpu_paint") = py::none(), py::arg("on_gpu_paint") = py::none(), py::arg("shared_texture_enabled") = false, py::arg("fps") = 60, py::arg("on_before_popup") = py::none(), py::arg("on_load_start") = py::none(), py::arg("on_load_end") = py::none(), py::arg("on_load_error") = py::none(), py::arg("on_address_change") = py::none(), py::arg("on_title_change") = py::none(), py::arg("on_loading_state_change") = py::none(), py::arg("on_favicon_url_change") = py::none(), py::arg("on_console_message") = py::none(), py::arg("on_before_download") = py::none(), py::arg("on_download_updated") = py::none(), py::arg("on_context_menu") = py::none(), py::arg("on_js_alert") = py::none(), py::arg("on_js_confirm") = py::none(), py::arg("on_js_prompt") = py::none(), py::arg("on_find_result") = py::none(), py::arg("on_fullscreen_mode_change") = py::none());

    m.def("do_work", []()
          { CefDoMessageLoopWork(); });

    m.def("shutdown", []()
          { CefShutdown(); });

    m.def("init_graphics", &init_graphics_bridge);
}