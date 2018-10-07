//
// Created by Zen Liu on 2018-10-7.
//

#ifndef ZEN_CEF_WEBSOCKET_SCHEME_HANDLER_H__H
#define ZEN_CEF_WEBSOCKET_SCHEME_HANDLER_H__H

#include <include/cef_scheme.h>
#include "debug.h"
#include "helper_win.h"
#include "helpers.h"
#include <include/wrapper/cef_helpers.h>
#include "include/wrapper/cef_closure_task.h"
#include "include/base/cef_bind.h"
class WebSocketSchemeHandler : public CefResourceHandler {
public:
     WebSocketSchemeHandler();

    bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;

    bool ReadResponse(void *data_out, int bytes_to_read, int &bytes_read, CefRefPtr<CefCallback> callback) override;

    void GetResponseHeaders(CefRefPtr<CefResponse> response, int64 &response_length, CefString &redirectUrl) override;

    void Cancel() override;

    bool CanGetCookie(const CefCookie &cookie) override;

    bool CanSetCookie(const CefCookie &cookie) override;
    void HandleRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback);
private:
    HWND win;
IMPLEMENT_REFCOUNTING(WebSocketSchemeHandler)
};

class WebSocketSchemeHandlerFactory : public CefSchemeHandlerFactory {
public:
     WebSocketSchemeHandlerFactory();
    CefRefPtr<CefResourceHandler>
    Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString &scheme_name,
           CefRefPtr<CefRequest> request) override {
        return new WebSocketSchemeHandler();
    }

private:
IMPLEMENT_REFCOUNTING(WebSocketSchemeHandlerFactory)
};
#endif //ZEN_CEF_WEBSOCKET_SCHEME_HANDLER_H__H
