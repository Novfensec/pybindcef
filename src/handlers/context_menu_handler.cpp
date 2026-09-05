#include "handlers/include/context_menu_handler.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

ContextMenuHandler::ContextMenuHandler(BrowserCallbacks *cb) : cb_(cb) {}

void ContextMenuHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                             CefRefPtr<CefFrame> frame,
                                             CefRefPtr<CefContextMenuParams> params,
                                             CefRefPtr<CefMenuModel> model)
{
    if (!cb_ || !cb_->on_context_menu)
        return;

    py::gil_scoped_acquire acquire;
    bool suppress = cb_->on_context_menu(
        params->GetXCoord(),
        params->GetYCoord(),
        params->GetLinkUrl().ToString(),
        params->GetSelectionText().ToString(),
        params->GetSourceUrl().ToString(),
        params->GetMediaType());

    if (suppress)
    {
        model->Clear();
    }
}
