#include "main_app.h"

PyCefApp::PyCefApp(const std::string& res_path) : resources_path_(res_path) {}

void PyCefApp::OnBeforeCommandLineProcessing(const CefString& process_type, 
                                             CefRefPtr<CefCommandLine> command_line) {
    // This doesn't work. https://github.com/chromiumembedded/cef/issues/3749
    // command_line->AppendSwitchWithValue("resources-dir-path", CefString(resources_path_));

    // std::string locales_p = resources_path_ + "/locales";
    // command_line->AppendSwitchWithValue("locales-dir-path", CefString(locales_p));
}
