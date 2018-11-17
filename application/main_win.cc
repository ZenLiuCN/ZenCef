#include <windows.h>
#include <iostream>
#include "config.h"
#include "app.h"
#include "logger.h"


// When generating projects with CMake the CEF_USE_SANDBOX value will be defined
// automatically if using the required compiler version. Pass -DUSE_SANDBOX=OFF
// to the CMake command-line to disable use of the sandbox.
// Uncomment this line to manually enable sandbox support.
// #define CEF_USE_SANDBOX 1

#if defined(CEF_USE_SANDBOX)
// The cef_sandbox.lib static library is currently built with VS2013. It may not
// link successfully with other VS versions.
#pragma comment(lib, "cef_sandbox.lib")
#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(hPrevInstance);
    InitLogger();
    // Enable High-DPI support on Windows 7 or newer.
    CefEnableHighDPISupport();

    void *sandbox_info = nullptr;

#if defined(CEF_USE_SANDBOX)
    // Manage the life span of the sandbox information object. This is necessary
    // for sandbox support on Windows. See cef_sandbox_win.h for complete details.
    CefScopedSandboxInfo scoped_sandbox;
    sandbox_info = scoped_sandbox.sandbox_info();
#endif

    //<editor-fold desc="SubProcess Start">
    // Provide CEF with command-line arguments.
    CefMainArgs main_args(hInstance);
    //<editor-fold desc="CefSettings">
    CefSettings settings;
    settings.remote_debugging_port = conf.CEF_DEBUG_PORT;
    settings.ignore_certificate_errors = 1;
    settings.log_severity=LOGSEVERITY_DISABLE;
    std::string locale(conf.CEF_DEFAULT_LOCALE);
    cef_string_utf8_to_utf16(locale.c_str(), locale.size(), &settings.locale);
    auto cacheDir = conf.CEF_CACHE_DIR;
    cef_string_utf8_to_utf16(cacheDir.c_str(), cacheDir.size(), &settings.cache_path);
    auto logFile = conf.LOG_CEF_FILE;
    cef_string_utf8_to_utf16(logFile.c_str(), logFile.size(), &settings.log_file);
/*    LOGV<<fmt::sprintf("cef log file path %s",conf.LOG_CEF_FILE.c_str());
    LOGV<<fmt::sprintf("cef cache file path %s",conf.CEF_CACHE_DIR.c_str());*/

#if !defined(CEF_USE_SANDBOX)
    settings.no_sandbox = true;
#endif
    //</editor-fold>
    //<editor-fold desc="BrowserSettings">
    CefBrowserSettings browser_settings;
    browser_settings.web_security = STATE_DISABLED;
    browser_settings.universal_access_from_file_urls = STATE_ENABLED;
    browser_settings.file_access_from_file_urls = STATE_ENABLED;
    //</editor-fold>
#ifdef DEBUG
    CefRefPtr<App> app(new App("./ext", ":65530", "http://127.0.0.1:65530/", browser_settings, true));
//    CefRefPtr<App> app(new App("./ext", ":65530", "http://soft.hao123.com/soft/appid/29236.html", browser_settings, true));
#else
    CefRefPtr<App> app(new App("./ext", ":65530", "http://127.0.0.1:65530", browser_settings, true));
#endif
    int exit_code = CefExecuteProcess(main_args, app.get(), sandbox_info);
    if (exit_code >= 0) {
        // The sub-process has completed so return here.
        return exit_code;
    }
    //</editor-fold>
    // Initialize CEF.
    CefInitialize(main_args, settings, app.get(), sandbox_info);
    // Run the CEF message loop. This will block until CefQuitMessageLoop() is
    // called.
    CefRunMessageLoop();

    // Shut down CEF.
    CefShutdown();

    return 0;
}
