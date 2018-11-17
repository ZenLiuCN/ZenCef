//
// Created by Zen Liu on 2018-10-5.
//

#include "helper_win.h"
#include <windows.h>

#ifdef DEBUG

#include <iostream>
#include "logger.h"

#endif

using namespace std;
string AppDir = "";
string AppName = "";
string AppFullPath = "";

string GetAppFullPath() {
    if (AppFullPath.empty()) {
        char exeFullPath[MAX_PATH]; // Full path
        GetModuleFileName(nullptr, exeFullPath, MAX_PATH);
        AppFullPath = (string) exeFullPath;    // Get full path of the file
        int pos = AppFullPath.find_last_of('\\', AppFullPath.length());
        AppDir = AppFullPath.substr(0, static_cast<unsigned int>(pos));  // Return the directory without the file name
        AppName = AppFullPath.substr(static_cast<unsigned int>(pos + 1));  // Return the directory without the file name
#ifdef DEBUG
        cout << AppDir << "\t" << AppName << "\t" << AppFullPath << endl;
        LOGD<<fmt::sprintf("\t%s \t%s \t %s",AppDir,AppName,AppFullPath);
#endif
        free(exeFullPath);
    }
    return AppFullPath;
}

string GetAppDir() {
    if (AppDir.empty()) {
        GetAppFullPath();
    }
    return AppDir;
}

string GetAppName() {
    if (AppName.empty()) {
        GetAppFullPath();
    }
    return AppName;
}


cef_string_utf8_t *cefSourceToString(const CefString *source) {
    cef_string_utf8_t *output = cef_string_userfree_utf8_alloc();
    if (source == nullptr) {
        return nullptr;
    }
    cef_string_to_utf8(source->GetStruct()->str, source->GetStruct()->length, output);
    return output;
}

char *cefSourceToString(const CefString &source) {
    cef_string_utf8_t *output = cef_string_userfree_utf8_alloc();
    if (source.empty()) {
        return nullptr;
    }
    cef_string_to_utf8(source.GetStruct()->str, source.GetStruct()->length, output);
    return output->str;
}

