#include "handlers/include/jsdialog_handler.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

JSDialogHandler::JSDialogHandler(BrowserCallbacks *cb) : cb_(cb) {}

bool JSDialogHandler::OnJSDialog(CefRefPtr<CefBrowser> browser,
                                 const CefString &origin_url,
                                 JSDialogType dialog_type,
                                 const CefString &message_text,
                                 const CefString &default_prompt_text,
                                 CefRefPtr<CefJSDialogCallback> callback,
                                 bool &suppress_message)
{
    if (!cb_)
        return false;

    py::gil_scoped_acquire acquire;
    bool handled = false;

    if (dialog_type == JSDIALOGTYPE_ALERT && cb_->on_js_alert)
    {
        handled = cb_->on_js_alert(message_text.ToString());
    }
    else if (dialog_type == JSDIALOGTYPE_CONFIRM && cb_->on_js_confirm)
    {
        handled = cb_->on_js_confirm(message_text.ToString());
    }
    else if (dialog_type == JSDIALOGTYPE_PROMPT && cb_->on_js_prompt)
    {
        handled = cb_->on_js_prompt(
            message_text.ToString(),
            default_prompt_text.ToString());
    }

    if (handled)
    {
        suppress_message = true;
        callback->Continue(true, CefString());
        return true;
    }

    return false;
}
