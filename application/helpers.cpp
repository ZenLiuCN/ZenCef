//
// Created by Zen Liu on 2018-10-7.
//


#include "helpers.h"


std::wstring DumpRequestContents(const CefRefPtr<CefRequest> &request) {
    std::wstringstream ss;
    ss << "URL: " << request->GetURL().ToString16();
    ss << "\nMethod: " << request->GetMethod().ToString16();

    CefRequest::HeaderMap headerMap;
    request->GetHeaderMap(headerMap);
    if (!headerMap.empty()) {
        ss << "\nHeaders:";
        CefRequest::HeaderMap::const_iterator it = headerMap.begin();
        for (; it != headerMap.end(); ++it) {
            ss << "\n\t" << (*it).first.ToString16() << ": " <<
               (*it).second.ToString16();
        }
    }
    //CAN'T GET POST DATA FROM NONE STD SCHEME
    CefRefPtr<CefPostData> postData = request->GetPostData();
    if (postData.get()) {
        CefPostData::ElementVector elements;
        postData->GetElements(elements);
        if (!elements.empty()) {
            ss << "\nPost Data:";
            CefRefPtr<CefPostDataElement> element;
            CefPostData::ElementVector::const_iterator it = elements.begin();
            for (; it != elements.end(); ++it) {
                element = (*it);
                if (element->GetType() == PDE_TYPE_BYTES) {
                    // the element is composed of bytes
                    ss << "\n\tBytes: ";
                    if (element->GetBytesCount() == 0) {
                        ss << "(empty)";
                    } else {
                        // retrieve the data.
                        size_t size = element->GetBytesCount();
                        char *bytes = new char[size];
                        element->GetBytes(size, bytes);
                        std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
                        ss << conv.from_bytes(bytes);
                        delete[] bytes;
                    }
                } else if (element->GetType() == PDE_TYPE_FILE) {
                    ss << "\n\tFile: " << std::wstring(element->GetFile());
                }
            }
        }
    }
    return ss.str();
}


std::string GetUrl(const CefRefPtr<CefRequest> &request) {
    return request.get()->GetURL().ToString();
}
