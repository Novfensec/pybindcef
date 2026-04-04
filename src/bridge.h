#pragma once
#include "include/cef_client.h"
#include "include/cef_render_handler.h"
#include "include/cef_life_span_handler.h"
#include "common/identifiers.h"
#include <pybind11/pybind11.h>
#include <functional>
#include <vector>

namespace py = pybind11;

class MainHandler : public CefClient, public CefRenderHandler, public CefLifeSpanHandler {
public:
    int width_ = 800;
    int height_ = 600;

    using PaintCallback = std::function<void(py::object, int, int)>;
    using AccelPaintCallback = std::function<void(uint64_t, int, int)>;

    MainHandler(PaintCallback cb, AccelPaintCallback acb) : callback_(cb), accel_callback_(acb) {}

    CefRefPtr<CefRenderHandler> GetRenderHandler() override { return this; }
    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }

    void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override {
        rect = CefRect(0, 0, width_, height_);
    }

    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
                 const RectList& dirtyRects, const void* buffer,
                 int width, int height) override {
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

    void OnAfterCreated(CefRefPtr<CefBrowser> browser) override {
        py::gil_scoped_acquire acquire;
        extern CefRefPtr<CefBrowser> g_browser; 
        g_browser = browser;
    }

    void OnBeforeClose(CefRefPtr<CefBrowser> browser) override {
        py::gil_scoped_acquire acquire;
        extern CefRefPtr<CefBrowser> g_browser;
        g_browser = nullptr;
    }

    virtual void OnAcceleratedPaint(
        CefRefPtr<CefBrowser> browser,
        CefRenderHandler::PaintElementType type,
        const CefRenderHandler::RectList& dirtyRects,
        const CefAcceleratedPaintInfo& info
    ) override {
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

    IMPLEMENT_REFCOUNTING(MainHandler);

private:
    PaintCallback callback_;
    AccelPaintCallback accel_callback_;
};
