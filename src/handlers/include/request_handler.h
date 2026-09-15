#pragma once
#include "include/cef_request_handler.h"
#include "include/cef_resource_request_handler.h"
#include "common/browser_callbacks.h"

/*
# RequestHandler

Intercepts every network request before it is sent.
Calls BrowserCallbacks::on_before_resource_load(url, resource_type) which
returns true to cancel (block) the request, false to allow it.

resource_type integer mirrors cef_resource_type_t:
  0=main_frame  1=sub_frame  2=stylesheet  3=script  4=image
  5=font        6=sub_resource 7=object    8=media   13=xhr  ...
*/
class RequestHandler : public CefRequestHandler,
                       public CefResourceRequestHandler
{
public:
    explicit RequestHandler(BrowserCallbacks *cb);

    CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        bool is_navigation,
        bool is_download,
        const CefString &request_initiator,
        bool &disable_default_handling) override;

    CefResourceRequestHandler::ReturnValue OnBeforeResourceLoad(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        CefRefPtr<CefCallback> callback) override;

    IMPLEMENT_REFCOUNTING(RequestHandler);

private:
    BrowserCallbacks *cb_;
};
