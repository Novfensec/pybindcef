#include "include/cef_client.h"
#include "include/cef_render_handler.h"
#include <pybind11/pybind11.h>
#include <functional>
#include <vector>

namespace py = pybind11;

class KivyHandler : public CefClient, public CefRenderHandler {
public:
    using PaintCallback = std::function<void(py::object, int, int)>;
    
    KivyHandler(PaintCallback cb) : callback_(cb) {}

    CefRefPtr<CefRenderHandler> GetRenderHandler() override { return this; }

    void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override {
        rect = CefRect(0, 0, 800, 600);
    }

    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
                 const RectList& dirtyRects, const void* buffer,
                 int width, int height) override {
        if (callback_) {
            size_t size = static_cast<size_t>(width) * height * 4;

            // Use const_cast to convert 'const void*' to 'void*' 
            // We set readonly=true (the third argument) to stay safe
            auto mview = py::memoryview::from_memory(
                const_cast<void*>(buffer), 
                static_cast<ssize_t>(size), 
                false
            );

            callback_(mview, width, height);
        }
    }

    IMPLEMENT_REFCOUNTING(KivyHandler);
private:
    PaintCallback callback_;
};
