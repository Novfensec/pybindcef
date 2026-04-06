#include "main_client.h"

MainClient::MainClient(CefRefPtr<RenderHandler> render_handler) {
    render_handler_ = render_handler;
    lifespan_handler_ = new LifeSpanHandler();
    download_handler_ = new DownloadHandler();
}

CefRefPtr<CefRenderHandler> MainClient::GetRenderHandler() { return render_handler_; }
CefRefPtr<CefLifeSpanHandler> MainClient::GetLifeSpanHandler() { return lifespan_handler_; }
CefRefPtr<CefDownloadHandler> MainClient::GetDownloadHandler() { return download_handler_; }
CefRefPtr<RenderHandler> MainClient::GetCustomRenderHandler() { return render_handler_; }