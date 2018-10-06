//
// Created by Zen Liu on 2018-10-6.
//

#ifndef PROJECT_WINDOW_SCHEME_HANDLER_H
#define PROJECT_WINDOW_SCHEME_HANDLER_H

#include <include/cef_scheme.h>
#include "debug.h"
#include "helper_win.h"

class WindowSchemeHandler : public CefResourceHandler {
public:
    explicit WindowSchemeHandler(HWND win);

    bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;

    bool ReadResponse(void *data_out, int bytes_to_read, int &bytes_read, CefRefPtr<CefCallback> callback) override;

    void GetResponseHeaders(CefRefPtr<CefResponse> response, int64 &response_length, CefString &redirectUrl) override;

    void Cancel() override;

private:
    HWND win;
IMPLEMENT_REFCOUNTING(WindowSchemeHandler)
};

class WindowSchemeHandlerFactory : public CefSchemeHandlerFactory {
public:
    explicit WindowSchemeHandlerFactory(HWND win);
    CefRefPtr<CefResourceHandler>
    Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString &scheme_name,
           CefRefPtr<CefRequest> request) override {
        return new WindowSchemeHandler(win);
    }

private:
    HWND win;
IMPLEMENT_REFCOUNTING(WindowSchemeHandlerFactory)
};

#endif //PROJECT_WINDOW_SCHEME_HANDLER_H
