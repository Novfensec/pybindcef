#include "handlers/include/find_handler.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

FindHandler::FindHandler(BrowserCallbacks *cb) : cb_(cb) {}

void FindHandler::OnFindResult(CefRefPtr<CefBrowser> browser,
                               int identifier,
                               int count,
                               const CefRect &selection_rect,
                               int active_match_ordinal,
                               bool final_update)
{
    if (!cb_ || !cb_->on_find_result)
        return;

    py::gil_scoped_acquire acquire;
    cb_->on_find_result(identifier, count, final_update);
}
