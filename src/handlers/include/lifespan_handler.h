#pragma once
#include "include/cef_life_span_handler.h"
#include "common/browser_callbacks.h"

/*
# LifeSpanHandler

Handles browser creation / destruction and popup / new-tab requests.
on_before_popup behaviour:
  - If the Python callback is NOT set: the popup URL is loaded in the same
    tab (previous default behaviour) and the native popup is suppressed.
  - If the Python callback IS set: the callback is called with (url, frame,
    disposition, user_gesture).  The C++ side still suppresses the native
    popup — it is up to the Python code to open a new CefWebView widget or
    call browser.load_url() as appropriate.
*/
class LifeSpanHandler : public CefLifeSpanHandler {
public:
    explicit LifeSpanHandler(BrowserCallbacks* cb);

    void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

    bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
                       CefRefPtr<CefFrame> frame,
                       int popup_id,
                       const CefString& target_url,
                       const CefString& target_frame_name,
                       WindowOpenDisposition target_disposition,
                       bool user_gesture,
                       const CefPopupFeatures& popupFeatures,
                       CefWindowInfo& windowInfo,
                       CefRefPtr<CefClient>& client,
                       CefBrowserSettings& settings,
                       CefRefPtr<CefDictionaryValue>& extra_info,
                       bool* no_javascript_access) override;

    IMPLEMENT_REFCOUNTING(LifeSpanHandler);

private:
    BrowserCallbacks* cb_;
};