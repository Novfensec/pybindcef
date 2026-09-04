# Changelog

All notable changes to this project will be documented in this file.

## [latest]

### Added
- **Load Handler:** Introduced `load_handler` to manage the state of webpage loading.
- **JS Dialog Handler:** Introduced `js_dialog_handler` to manage JavaScript dialogs (e.g., alerts, confirms, prompts).
- **Find Handler:** Added `find_handler` for finding text in the page.
- **Display Handler:** Introduced `display_handler` to handle display events.
- **Context Menu Handler:** Added `context_menu_handler` for handling context menus.
- **Zoom Level API:** Introduced support to set the zoom level of the browser.
- **Multiple Browser Windows:** Added `BrowserCallbacksStruct` to support multi-browser window architectures.
- **Build Scripts:** Added standard build scripts to resolve manual build steps.

### Changed
- **Major API Overhaul:** Significant changes and refactoring in the core API and `src/bridge.cpp` bridging layer.
- **Package Installation:** Standardized the package install process.
- **Examples & Documentation:** Updated examples and README to reflect the new API structure and features.
- **Formatting:** Code formatting improvements across the codebase.

## [0.1.0]
Initial release.
