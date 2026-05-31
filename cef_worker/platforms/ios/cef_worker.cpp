// CEF Worker for iOS
// Note: On iOS, Apple requires single-process mode for all browser engines.
// A separate subprocess (like on desktop) is NOT permitted on iOS.
// This file is provided as a stub for API compatibility.
//
// On iOS, the CEF framework operates in single-process mode within the
// main application process. WKWebView (WebKit) handles all rendering
// internally through the system's Web Content process.

#include <iostream>

int main(int argc, char* argv[]) {
    // On iOS, this subprocess executable is not used.
    // CEF runs in single-process mode due to iOS sandbox restrictions.
    std::cerr << "cef_worker: iOS does not support multi-process mode. "
              << "CEF runs within the main app process." << std::endl;
    return 1;
}
