// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "life_span_handler.h"

#include <string>
#include <windows.h>

#include "include/cef_browser.h"
#include "include/wrapper/cef_helpers.h"

void LifeSpanHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
                                  const CefString &title) {
    CEF_REQUIRE_UI_THREAD();
    CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
    SetWindowTextW(hwnd, std::wstring(title).c_str());
}
