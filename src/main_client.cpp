#include "main_client.h"

MainClient::MainClient(BrowserCallbacks *cb)
{
    render_handler_ = new RenderHandler(cb);
    lifespan_handler_ = new LifeSpanHandler(cb);
    load_handler_ = new LoadHandler(cb);
    display_handler_ = new DisplayHandler(cb);
    download_handler_ = new DownloadHandler(cb);
    context_menu_handler_ = new ContextMenuHandler(cb);
    jsdialog_handler_ = new JSDialogHandler(cb);
    find_handler_ = new FindHandler(cb);
}

CefRefPtr<CefRenderHandler> MainClient::GetRenderHandler() { return render_handler_; }
CefRefPtr<CefLifeSpanHandler> MainClient::GetLifeSpanHandler() { return lifespan_handler_; }
CefRefPtr<CefLoadHandler> MainClient::GetLoadHandler() { return load_handler_; }
CefRefPtr<CefDisplayHandler> MainClient::GetDisplayHandler() { return display_handler_; }
CefRefPtr<CefDownloadHandler> MainClient::GetDownloadHandler() { return download_handler_; }
CefRefPtr<CefContextMenuHandler> MainClient::GetContextMenuHandler() { return context_menu_handler_; }
CefRefPtr<CefJSDialogHandler> MainClient::GetJSDialogHandler() { return jsdialog_handler_; }
CefRefPtr<CefFindHandler> MainClient::GetFindHandler() { return find_handler_; }
CefRefPtr<RenderHandler> MainClient::GetCustomRenderHandler() { return render_handler_; }