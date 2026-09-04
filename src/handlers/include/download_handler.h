#pragma once
#include "include/cef_download_handler.h"
#include "common/browser_callbacks.h"

/*
# DownloadHandler

Intercepts file downloads.
on_before_download:
  Return a non-empty path string to accept and set the save location.
  Return "" to cancel.  If callback is not set, download auto-continues
  with the browser-suggested name (original behaviour).
on_download_updated:
  Progress / completion notification for an active download.
*/
class DownloadHandler : public CefDownloadHandler {
public:
    explicit DownloadHandler(BrowserCallbacks* cb);

    bool OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefDownloadItem> download_item,
                          const CefString& suggested_name,
                          CefRefPtr<CefBeforeDownloadCallback> callback) override;

    void OnDownloadUpdated(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefDownloadItem> download_item,
                           CefRefPtr<CefDownloadItemCallback> callback) override;

    IMPLEMENT_REFCOUNTING(DownloadHandler);

private:
    BrowserCallbacks* cb_;
};
