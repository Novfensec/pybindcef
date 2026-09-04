#include "handlers/include/display_handler.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

DisplayHandler::DisplayHandler(BrowserCallbacks *cb) : cb_(cb) {}

void DisplayHandler::OnAddressChange(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     const CefString &url)
{
    if (!frame->IsMain() || !cb_ || !cb_->on_address_change)
        return;

    py::gil_scoped_acquire acquire;
    cb_->on_address_change(url.ToString());
}

void DisplayHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
                                   const CefString &title)
{
    if (!cb_ || !cb_->on_title_change)
        return;

    py::gil_scoped_acquire acquire;
    cb_->on_title_change(title.ToString());
}

void DisplayHandler::OnFaviconURLChange(CefRefPtr<CefBrowser> browser,
                                        const std::vector<CefString> &icon_urls)
{
    if (!cb_ || !cb_->on_favicon_url_change || icon_urls.empty())
        return;

    py::gil_scoped_acquire acquire;
    cb_->on_favicon_url_change(icon_urls[0].ToString());
}

bool DisplayHandler::OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                                      cef_log_severity_t level,
                                      const CefString &message,
                                      const CefString &source,
                                      int line)
{
    if (!cb_ || !cb_->on_console_message)
        return false;

    py::gil_scoped_acquire acquire;
    return cb_->on_console_message(
        static_cast<int>(level),
        message.ToString(),
        source.ToString(),
        line);
}
