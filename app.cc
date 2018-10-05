// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "app.h"

#include <iostream>
#include <string>
#include <goserver.h>
#include "debug.h"

#include "life_span_handler.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"
#include "helper_win.h"

App::App() = default;

void App::OnContextInitialized() {
    CEF_REQUIRE_UI_THREAD();

    // Information used when creating the native window.
    CefWindowInfo window_info;

#if defined(OS_WIN)
    // On Windows we need to specify certain flags that will be passed to
    // CreateWindowEx().
    window_info.SetAsPopup(nullptr, "CEF");
#endif

    // LifeSpanHandler implements browser-level callbacks.
    CefRefPtr<LifeSpanHandler> handler(new LifeSpanHandler());

    // Specify CEF browser settings here.
    CefBrowserSettings browser_settings;

    std::string url;

    // Check if a "--url=" value was provided via the command-line. If so, use
    // that instead of the default URL.
    CefRefPtr<CefCommandLine> command_line =
            CefCommandLine::GetGlobalCommandLine();
    url = command_line->GetSwitchValue("url");
    if (url.empty())
        url = "file:///" + GetAppDir() + "/test.html";

    // Create the first browser window.
    auto br = CefBrowserHost::CreateBrowserSync(window_info, handler.get(), url, browser_settings, nullptr);
    HWND win = br->GetHost()->GetWindowHandle();
    //set window
    SetWindowLong(win, GWL_STYLE, GetWindowLong(win, GWL_STYLE) ^ (WS_CAPTION));
    SetWindowPos(win, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
    LOGGER_("%p", win)
    goSetHwnd(win);
    goStartServer();
}

//TODO 2018-10-5 14:42 Zen Liu: NOT EFFECTED
void App::OnWebKitInitialized() {
    auto js = goGetExtJson();
    auto code = CefStringUTF16(js);
    std::wcout << "register js code" << code.c_str() << std::endl;
    CefRegisterExtension("v8/test", code, nullptr);
//    CefRenderProcessHandler::OnWebKitInitialized();
}

bool App::OnConsoleMessage(CefRefPtr<CefBrowser> browser, const CefString &message, const CefString &source,
                           int line) {
    std::wcout << message.c_str() << std::endl;
    return false;
}
