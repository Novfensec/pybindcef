#include "handlers/include/render_handler.h"
#include "common/identifiers.h"

RenderHandler::RenderHandler(PaintCallback cb, AccelPaintCallback acb) 
    : callback_(cb), accel_callback_(acb) {}

void RenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
    rect = CefRect(0, 0, width_, height_);
}

void RenderHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
             const RectList& dirtyRects, const void* buffer,
             int width, int height) {
    py::gil_scoped_acquire acquire;
    if (callback_ && buffer) {
        size_t size = static_cast<size_t>(width) * height * 4;
        auto mview = py::memoryview::from_memory(
            const_cast<void*>(buffer), 
            static_cast<ssize_t>(size), 
            false
        );
        callback_(mview, width, height);
    }
}

void RenderHandler::OnAcceleratedPaint(CefRefPtr<CefBrowser> browser,
                        CefRenderHandler::PaintElementType type,
                        const CefRenderHandler::RectList& dirtyRects,
                        const CefAcceleratedPaintInfo& info) {
    py::gil_scoped_acquire acquire;
    if (accel_callback_) {
        uint64_t handle_id = 0;

        #if defined(_WIN32)
            handle_id = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(info.shared_texture_handle));
        #elif defined(__APPLE__)
            #include <TargetConditionals.h>
            #if TARGET_OS_IPHONE
                handle_id = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(info.shared_texture_io_surface));
            #else
                handle_id = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(info.shared_texture_io_surface));
            #endif
        #elif defined(__ANDROID__)
            handle_id = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(info.shared_texture_handle));
        #elif defined(__linux__)
            handle_id = static_cast<uint64_t>(info.planes[0].fd);
        #endif

        accel_callback_(handle_id, width_, height_); 
    }
}