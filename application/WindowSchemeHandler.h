//
// Created by Zen Liu on 2018-10-6.
//

#ifndef ZEN_CEF_WINDOW_SCHEME_HANDLER_H
#define ZEN_CEF_WINDOW_SCHEME_HANDLER_H

#include <include/cef_scheme.h>
#include "debug.h"
#include "helper_win.h"
#include "helpers.h"
#include <include/wrapper/cef_helpers.h>
#include "include/wrapper/cef_closure_task.h"
#include "include/base/cef_bind.h"
typedef struct state {
    int x;
    int y;
}STATE;
class WindowSchemeHandler : public CefResourceHandler {
public:
    explicit WindowSchemeHandler(HWND win);

    bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;

    bool ReadResponse(void *data_out, int bytes_to_read, int &bytes_read, CefRefPtr<CefCallback> callback) override;

    void GetResponseHeaders(CefRefPtr<CefResponse> response, int64 &response_length, CefString &redirectUrl) override;

    void Cancel() override;

    bool CanGetCookie(const CefCookie &cookie) override;

    bool CanSetCookie(const CefCookie &cookie) override;
    void HandleRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback);

private:
    HWND win;
    STATE state;
    int mx = GetSystemMetrics(SM_CXSCREEN);
    int my = GetSystemMetrics(SM_CYSCREEN);
    RECT rc;
    POINT lastPos;
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

#endif //ZEN_CEF_WINDOW_SCHEME_HANDLER_H
