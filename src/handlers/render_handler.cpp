#include "handlers/include/render_handler.h"
#include "common/identifiers.h"

RenderHandler::RenderHandler(BrowserCallbacks *cb) : cb_(cb) {}

void RenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
{
    rect = CefRect(0, 0, width_, height_);
}

void RenderHandler::OnPaint(CefRefPtr<CefBrowser> browser,
                            PaintElementType type,
                            const RectList &dirtyRects,
                            const void *buffer,
                            int width, int height)
{
    if (!cb_ || !cb_->on_cpu_paint || !buffer)
        return;

    py::gil_scoped_acquire acquire;
    size_t size = static_cast<size_t>(width) * height * 4;
    py::bytes data(static_cast<const char *>(buffer), size);
    cb_->on_cpu_paint(data, width, height);
}

void RenderHandler::OnAcceleratedPaint(CefRefPtr<CefBrowser> browser,
                                       CefRenderHandler::PaintElementType type,
                                       const CefRenderHandler::RectList &dirtyRects,
                                       const CefAcceleratedPaintInfo &info)
{
    if (!cb_ || !cb_->on_gpu_paint)
        return;

    py::gil_scoped_acquire acquire;
    uint64_t handle_id = 0;

#if defined(_WIN32)
    handle_id = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(info.shared_texture_handle));
#elif defined(__APPLE__)
    handle_id = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(info.shared_texture_io_surface));
#elif defined(__linux__)
    handle_id = static_cast<uint64_t>(info.planes[0].fd);
#endif

    cb_->on_gpu_paint(handle_id, width_, height_);
}