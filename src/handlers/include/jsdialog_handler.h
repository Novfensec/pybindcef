#pragma once
#include "include/cef_jsdialog_handler.h"
#include "common/browser_callbacks.h"

/*
# JSDialogHandler

Intercepts JavaScript alert(), confirm(), and prompt() dialogs.
If the corresponding Python callback returns True, the dialog is suppressed
(callback->Continue(true/false, "") is called automatically).
If the callback is not set, CEF shows its default dialog.
*/
class JSDialogHandler : public CefJSDialogHandler {
public:
    explicit JSDialogHandler(BrowserCallbacks* cb);

    bool OnJSDialog(CefRefPtr<CefBrowser> browser,
                    const CefString& origin_url,
                    JSDialogType dialog_type,
                    const CefString& message_text,
                    const CefString& default_prompt_text,
                    CefRefPtr<CefJSDialogCallback> callback,
                    bool& suppress_message) override;

    IMPLEMENT_REFCOUNTING(JSDialogHandler);

private:
    BrowserCallbacks* cb_;
};
