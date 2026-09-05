#include "handlers/include/download_handler.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

DownloadHandler::DownloadHandler(BrowserCallbacks *cb) : cb_(cb) {}

bool DownloadHandler::OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefDownloadItem> download_item,
                                       const CefString &suggested_name,
                                       CefRefPtr<CefBeforeDownloadCallback> callback)
{
    if (!callback.get())
        return false;

    if (cb_ && cb_->on_before_download)
    {
        py::gil_scoped_acquire acquire;
        std::string save_path = cb_->on_before_download(
            suggested_name.ToString(),
            download_item->GetURL().ToString(),
            download_item->GetMimeType().ToString());
        if (!save_path.empty())
        {
            callback->Continue(CefString(save_path), false);
        }
        else
        {
            callback->Continue(CefString(), false);
        }
    }
    else
    {
        callback->Continue(suggested_name, true);
    }
    return true;
}

void DownloadHandler::OnDownloadUpdated(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefDownloadItem> download_item,
                                        CefRefPtr<CefDownloadItemCallback> callback)
{
    if (!cb_ || !cb_->on_download_updated)
        return;

    py::gil_scoped_acquire acquire;
    cb_->on_download_updated(
        download_item->GetFullPath().ToString(),
        download_item->GetTotalBytes(),
        download_item->GetReceivedBytes(),
        download_item->IsComplete(),
        download_item->IsCanceled());
}
