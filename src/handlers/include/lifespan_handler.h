#pragma once
#include "include/cef_life_span_handler.h"

extern CefRefPtr<CefBrowser> g_browser;

class LifeSpanHandler : public CefLifeSpanHandler {
public:
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
};