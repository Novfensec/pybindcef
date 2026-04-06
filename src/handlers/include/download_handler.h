#pragma once
#include "include/cef_download_handler.h"

class DownloadHandler : public CefDownloadHandler {

public:
    bool OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefDownloadItem> download_item,
                          const CefString& suggested_name,
                          CefRefPtr<CefBeforeDownloadCallback> callback) override;

    IMPLEMENT_REFCOUNTING(DownloadHandler);
};
