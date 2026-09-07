#include "include/capi/cef_app_capi.h"
#include <windows.h>

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    cef_main_args_t args;
    args.instance = hInstance;
    return cef_execute_process(&args, nullptr, nullptr);
}