#include "main_app.h"

PyCefApp::PyCefApp(const std::string &res_path) : resources_path_(res_path) {}

void PyCefApp::OnBeforeCommandLineProcessing(const CefString &process_type,
                                             CefRefPtr<CefCommandLine> command_line)
{
    // This doesn't work. https://github.com/chromiumembedded/cef/issues/3749
    // command_line->AppendSwitchWithValue("resources-dir-path", CefString(resources_path_));

    // std::string locales_p = resources_path_ + "/locales";
    // command_line->AppendSwitchWithValue("locales-dir-path", CefString(locales_p));

    // Disable GPU compositing (offscreen rendering uses software path).
    command_line->AppendSwitch("disable-gpu");
    command_line->AppendSwitch("disable-gpu-compositing");
    command_line->AppendSwitch("disable-gpu-sandbox");

    // Allow all autoplay without requiring a user gesture first.
    // This is the correct place — CefBrowserSettings has no autoplay_policy field.
    command_line->AppendSwitchWithValue("autoplay-policy", "no-user-gesture-required");

    // Enable media features that some video players depend on.
    // HlsPlayer enables Chromium's experimental native HLS support (M120+).
    command_line->AppendSwitchWithValue(
        "enable-features",
        "AutoplayIgnoreWebAudio,MediaEngagementBypassAutoplayPolicies,HlsPlayer"
    );

    // Allow mixed content media (http video on https pages) — improves compat.
    command_line->AppendSwitch("allow-running-insecure-content");

}
