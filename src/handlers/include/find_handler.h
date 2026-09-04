#pragma once
#include "include/cef_find_handler.h"
#include "common/browser_callbacks.h"

/*
# FindHandler

Receives results from browser->GetHost()->Find(...)
*/
class FindHandler : public CefFindHandler {
public:
    explicit FindHandler(BrowserCallbacks* cb);

    void OnFindResult(CefRefPtr<CefBrowser> browser,
                      int identifier,
                      int count,
                      const CefRect& selection_rect,
                      int active_match_ordinal,
                      bool final_update) override;

    IMPLEMENT_REFCOUNTING(FindHandler);

private:
    BrowserCallbacks* cb_;
};
