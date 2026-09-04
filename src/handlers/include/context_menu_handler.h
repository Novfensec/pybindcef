#pragma once
#include "include/cef_context_menu_handler.h"
#include "common/browser_callbacks.h"

/*
# ContextMenuHandler

Intercepts right-click context menus.
If on_context_menu returns True, the menu model is cleared so no
native CEF menu appears.
*/
class ContextMenuHandler : public CefContextMenuHandler {
public:
    explicit ContextMenuHandler(BrowserCallbacks* cb);

    void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             CefRefPtr<CefContextMenuParams> params,
                             CefRefPtr<CefMenuModel> model) override;

    IMPLEMENT_REFCOUNTING(ContextMenuHandler);

private:
    BrowserCallbacks* cb_;
};
