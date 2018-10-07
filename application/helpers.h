//
// Created by Zen Liu on 2018-10-7.
//

#ifndef ZEN_CEF_HELPERS_H
#define ZEN_CEF_HELPERS_H

#include <include/cef_request.h>
#include <codecvt>
#include <locale>

std::wstring DumpRequestContents(const CefRefPtr<CefRequest> &request);
/**
 * GetUrl from request
 * @param request
 * @return string
 */
std::string GetUrl(const CefRefPtr<CefRequest> &request);

#endif //ZEN_CEF_HELPERS_H
