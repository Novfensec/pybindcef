#include "handlers/include/request_handler.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

RequestHandler::RequestHandler(BrowserCallbacks *cb) : cb_(cb) {}

CefRefPtr<CefResourceRequestHandler>
RequestHandler::GetResourceRequestHandler(
    CefRefPtr<CefBrowser>,
    CefRefPtr<CefFrame>,
    CefRefPtr<CefRequest>,
    bool,
    bool,
    const CefString &,
    bool &)
{
    // Only intercept if a Python filter is registered.
    if (cb_ && cb_->on_before_resource_load)
        return this;
    return nullptr;
}

CefResourceRequestHandler::ReturnValue
RequestHandler::OnBeforeResourceLoad(
    CefRefPtr<CefBrowser>,
    CefRefPtr<CefFrame>,
    CefRefPtr<CefRequest> request,
    CefRefPtr<CefCallback>)
{
    if (!cb_ || !cb_->on_before_resource_load)
        return RV_CONTINUE;

    std::string url = request->GetURL().ToString();
    int resource_type = static_cast<int>(request->GetResourceType());

    py::gil_scoped_acquire acquire;
    bool blocked = cb_->on_before_resource_load(url, resource_type);
    return blocked ? RV_CANCEL : RV_CONTINUE;
}
