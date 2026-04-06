#include "handlers/include/download_handler.h"

bool DownloadHandler::OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefDownloadItem> download_item,
                                        const CefString& suggested_name,
                                        CefRefPtr<CefBeforeDownloadCallback> callback) {
    if (callback.get()) {
        callback->Continue(suggested_name, true);
        return true;
    }
    return false;
}

