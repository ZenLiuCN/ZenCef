//
// Created by Zen Liu on 2018-11-13.
//

#ifndef PROJECT_DOWNLOADDIALOG_HPP
#define PROJECT_DOWNLOADDIALOG_HPP

#include <include/cef_download_item.h>
#include <mingw.thread.h>
#include <map>
#include <commctrl.h>
#include <iomanip>
#include "logger.h"
#include "res/resource.h"


namespace downloadBox {
    struct string : std::wstring {
        string() : std::wstring() {}

        template<typename T>
        string(const T &t) : std::wstring() {
            std::wstringstream ss;
            if (ss << std::setprecision(20) << t)
                this->assign(ss.str());
        }

        template<unsigned N>
        string(const std::string::value_type (&str)[N]) : std::wstring() {
            std::string s(str);
            this->assign(s.begin(), s.end());
        }

        template<unsigned N>
        string(const std::wstring::value_type (&str)[N]) : std::wstring(str) {}

        string(const std::string &t) : std::wstring(t.begin(), t.end()) {}

        string(const std::wstring &t) : std::wstring(t) {}

        template<typename T>
        bool operator>>(T &t) const {
            std::wstringstream ss(*this);
            return ss >> t ? true : (t = T(), false);
        }

        template<typename T>
        string &operator<<(const T &t) {
            std::wstringstream ss;
            return ss << t ? (this->assign(*this + ss.str()), *this) : *this;
        }

        string &operator<<(const std::string &t) {
            return operator<<(std::wstring(t.begin(), t.end()));
        }

        template<typename T>
        T as() const {
            T t;
            std::wstringstream ss(*this);
            return ss >> t ? t : T();
        }

        std::wstring str() const {
            return *this;
        }

        operator int() const {
            return as<int>();
        }
    };

    using vars = std::map<string, string>;

    vars &getInfo() {
        static std::map<std::thread::id, vars> all;
        std::thread::id self = std::this_thread::get_id();
        return (all[self] = all[self]);
    }

    HWND &getHWND() {
        static std::map<std::thread::id, HWND> all;
        std::thread::id self = std::this_thread::get_id();
        return (all[self] = all[self]);
    }

    INT_PTR CALLBACK DialogProc(
            _In_ HWND hwndDlg,
            _In_ UINT uMsg,
            _In_ WPARAM wParam,
            _In_ LPARAM lParam
    ) {
        switch (uMsg) {
            case WM_INITDIALOG: {
                getInfo()["cancel"] = false;
                SetWindowTextW(hwndDlg, L"下载文件");
                auto hFont = CreateFont(14, 0, 0, 0, 0, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "微软雅黑");//创建字体
                SendMessage(GetDlgItem(hwndDlg, IDD_CONTENT), WM_SETFONT, (WPARAM) hFont, TRUE);
                SetWindowTextW(GetDlgItem(hwndDlg, IDD_CONTENT),
                               getInfo()["url"].insert(0, L"下载文件:\n").append(L" 进行中...").c_str());
                EnableWindow(GetDlgItem(hwndDlg, IDOK), FALSE);

            }
            case WM_COMMAND: {
                switch (LOWORD(wParam)) {
                    case IDOK: {
                        ShellExecuteW(nullptr, nullptr, L"explorer.exe",
                                      getInfo()["path"].insert(0, L"/select,\"").append(L"\"").c_str(),
                                      nullptr, SW_SHOWNORMAL);
//                        EndDialog(hwndDlg,getInfo()["nResult"]);
                        DestroyWindow(hwndDlg);
                        return S_OK;
                    }
                    case IDCANCEL: {
//                        EndDialog(hwndDlg,getInfo()["nResult"]);
                        if (getInfo()["isDone"] == 100) {
                            DestroyWindow(hwndDlg);
                        } else {
                            getInfo()["cancel"] = true;
                        }

                        return S_OK;
                    }
                }
                break;
            }

        }
        return false;
    }


    void ShowDownloading(const vars &d_) {
        auto &dialog = (getInfo() = d_);
        getHWND() = CreateDialog(GetModuleHandle(nullptr), MAKEINTRESOURCEA(IDD_DIALOG_DOWNLOAD), nullptr, DialogProc);
    }

    void ShowDownloading(const string &input) {
        vars v;

        // parse input text into map. format is key=value;key=value; [...]
        std::wstringstream split(input + L';');
        for (string each; std::getline(split, each, L';');) {
            std::wstringstream token(each);
            for (string prev, key; std::getline(token, key, L'='); prev = key) {
                v[v.find(prev) == v.end() ? key : prev] = key;
            }
        }

        for (auto &in : v) {
            for (auto &ch : in.second) {
                /**/ if (ch == '\a') ch = '=';
                else if (ch == '\b') ch = ';';
            }
        }

        ShowDownloading(v);
    }
}

#endif //PROJECT_DOWNLOADDIALOG_HPP
