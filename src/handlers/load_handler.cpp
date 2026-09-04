#include "handlers/include/load_handler.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

LoadHandler::LoadHandler(BrowserCallbacks *cb) : cb_(cb) {}

void LoadHandler::OnLoadStart(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame,
                              TransitionType transition_type)
{
    if (!frame->IsMain() || !cb_ || !cb_->on_load_start)
        return;

    py::gil_scoped_acquire acquire;
    cb_->on_load_start(0, frame->GetURL().ToString());
}

void LoadHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser,
                            CefRefPtr<CefFrame> frame,
                            int http_status_code)
{
    if (!frame->IsMain() || !cb_ || !cb_->on_load_end)
        return;

    py::gil_scoped_acquire acquire;
    cb_->on_load_end(http_status_code, frame->GetURL().ToString());
}

void LoadHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame,
                              ErrorCode error_code,
                              const CefString &error_text,
                              const CefString &failed_url)
{
    if (!frame->IsMain() || !cb_ || !cb_->on_load_error)
        return;

    py::gil_scoped_acquire acquire;
    cb_->on_load_error(
        static_cast<int>(error_code),
        error_text.ToString(),
        failed_url.ToString());
}

void LoadHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                       bool is_loading,
                                       bool can_go_back,
                                       bool can_go_forward)
{
    if (!cb_ || !cb_->on_loading_state_change)
        return;

    py::gil_scoped_acquire acquire;
    cb_->on_loading_state_change(is_loading, can_go_back, can_go_forward);
}
