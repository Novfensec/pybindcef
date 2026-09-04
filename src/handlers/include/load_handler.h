#pragma once
#include "include/cef_load_handler.h"
#include "common/browser_callbacks.h"

/*
LoadHandler

Tracks page load lifecycle: start, end, and errors.
*/
class LoadHandler : public CefLoadHandler {
public:
    explicit LoadHandler(BrowserCallbacks* cb);

    void OnLoadStart(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefFrame> frame,
                     TransitionType transition_type) override;

    void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                   CefRefPtr<CefFrame> frame,
                   int http_status_code) override;

    void OnLoadError(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefFrame> frame,
                     ErrorCode error_code,
                     const CefString& error_text,
                     const CefString& failed_url) override;

    void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                              bool is_loading,
                              bool can_go_back,
                              bool can_go_forward) override;

    IMPLEMENT_REFCOUNTING(LoadHandler);

private:
    BrowserCallbacks* cb_;
};
