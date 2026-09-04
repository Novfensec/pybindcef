#pragma once
#include "include/cef_render_handler.h"
#include "common/browser_callbacks.h"

/*
# RenderHandler

Handles CEF paint callbacks (both software CPU and GPU accelerated paths).
Dispatches into Python via BrowserCallbacks::on_cpu_paint /
BrowserCallbacks::on_gpu_paint.
*/
class RenderHandler : public CefRenderHandler {
public:
    int width_  = 800;
    int height_ = 600;

    explicit RenderHandler(BrowserCallbacks* cb);

    void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;

    void OnPaint(CefRefPtr<CefBrowser> browser,
                 PaintElementType type,
                 const RectList& dirtyRects,
                 const void* buffer,
                 int width, int height) override;

    void OnAcceleratedPaint(CefRefPtr<CefBrowser> browser,
                            CefRenderHandler::PaintElementType type,
                            const CefRenderHandler::RectList& dirtyRects,
                            const CefAcceleratedPaintInfo& info) override;

    IMPLEMENT_REFCOUNTING(RenderHandler);

private:
    BrowserCallbacks* cb_;
};