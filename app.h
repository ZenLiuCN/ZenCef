// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
#define CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_

#include "include/cef_app.h"
#include "go_server.h"
#include "client.h"
#include "helper_win.h"
#include "debug.h"
#include <include/wrapper/cef_helpers.h>

// Implement application-level callbacks for the browser process.
class App : public CefApp,
                  public CefBrowserProcessHandler,
                  public CefRenderProcessHandler                  {
public:
    App(std::string root,std::string port,std::string url,CefBrowserSettings browser_settings,bool enableFlash);

    // CefApp methods:
    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()
    OVERRIDE { return this; }

    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler()
    OVERRIDE { return this; }

    void OnBeforeCommandLineProcessing(const CefString &process_type, CefRefPtr<CefCommandLine> command_line) override;

    // CefBrowserProcessHandler methods:
    void OnContextInitialized() OVERRIDE;

    void OnWebKitInitialized() OVERRIDE;

    void OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar) override;

private:
    GoServer* go;
    std::string uri;
    std::string portStr;
    std::string root;
    bool enableFlash;
    CefBrowserSettings browserSettings;
    // Include the default reference counting implementation.
IMPLEMENT_REFCOUNTING(App);
};

#endif  // CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
