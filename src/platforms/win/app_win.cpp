#include "platform_utils.h"
#include "include/cef_app.h"

bool platform_initialize_cef(const std::string& sub_path, const std::string& res_path) {

    HINSTANCE hInstance = GetModuleHandle(NULL);
    CefMainArgs args(hInstance);

    CefSettings settings;
    settings.no_sandbox = true;
    settings.windowless_rendering_enabled = true;
    settings.external_message_pump = false;
    settings.multi_threaded_message_loop = false;

    std::string cache_p = res_path + "/web_cache";
    CefString(&settings.cache_path).FromASCII(cache_p.c_str());
    CefString(&settings.browser_subprocess_path).FromASCII(sub_path.c_str());

    std::string log_p = res_path + "/cef.log";
    CefString(&settings.log_file).FromASCII(log_p.c_str());

    return CefInitialize(args, settings, nullptr, nullptr);
}