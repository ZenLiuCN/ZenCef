#include <utility>

//
// Created by Zen Liu on 2018-11-16.
//

#ifndef PROJECT_HTTPCLIENT_H
#define PROJECT_HTTPCLIENT_H


#include <include/cef_urlrequest.h>
#include <functional>
#include <algorithm>
#include <chrono>
#include "md5.hpp"
#include "config.h"

#ifndef PROJECT_HTTPCLIENT_H_FILE_EXISTS
#define  PROJECT_HTTPCLIENT_H_FILE_EXISTS

bool FileExists(std::string path) {
    DWORD dwAttrib = GetFileAttributes(path.c_str());
    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
            !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

using namespace std;
using namespace std::chrono;
// filetime_duration has the same layout as FILETIME; 100ns intervals
using filetime_duration = duration<int64_t, ratio<1, 10'000'000>>;
// January 1, 1601 (NT epoch) - January 1, 1970 (Unix epoch):
constexpr duration<int64_t> nt_to_unix_epoch{INT64_C(-11644473600)};

system_clock::time_point FILETIME_to_system_clock(FILETIME fileTime) {
    const filetime_duration asDuration{static_cast<int64_t>(
                                               (static_cast<uint64_t>(fileTime.dwHighDateTime) << 32)
                                               | fileTime.dwLowDateTime)};
    const auto withUnixEpoch = asDuration + nt_to_unix_epoch;
    return system_clock::time_point{
            duration_cast<system_clock::duration>(withUnixEpoch)};
}

FILETIME system_clock_to_FILETIME(system_clock::time_point systemPoint) {
    const auto asDuration = duration_cast<filetime_duration>(
            systemPoint.time_since_epoch());
    const auto withNtEpoch = asDuration - nt_to_unix_epoch;
    const uint64_t rawCount = static_cast<const uint64_t>(withNtEpoch.count());
    FILETIME result;
    result.dwLowDateTime = static_cast<DWORD>(rawCount); // discards upper bits
    result.dwHighDateTime = static_cast<DWORD>(rawCount >> 32);
    return result;
}

bool FileRefresh(std::string path) {
    if (!FileExists(path))
        return true;
    FILETIME lastWrite;
    auto file = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, NULL);
    if (!GetFileTime(file, nullptr, nullptr, &lastWrite)) {
        return false;
    }
    CloseHandle(file);
    auto pt = FILETIME_to_system_clock(lastWrite);
    auto def = (duration_cast<hours>(high_resolution_clock::now().time_since_epoch()).count() -
                duration_cast<hours>(pt.time_since_epoch()).count());
    LOGD << fmt::sprintf("file %s last write time diff hour is %d", path.c_str(), def);
    return def > 48;
}


#endif

class HttpClient : public CefURLRequestClient {
public:
    HttpClient()
            : upload_total_(0),
              download_total_(0) {}

    virtual void OnComplete(CefString url, CefURLRequest::Status status, CefURLRequest::ErrorCode errorCode,
                            CefRefPtr<CefResponse> response, std::string data) = 0;

    virtual void DoRequest(const CefString &url, const CefString &method) {
        auto req = CefRequest::Create();
        req->SetURL(url);
        req->SetMethod(method);
        CefURLRequest::Create(req, this, nullptr);
    }

    void OnRequestComplete(CefRefPtr<CefURLRequest> request) OVERRIDE {
        CefURLRequest::Status status = request->GetRequestStatus();
        CefURLRequest::ErrorCode error_code = request->GetRequestError();
        CefRefPtr<CefResponse> response = request->GetResponse();
        OnComplete(request->GetRequest()->GetURL(), status, error_code, response, download_data_);
        // Do something with the response...
    }

    void OnUploadProgress(CefRefPtr<CefURLRequest> request,
                          int64 current,
                          int64 total) OVERRIDE {
        upload_total_ = total;
    }

    void OnDownloadProgress(CefRefPtr<CefURLRequest> request,
                            int64 current,
                            int64 total) OVERRIDE {
        download_total_ = total;
    }

    void OnDownloadData(CefRefPtr<CefURLRequest> request,
                        const void *data,
                        size_t data_length) OVERRIDE {
        download_data_ += std::string(static_cast<const char *>(data), data_length);
    }

    bool GetAuthCredentials(bool isProxy,
                            const CefString &host,
                            int port,
                            const CefString &realm,
                            const CefString &scheme,
                            CefRefPtr<CefAuthCallback> callback) OVERRIDE {
        return false;  // Not handled.
    }

private:
    int64 upload_total_;
    int64 download_total_;
    std::string download_data_;


private:
IMPLEMENT_REFCOUNTING(HttpClient);
};

/*class HttpGet : public HttpClient {
public:
    explicit HttpGet(std::function<void(std::string file)> cb) : callback(std::move(cb)) {};

    void OnComplete(CefString url, CefURLRequest::Status status, CefURLRequest::ErrorCode errorCode,
                    CefRefPtr<CefResponse> response,
                    std::string data) override {
        LOGD << fmt::sprintf("url:%s status: %d errcode %d statusText:%s", url.ToString().c_str(), status, errorCode,
                             response->GetStatusText().ToString().c_str());
        if (status == UR_SUCCESS && errorCode == ERR_NONE && response->GetStatus() == 200) {
            auto ico = nameOfUrl(url);
            LOGD << fmt::sprintf("file path is %s", ico.c_str());
            auto hFile = CreateFile(ico.c_str(), GENERIC_WRITE, 0, NULL,
                                    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile == nullptr || hFile == INVALID_HANDLE_VALUE) {
                LOGD << fmt::sprintf("open file error %d", GetLastError());
                return;
            }
            DWORD byteWrite = 0;
            if (WriteFile(hFile, data.c_str(), data.length(), &byteWrite, nullptr)) {
                CloseHandle(hFile);
                callback(ico);
                return;
            }
            CloseHandle(hFile);
            LOGD << fmt::sprintf("write file error or open file error %d", GetLastError());
        }

    }

    std::string nameOfUrl(CefString url) {
        auto ico = conf.CEF_CACHE_DIR;
        mkdir(ico.append("\\favicon\\").c_str());
        ico.append(md5::md5_hash_hex(url)).append(".ico");
        return ico;
    }

    void DoRequest(const CefString &url) {
        auto ico = nameOfUrl(url);
        if (FileRefresh(ico.c_str())) {
            HttpClient::DoRequest(url, "GET");
        } else {
            callback(ico);
        }
    }

private:
    const std::function<void(std::string)> callback;

IMPLEMENT_REFCOUNTING(HttpGet);
};*/
class HttpGetIcon : public HttpClient {
public:
    explicit HttpGetIcon(HWND hwnd) : win(hwnd) {};

    void OnComplete(CefString url, CefURLRequest::Status status, CefURLRequest::ErrorCode errorCode,
                    CefRefPtr<CefResponse> response,
                    std::string data) override {
        LOGD << fmt::sprintf("url:%s status: %d errcode %d statusText:%s", url.ToString().c_str(), status, errorCode,
                             response->GetStatusText().ToString().c_str());
        if (status == UR_SUCCESS && errorCode == ERR_NONE && response->GetStatus() == 200) {
            auto ico = nameOfUrl(url);
            LOGD << fmt::sprintf("file path is %s", ico.c_str());
            auto hFile = CreateFile(ico.c_str(), GENERIC_WRITE, 0, NULL,
                                    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile == nullptr || hFile == INVALID_HANDLE_VALUE) {
                LOGD << fmt::sprintf("open file error %d", GetLastError());
                return;
            }
            DWORD byteWrite = 0;
            if (WriteFile(hFile, data.c_str(), data.length(), &byteWrite, nullptr)) {
                CloseHandle(hFile);
                LOGD<<fmt::sprintf("load new icon %s into browser",ico.c_str());
                HANDLE icon = LoadImage(nullptr, ico.c_str(), IMAGE_ICON, 0, 0,
                                        LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_LOADFROMFILE);
                if (icon == nullptr || icon == INVALID_HANDLE_VALUE) {
                    LOGD << fmt::sprintf("get ico of %s error %d", ico.c_str(), GetLastError());
                    return;
                }
                SendMessage(win, WM_SETICON, ICON_BIG, (LPARAM) icon);
                return;
            }
            CloseHandle(hFile);
            LOGD << fmt::sprintf("write file error  %d", GetLastError());
        }

    }

    std::string nameOfUrl(CefString url) {
        auto ico = conf.CEF_CACHE_DIR;
        mkdir(ico.append("\\favicon\\").c_str());
        auto name=md5::md5_hash_hex(url);
        transform(name.begin(), name.end(), name.begin(), ::toupper);
        ico.append(name).append(".ico");
        return ico;
    }

    void DoRequest(const CefString &url) {
        auto ico = nameOfUrl(url);
        if (FileRefresh(ico.c_str())) {
            HttpClient::DoRequest(url, "GET");
        } else {
            LOGD<<fmt::sprintf("load old icon %s into browser",ico.c_str());
            HANDLE icon = LoadImage(nullptr, ico.c_str(), IMAGE_ICON, 0, 0,
                                    LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_LOADFROMFILE);
            if (icon == nullptr || icon == INVALID_HANDLE_VALUE) {
                LOGD << fmt::sprintf("get ico of %s error %d", ico.c_str(), GetLastError());
                return;
            }
            SendMessage(win, WM_SETICON, ICON_BIG, (LPARAM) icon);
        }
    }

private:
    HWND win;

IMPLEMENT_REFCOUNTING(HttpGetIcon);
};

#endif //PROJECT_HTTPCLIENT_H
