#include "handlers/include/lifespan_handler.h"

void LifeSpanHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    g_browser = browser;
}

void LifeSpanHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    g_browser = nullptr;
}

bool LifeSpanHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser,
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
                   bool* no_javascript_access) {
    browser->GetMainFrame()->LoadURL(target_url);
    return true; 
}