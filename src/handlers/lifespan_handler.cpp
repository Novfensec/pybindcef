#include "handlers/include/lifespan_handler.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

LifeSpanHandler::LifeSpanHandler(BrowserCallbacks *cb) : cb_(cb) {}

void LifeSpanHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    if (cb_ && cb_->_on_after_created)
    {
        cb_->_on_after_created(browser);
    }
}

void LifeSpanHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
}

bool LifeSpanHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    int popup_id,
                                    const CefString &target_url,
                                    const CefString &target_frame_name,
                                    WindowOpenDisposition target_disposition,
                                    bool user_gesture,
                                    const CefPopupFeatures &popupFeatures,
                                    CefWindowInfo &windowInfo,
                                    CefRefPtr<CefClient> &client,
                                    CefBrowserSettings &settings,
                                    CefRefPtr<CefDictionaryValue> &extra_info,
                                    bool *no_javascript_access)
{
    if (cb_ && cb_->on_before_popup)
    {
        py::gil_scoped_acquire acquire;
        cb_->on_before_popup(
            target_url.ToString(),
            target_frame_name.ToString(),
            static_cast<int>(target_disposition),
            user_gesture);
    }
    else
    {
        browser->GetMainFrame()->LoadURL(target_url);
    }
    return true;
}