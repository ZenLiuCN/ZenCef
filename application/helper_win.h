//
// Created by Zen Liu on 2018-10-5.
//

#ifndef ZEN_CEF_HELPER_WIN_H
#define ZEN_CEF_HELPER_WIN_H

#include <windows.h>
#include <string>
#include <include/internal/cef_string.h>

std::string GetAppDir();

cef_string_utf8_t *cefSourceToString(const CefString *source);
char *cefSourceToString(const CefString &source);
#endif //ZEN_CEF_HELPER_WIN_H
