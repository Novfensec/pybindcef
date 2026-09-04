#pragma once
#include "include/cef_client.h"
#include "handlers/include/render_handler.h"
#include "handlers/include/lifespan_handler.h"
#include "handlers/include/download_handler.h"
#include "handlers/include/load_handler.h"
#include "handlers/include/display_handler.h"
#include "handlers/include/context_menu_handler.h"
#include "handlers/include/jsdialog_handler.h"
#include "handlers/include/find_handler.h"

/*
# MainClient

CefClient aggregating all CEF handler sub-objects.
All handlers share a single BrowserCallbacks pointer that is owned by
BrowserInstance (in bridge.cpp) and outlives every handler.
*/
class MainClient : public CefClient
{
public:
    explicit MainClient(BrowserCallbacks *cb);

    CefRefPtr<CefRenderHandler> GetRenderHandler() override;
    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
    CefRefPtr<CefLoadHandler> GetLoadHandler() override;
    CefRefPtr<CefDisplayHandler> GetDisplayHandler() override;
    CefRefPtr<CefDownloadHandler> GetDownloadHandler() override;
    CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override;
    CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() override;
    CefRefPtr<CefFindHandler> GetFindHandler() override;
    CefRefPtr<RenderHandler> GetCustomRenderHandler();

    IMPLEMENT_REFCOUNTING(MainClient);

private:
    CefRefPtr<RenderHandler> render_handler_;
    CefRefPtr<LifeSpanHandler> lifespan_handler_;
    CefRefPtr<LoadHandler> load_handler_;
    CefRefPtr<DisplayHandler> display_handler_;
    CefRefPtr<DownloadHandler> download_handler_;
    CefRefPtr<ContextMenuHandler> context_menu_handler_;
    CefRefPtr<JSDialogHandler> jsdialog_handler_;
    CefRefPtr<FindHandler> find_handler_;
};