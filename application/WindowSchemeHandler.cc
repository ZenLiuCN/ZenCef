

#include "WindowSchemeHandler.h"
#include "go_server.h"

bool WindowSchemeHandler::ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) {
    // Evaluate |request| to determine proper handling...
    // Execute |callback| once header information is available.
    // Return true to handle the request
//    CefPostTask(TID_IO, base::Bind(&WindowSchemeHandler::HandleRequest, this, request, callback));
    auto url = GetUrl(request);
    auto cmd = url.substr(url.rfind('/') + 1);
//    LOGGER_("cmd get %s=>%s",url.c_str(),cmd.c_str())
    if (cmd == "close") {
        PostMessage(win, WM_SYSCOMMAND, SC_CLOSE, 0);

    } else if (cmd == "max") {
        PostMessage(win, WM_SYSCOMMAND, SC_MAXIMIZE, 0);

    } else if (cmd == "min") {
        PostMessage(win, WM_SYSCOMMAND, SC_MINIMIZE, 0);

    } else if (cmd == "full") {
        MoveWindow(win, 0, 0, mx, my, true);

    } else if (cmd == "restore") {
        if (state.x!=0) {
            SetWindowPos(win, HWND_NOTOPMOST, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
                         SWP_SHOWWINDOW & (~SWP_NOMOVE) & (~SWP_NOSIZE));
            state.x = 0;
        } else {
            PostMessage(win, WM_SYSCOMMAND, SC_RESTORE, 0);
        }

    } else if (cmd == "topmost") {
        GetWindowRect(win, &rc);
        state.x  = 1;
        SetWindowPos(win, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

    } else if (cmd == "nonetop") {
        if ( state.x ==0) return false;

        GetWindowRect(win, &rc);
        SetWindowPos(win, HWND_NOTOPMOST, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
                     SWP_SHOWWINDOW & ~SWP_NOMOVE & ~SWP_NOSIZE);
        state.x  = 0;

    } else if (cmd == "fullscreen") {
        SetWindowLong(win, GWL_STYLE, WS_OVERLAPPED | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
        GetWindowRect(win, &rc);
        state.x  = 1;
        MoveWindow(win, 0, 0, mx, my, true);
        SetWindowPos(win, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

    } else if (cmd == "normal") {
        SetWindowLong(win, GWL_STYLE,
                      WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX |
                      WS_MAXIMIZEBOX);
        SetWindowPos(win, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
        ShowWindow(win, SW_SHOW);

    } else if (cmd == "less") {
        SetWindowLong(win, GWL_STYLE, WS_OVERLAPPED | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
        SetWindowPos(win, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

    } else if (cmd == "thin") {
        SetWindowLong(win, GWL_STYLE, GetWindowLong(win, GWL_STYLE) & ~(WS_CAPTION | WS_THICKFRAME));
        SetWindowPos(win, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

    } else if (cmd == "drag") {
        LOGGER_("cmd :%s match %d %d", cmd.c_str(),cmd.find("move") ,  state.y);
        state.y = 1;
        LOGGER_("cmd :%s match %d %d", cmd.c_str(),cmd.find("move") ,  state.y);
        GetCursorPos(&lastPos);
    } else if (cmd == "drop") {
        LOGGER_("cmd :%s match %d  %d", cmd.c_str(),cmd.find("move") ,  state.y);
        state.y = 0;
        LOGGER_("cmd :%s match %d %d", cmd.c_str(),cmd.find("move") ,  state.y);
    } else {
        LOGGER_("cmd :%s match %d %d", cmd.c_str(),cmd.find("move") ,  state.y);
        if (cmd.find("move") >= 0 &&   state.y==1) {
            LOGGER_("cmd :%s", cmd.c_str());
            auto b = cmd.find('|');
            auto e = cmd.rfind('|');
            try {
                int x = stoi(cmd.substr(b + 1, e - b));
                int y = stoi(cmd.substr(e + 1));
                int ox = x - lastPos.x;
                int oy = y - lastPos.y;
                int tx = lastPos.x + ox;
                int ty = lastPos.y + oy;
                RECT re;
                GetWindowRect(win, &re);
                if (ox != 0 || oy != 0) {
                    OffsetRect(&re, ox, oy);
                    if (tx > mx || ty > my || tx < 0 || ty < 0) {
                        SetCursorPos(lastPos.x, lastPos.y);
                        callback->Continue();
                        return true;
                    }
                    MoveWindow(win, re.left, re.top, re.right - re.left, re.bottom - re.top, false);
                    lastPos.x = lastPos.x + ox;
                    lastPos.y = lastPos.y + oy;
                    SetCursorPos(lastPos.x, lastPos.y);
                }
            } catch (std::exception &) {
                callback->Cancel();
                return false;
            }
        }
    }
    callback->Continue();
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
//    response->SetMimeType("text/html");
    response->SetStatus(200);
    //Specify the resulting response length.
    response_length = 0;
}

bool WindowSchemeHandler::CanGetCookie(const CefCookie &cookie) {
    return false;
}

bool WindowSchemeHandler::CanSetCookie(const CefCookie &cookie) {
    return false;
}

void WindowSchemeHandler::Cancel() {
    LOGGER_("request canceled")
}

WindowSchemeHandler::WindowSchemeHandler(HWND win) {
    this->win = win;
}

void WindowSchemeHandler::HandleRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) {
/*#ifdef DEBUG
    WCHAR_LOGGER_(L"%ls\n",DumpRequestContents(request).c_str());
#endif*/
    auto url = GetUrl(request);
    auto cmd = url.substr(url.rfind('/') + 1);
    if (cmd == "close") {
        PostMessage(win, WM_SYSCOMMAND, SC_CLOSE, 0);
        return;
    } else if (cmd == "max") {
        PostMessage(win, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
        return;
    } else if (cmd == "min") {
        PostMessage(win, WM_SYSCOMMAND, SC_MINIMIZE, 0);
        return;
    } else if (cmd == "full") {
        MoveWindow(win, 0, 0, mx, my, true);
        return;
    } else if (cmd == "restore") {
        if ( state.x==1) {
            SetWindowPos(win, HWND_NOTOPMOST, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
                         SWP_SHOWWINDOW & (~SWP_NOMOVE) & (~SWP_NOSIZE));
            state.x = 0;
        } else {
            PostMessage(win, WM_SYSCOMMAND, SC_RESTORE, 0);
        }
        return;
    } else if (cmd == "topmost") {
        GetWindowRect(win, &rc);
        state.x = 1;
        SetWindowPos(win, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
        return;
    } else if (cmd == "nonetop") {
        if (state.x==0)
            return;
        GetWindowRect(win, &rc);
        SetWindowPos(win, HWND_NOTOPMOST, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
                     SWP_SHOWWINDOW & ~SWP_NOMOVE & ~SWP_NOSIZE);
        state.x = 0;
        return;
    } else if (cmd == "fullscreen") {
        SetWindowLong(win, GWL_STYLE, WS_OVERLAPPED | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
        GetWindowRect(win, &rc);
        state.x = 1;
        MoveWindow(win, 0, 0, mx, my, true);
        SetWindowPos(win, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
        return;
    } else if (cmd == "normal") {
        SetWindowLong(win, GWL_STYLE,
                      WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX |
                      WS_MAXIMIZEBOX);
        SetWindowPos(win, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
        ShowWindow(win, SW_SHOW);
        return;
    } else if (cmd == "less") {
        SetWindowLong(win, GWL_STYLE, WS_OVERLAPPED | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
        SetWindowPos(win, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
        return;
    } else if (cmd == "thin") {
        SetWindowLong(win, GWL_STYLE, GetWindowLong(win, GWL_STYLE) & ~(WS_CAPTION | WS_THICKFRAME));
        SetWindowPos(win, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
        return;
    } else if (cmd == "drag") {
        state.y = 1;
        GetCursorPos(&lastPos);
        return;
    } else if (cmd == "drop") {
        state.y = 0;
        return;
    } else {
        if (cmd.find("move") > 0 && state.x!=0) {
            LOGGER_("cmd :%s", cmd.c_str());
            auto b = cmd.find('|');
            auto e = cmd.rfind('|');
            try {
                int x = stoi(cmd.substr(b + 1, e - b));
                int y = stoi(cmd.substr(e + 1));
                int ox = x - lastPos.x;
                int oy = y - lastPos.y;
                int tx = lastPos.x + ox;
                int ty = lastPos.y + oy;
                RECT re;
                GetWindowRect(win, &re);
                if (ox != 0 || oy != 0) {
                    OffsetRect(&re, ox, oy);
                    if (tx > mx || ty > my || tx < 0 || ty < 0) {
                        SetCursorPos(lastPos.x, lastPos.y);
                        return;
                    }
                    MoveWindow(win, re.left, re.top, re.right - re.left, re.bottom - re.top, false);
                    lastPos.x = lastPos.x + ox;
                    lastPos.y = lastPos.y + oy;
                    SetCursorPos(lastPos.x, lastPos.y);
                }
            } catch (std::exception &) {
                return;
            }
        }
    }
    callback->Continue();
};

WindowSchemeHandlerFactory::WindowSchemeHandlerFactory(HWND win) {
    this->win = win;
}

