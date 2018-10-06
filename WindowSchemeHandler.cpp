//
// Created by Zen Liu on 2018-10-6.
//

#include "WindowSchemeHandler.h"

bool WindowSchemeHandler::ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) {
    // Evaluate |request| to determine proper handling...
    // Execute |callback| once header information is available.
    // Return true to handle the request
    CefPostData::ElementVector ele;
    request->GetPostData()->GetElements(ele);
    if (ele.empty()) {
        GO_LOGGER_("request local scheme %s \n", cefSourceToString(request->GetURL()));
    } else {
/*        int count = ele[0]->GetBytesCount();
        BYTE b[count];
        ele[0]->GetBytes(static_cast<size_t>(count), b);
        std::wstring_convert<std::codecvt_utf8<wchar_t>,wchar_t> convert;
        auto str= std::string(reinterpret_cast<char*>(b),(unsigned int)count);
        GO_LOGGER_("request local scheme %s \n %ls \n", cefSourceToString(request->GetURL()),convert.from_bytes(str).c_str());*/
/*        auto count = request->GetPostData()->GetElementCount();
        GO_LOGGER_("request local scheme %s ,body length %d\n", cefSourceToString(request->GetURL()), count);*/
    }


    return true;
}

bool
WindowSchemeHandler::ReadResponse(void *data_out, int bytes_to_read, int &bytes_read, CefRefPtr<CefCallback> callback) {
    LOGGER_("write local scheme")
    return false;
}

void WindowSchemeHandler::GetResponseHeaders(CefRefPtr<CefResponse> response, int64 &response_length,
                                             CefString &redirectUrl) {
    // Populate the response headers.
    response->SetMimeType("text/html");
    response->SetStatus(200);
    //Specify the resulting response length.
    response_length = 0;
}

void WindowSchemeHandler::Cancel() {
    LOGGER_("request canceled")
}

WindowSchemeHandler::WindowSchemeHandler(HWND win) {
    this->win = win;
};

WindowSchemeHandlerFactory::WindowSchemeHandlerFactory(HWND win) {
    this->win = win;
}

