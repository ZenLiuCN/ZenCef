// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "client.h"
#include "debug.h"

void Client::OnTitleChange(CefRefPtr<CefBrowser> browser,
                           const CefString &title) {
    CEF_REQUIRE_UI_THREAD();
    CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
    SetWindowTextW(hwnd, std::wstring(title).c_str());
}

bool Client::OnKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent &event, MSG *os_event) {
    switch (event.windows_key_code) {
        case VK_F5:
           if(event.type==KEYEVENT_RAWKEYDOWN && event.modifiers==EVENTFLAG_ALT_DOWN){
               browser->Reload();
               return true;
           }
            return false;
        case VK_F10:
            if(event.type==KEYEVENT_RAWKEYDOWN&& event.modifiers==EVENTFLAG_ALT_DOWN){
                ShowDevTool(browser);
                return true;
            }
            return false;
        default:
            return CefKeyboardHandler::OnKeyEvent(browser, event, os_event);
    }

}

void Client::ShowDevTool(const CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();
    LOGGER_("will show dev tools %p ",browser.get())
    CefWindowInfo wi;
    wi.transparent_painting_enabled=1;
    CefBrowserSettings bs;
    wi.SetAsPopup(nullptr, "DEV_TOOLS");
    browser->GetHost()->ShowDevTools(wi, this, bs, CefPoint(0, 0));
}
