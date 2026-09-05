#pragma once
#include <functional>
#include <string>
#include <optional>
#include <pybind11/pybind11.h>

namespace py = pybind11;

/*
# BrowserCallbacks

Aggregates every Python-visible callback for a single browser instance.
All fields are optional std::function — unset (null) means "use default
CEF behaviour" for that event.

Ownership: one BrowserCallbacks object lives inside BrowserInstance in
bridge.cpp and is handed to each handler as a raw non-owning pointer.
The BrowserInstance outlives all handlers, so the pointer is always valid.
*/
struct BrowserCallbacks {
    std::function<void(py::object, int, int)>  on_cpu_paint;

    std::function<void(uint64_t, int, int)>    on_gpu_paint;

    /*
    Fired when a page tries to open a new tab/window (target=_blank etc.).
    args: (url: str, target_frame_name: str, disposition: int, user_gesture: bool)
    
    disposition values (WindowOpenDisposition):
    0 = unknown, 1 = current_tab, 3 = new_foreground_tab,
    4 = new_background_tab, 5 = new_popup, 6 = new_window, 7 = save_to_disk
    
    The handler always suppresses the native popup; the C++ side then calls
    browser->GetMainFrame()->LoadURL(url) unless this callback handles it
    differently.  If you want to open it in a *new* CefWebView, do so
    inside this callback — the C++ side won't load anything when this is set.
    */
    std::function<void(std::string url,
                       std::string target_frame_name,
                       int         disposition,
                       bool        user_gesture)>  on_before_popup;

    // args: (http_status_code: int, url: str)
    std::function<void(int, std::string)> on_load_start;

    // args: (http_status_code: int, url: str)
    std::function<void(int, std::string)> on_load_end;

    // args: (error_code: int, error_text: str, failed_url: str)
    std::function<void(int, std::string, std::string)> on_load_error;

    // args: (url: str)
    std::function<void(std::string)> on_address_change;

    // args: (title: str)
    std::function<void(std::string)> on_title_change;

    // args: (is_loading: bool, can_go_back: bool, can_go_forward: bool)
    std::function<void(bool, bool, bool)> on_loading_state_change;

    // args: (favicon_url: str)  — first favicon URL from the page
    std::function<void(std::string)> on_favicon_url_change;

    /*
    Called for every console.log / console.error / etc. from the page.
    args: (level: int, message: str, source: str, line: int)
    level: 0=debug, 1=info, 2=warning, 3=error
    Return True to suppress CEF's own console output.
    */
    std::function<bool(int, std::string, std::string, int)> on_console_message;

    /*
    Called before a download starts.
    args: (suggested_name: str, url: str, mime_type: str)
    Return a non-empty string = absolute path to save the file.
    Return "" = cancel the download.
    */
    std::function<std::string(std::string, std::string, std::string)> on_before_download;

    /*
    Progress / completion updates for an active download.
    args: (path: str, total_bytes: int, received_bytes: int,
            is_complete: bool, is_canceled: bool)
    */
    std::function<void(std::string, int64_t, int64_t, bool, bool)> on_download_updated;

    /*
    Called just before the right-click context menu is shown.
    args: (x: int, y: int, link_url: str, selection_text: str, source_url: str, media_type: int)
    Return True to suppress the native CEF context menu entirely.
    */
    std::function<bool(int, int, std::string, std::string, std::string, int)> on_context_menu;

    // args: (message: str)  — Return True to suppress CEF's native dialog.
    std::function<bool(std::string)> on_js_alert;

    // args: (message: str)  — Return True to suppress CEF's native dialog.
    std::function<bool(std::string)> on_js_confirm;

    // args: (message: str, default_value: str)  — Return True to suppress.
    std::function<bool(std::string, std::string)> on_js_prompt;

    // args: (identifier: int, count: int, is_final_update: bool)
    std::function<void(int, int, bool)> on_find_result;

    // args: (fullscreen: bool)
    std::function<void(bool)> on_fullscreen_mode_change;

    /// Called by LifeSpanHandler::OnAfterCreated to wire up inst->browser.
    std::function<void(CefRefPtr<CefBrowser>)> _on_after_created;

    // Convenience type aliases used in bridge.cpp property setters.
    using PaintCb = std::function<void(py::object, int, int)>;
    using AccelCb = std::function<void(uint64_t, int, int)>;
};
