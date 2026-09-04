#pragma once
#include "include/cef_display_handler.h"
#include "common/browser_callbacks.h"

/*
# DisplayHandler

Tracks UI display state: address bar, title, loading spinner,
favicons, and console messages from the page.
*/
class DisplayHandler : public CefDisplayHandler {
public:
    explicit DisplayHandler(BrowserCallbacks* cb);

    void OnAddressChange(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         const CefString& url) override;

    void OnTitleChange(CefRefPtr<CefBrowser> browser,
                       const CefString& title) override;

    void OnFaviconURLChange(CefRefPtr<CefBrowser> browser,
                            const std::vector<CefString>& icon_urls) override;

    bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                          cef_log_severity_t level,
                          const CefString& message,
                          const CefString& source,
                          int line) override;

    IMPLEMENT_REFCOUNTING(DisplayHandler);

private:
    BrowserCallbacks* cb_;
};
