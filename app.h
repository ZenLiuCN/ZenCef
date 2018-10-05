// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
#define CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_

#include "include/cef_app.h"

// Implement application-level callbacks for the browser process.
class App : public CefApp,
                  public CefBrowserProcessHandler,
                  public CefRenderProcessHandler                  {
public:
    App();

    // CefApp methods:
    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()
    OVERRIDE { return this; }

    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler()
    OVERRIDE { return this; }

    // CefBrowserProcessHandler methods:
    void OnContextInitialized() OVERRIDE;

    void OnWebKitInitialized() OVERRIDE;
     static bool OnConsoleMessage(CefRefPtr<CefBrowser> browser, const CefString &message, const CefString &source,
                                  int line);
private:
    // Include the default reference counting implementation.
IMPLEMENT_REFCOUNTING(App);
};

#endif  // CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
