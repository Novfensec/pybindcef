#pragma once
#include "include/cef_client.h"
#include "handlers/include/render_handler.h"
#include "handlers/include/lifespan_handler.h"
#include "handlers/include/download_handler.h"

class MainClient : public CefClient {
private:
    CefRefPtr<RenderHandler> render_handler_;
    CefRefPtr<LifeSpanHandler> lifespan_handler_;
    CefRefPtr<DownloadHandler> download_handler_;

public:
    MainClient(CefRefPtr<RenderHandler> render_handler);

    CefRefPtr<CefRenderHandler> GetRenderHandler() override;
    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
    CefRefPtr<CefDownloadHandler> GetDownloadHandler() override;
    CefRefPtr<RenderHandler> GetCustomRenderHandler();

    IMPLEMENT_REFCOUNTING(MainClient);
};