//
// Created by Zen Liu on 2018-10-7.
//

#include "WebSocketSchemeHandler.h"


WebSocketSchemeHandlerFactory::WebSocketSchemeHandlerFactory() = default;

WebSocketSchemeHandler::WebSocketSchemeHandler() = default;

bool WebSocketSchemeHandler::ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) {
    WCHAR_LOGGER_(L"%ls\n",DumpRequestContents(request).c_str())
    return false;
}

bool WebSocketSchemeHandler::ReadResponse(void *data_out, int bytes_to_read, int &bytes_read,
                                          CefRefPtr<CefCallback> callback) {
    return false;
}

void WebSocketSchemeHandler::Cancel() {

}

void WebSocketSchemeHandler::GetResponseHeaders(CefRefPtr<CefResponse> response, int64 &response_length,
                                                CefString &redirectUrl) {

}

bool WebSocketSchemeHandler::CanGetCookie(const CefCookie &cookie) {
    return CefResourceHandler::CanGetCookie(cookie);
}

bool WebSocketSchemeHandler::CanSetCookie(const CefCookie &cookie) {
    return CefResourceHandler::CanSetCookie(cookie);
}

void WebSocketSchemeHandler::HandleRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) {

}
