#pragma once

#include <string>
#include "include/cef_app.h"
#include "include/cef_command_line.h"

class PyCefApp : public CefApp {
public:
    explicit PyCefApp(const std::string& res_path);

    void OnBeforeCommandLineProcessing(const CefString& process_type, 
                                       CefRefPtr<CefCommandLine> command_line) override;

private:
    std::string resources_path_;

    IMPLEMENT_REFCOUNTING(PyCefApp);
};
