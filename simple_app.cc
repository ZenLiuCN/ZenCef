// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "simple_app.h"

#include <iostream>
#include <string>
#include <goserver.h>

#include "simple_handler.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"

SimpleApp::SimpleApp() = default;

void SimpleApp::OnContextInitialized() {
    CEF_REQUIRE_UI_THREAD();

    // Information used when creating the native window.
    CefWindowInfo window_info;

#if defined(OS_WIN)
    // On Windows we need to specify certain flags that will be passed to
    // CreateWindowEx().
    window_info.SetAsPopup(nullptr, "CEF");
#endif

    // SimpleHandler implements browser-level callbacks.
    CefRefPtr<SimpleHandler> handler(new SimpleHandler());

    // Specify CEF browser settings here.
    CefBrowserSettings browser_settings;

    std::string url;

    // Check if a "--url=" value was provided via the command-line. If so, use
    // that instead of the default URL.
    CefRefPtr<CefCommandLine> command_line =
            CefCommandLine::GetGlobalCommandLine();
    url = command_line->GetSwitchValue("url");
    if (url.empty())
        url = "file:///D:/Projects/Project/go/cef-go/bin/test.html";

    // Create the first browser window.
    auto br = CefBrowserHost::CreateBrowserSync(window_info, handler.get(), url, browser_settings, nullptr);
    HWND win = br->GetHost()->GetWindowHandle();
    std::cout << win << std::endl;
    goSetHwnd(win);
    goStartServer();
}

////TODO 2018-10-5 14:42 Zen Liu: NOT EFFECTED
void SimpleApp::OnWebKitInitialized() {
        auto js=goGetExtJson();
        auto code=CefStringUTF16(js);
    std::wcout <<"register js code" <<code.c_str() << std::endl;
     CefRegisterExtension("v8/test", code, nullptr);
//    CefRenderProcessHandler::OnWebKitInitialized();
}

bool SimpleApp::OnConsoleMessage(CefRefPtr<CefBrowser> browser, const CefString &message, const CefString &source,
                                 int line) {
    std::wcout << message.c_str() << std::endl;
    return false;
}
