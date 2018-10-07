#include <windows.h>
#include <iostream>
#include "app.h"


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


//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
// Entry point function for all processes.
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
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

    int exit_code = CefExecuteProcess(main_args, nullptr, sandbox_info);
    if (exit_code >= 0) {
        // The sub-process has completed so return here.
        return exit_code;
    }
    //</editor-fold>
    //<editor-fold desc="Hidden console Window">

    auto hwnd = GetConsoleWindow();
    LOGGER_("hidden console window %p", hwnd);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_HIDEWINDOW);


    //</editor-fold>
    //<editor-fold desc="CefSettings">
    CefSettings settings;
    settings.remote_debugging_port = 9222;
    settings.ignore_certificate_errors = 1;


#if !defined(CEF_USE_SANDBOX)
    settings.no_sandbox = true;
#endif
    //</editor-fold>
    //<editor-fold desc="BrowserSettings">
    CefBrowserSettings browser_settings;
    browser_settings.web_security=STATE_DISABLED;
    browser_settings.universal_access_from_file_urls=STATE_ENABLED;
    browser_settings.file_access_from_file_urls=STATE_ENABLED;
    //</editor-fold>
    CefRefPtr<App> app(new App("../ext", ":65530", "http://127.0.0.1:65530/",browser_settings,true));

    // Initialize CEF.
    CefInitialize(main_args, settings, app.get(), sandbox_info);

    // Run the CEF message loop. This will block until CefQuitMessageLoop() is
    // called.
    CefRunMessageLoop();

    // Shut down CEF.
    CefShutdown();

    return 0;
}
