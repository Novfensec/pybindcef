#include "platform_utils.h"
#include "main_app.h"

// iOS CEF initialization
// Note: On iOS, CEF wraps WKWebView due to Apple's requirement that all
// browser engines on iOS must use WebKit. The CEF API surface is maintained
// but the underlying implementation differs from desktop platforms.

bool platform_initialize_cef(const std::string& sub_path, const std::string& res_path) {
    // On iOS, CefMainArgs takes the main function arguments
    // In practice, these come from UIApplicationMain
    int argc = 0;
    char** argv = nullptr;
    CefMainArgs args(argc, argv);

    CefSettings settings;
    settings.no_sandbox = true;
    settings.windowless_rendering_enabled = true;
    settings.external_message_pump = true; // iOS requires external message pump
    settings.multi_threaded_message_loop = false;

    std::string cache_p = res_path + "/web_cache";
    CefString(&settings.cache_path).FromASCII(cache_p.c_str());

    // On iOS, subprocess is not used (single-process mode)
    // CefString(&settings.browser_subprocess_path).FromASCII(sub_path.c_str());

    std::string log_p = res_path + "/cef.log";
    CefString(&settings.log_file).FromASCII(log_p.c_str());

    CefRefPtr<PyCefApp> app(new PyCefApp(res_path));

    return CefInitialize(args, settings, app, nullptr);
}
