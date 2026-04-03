#include "include/cef_client.h"
#include "include/cef_render_handler.h"
#include "include/cef_life_span_handler.h"
#include <pybind11/pybind11.h>
#include <functional>
#include <vector>

namespace py = pybind11;

class KivyHandler : public CefClient, public CefRenderHandler, public CefLifeSpanHandler {
public:
    int width_ = 800;
    int height_ = 600;

    using PaintCallback = std::function<void(py::object, int, int)>;

    KivyHandler(PaintCallback cb) : callback_(cb) {}

    CefRefPtr<CefRenderHandler> GetRenderHandler() override { return this; }
    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }

    void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override {
        rect = CefRect(0, 0, width_, height_);
    }

    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
                 const RectList& dirtyRects, const void* buffer,
                 int width, int height) override {
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
        extern CefRefPtr<CefBrowser> g_browser; 
        g_browser = browser;
    }

    void OnBeforeClose(CefRefPtr<CefBrowser> browser) override {
        extern CefRefPtr<CefBrowser> g_browser;
        g_browser = nullptr;
    }

    IMPLEMENT_REFCOUNTING(KivyHandler);

private:
    PaintCallback callback_;
};