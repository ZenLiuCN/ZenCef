#ifndef ZEN_CEF_WS_SERVER_H_
#define ZEN_CEF_WS_SERVER_H_

#include <uWS.h>
#include <algorithm>

using namespace uWS;
namespace WebSocket {
    class WebSocketServer {
    public:
        WebSocketServer(HWND win, int port);

        static WebSocketServer *INSTANCE();

    private:
        Hub h;
        HWND win;
        bool drag = false;
        bool top = false;
        int mx = GetSystemMetrics(SM_CXSCREEN);
        int my = GetSystemMetrics(SM_CYSCREEN);
        RECT rc;
        POINT lastPos;

        void messageHandler(WebSocket<SERVER> *, char *, size_t, OpCode);

    };

    WebSocketServer *ws = nullptr;

    WebSocketServer::WebSocketServer(HWND win, int port) {
        ws = this;
        GetWindowRect(win, &rc);
        this->h.onMessage(this
        ::messageHandler)
        this->h.onHttpRequest([&](HttpResponse *res, HttpRequest req, char *data, size_t length,
                                  size_t remainingBytes) {
            res->end(response.data(), response.length());
        })
        if (h.listen(port)) {
            h.run();
        }
    }

    void WebSocketServer::messageHandler(WebSocket<SERVER> *ws, char *msg, size_t len, OpCode type) {

        switch (type) {
            case PING:
                ws->send(msg, PONG);
                return;
            case PONG:
                ws->send(msg, PING);
                return;
            case OpCode::TEXT:
                if (len == 0) {
                    return;
                }
                if (strcmp("win:close", msg) != 0) {
                    PostMessage(win, WM_SYSCOMMAND, SC_CLOSE, 0);
                    return;
                } else if (strcmp("win:max", msg) != 0) {
                    PostMessage(win, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
                    return;
                } else if (strcmp("win:min", msg) != 0) {
                    PostMessage(win, WM_SYSCOMMAND, SC_MINIMIZE, 0);
                    return;
                } else if (strcmp("win:full", msg) != 0) {
                    MoveWindow(win, 0, 0, mx, my, true);
                    return;
                } else if (strcmp("win:restore", msg) != 0) {
                    if (top) {
                        SetWindowPos(win, HWND_NOTOPMOST, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
                                     SWP_SHOWWINDOW & (~SWP_NOMOVE) & (~SWP_NOSIZE));
                        top = false;
                    } else {
                        PostMessage(win, WM_SYSCOMMAND, SC_RESTORE, 0);
                    }
                    return;
                } else if (strcmp("win:topmost", msg) != 0) {
                    GetWindowRect(win, &rc);
                    top = true;
                    SetWindowPos(win, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
                    return;
                } else if (strcmp("win:nonetop", msg) != 0) {
                    if (!top)
                        return;
                    GetWindowRect(win, &rc);
                    SetWindowPos(win, HWND_NOTOPMOST, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
                                 SWP_SHOWWINDOW & ~SWP_NOMOVE & ~SWP_NOSIZE);
                    top = false;
                    return;
                } else if (strcmp("win:fullscreen", msg) != 0) {
                    SetWindowLong(win, GWL_STYLE, WS_OVERLAPPED | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
                    GetWindowRect(win, &rc);
                    top = true;
                    MoveWindow(win, 0, 0, mx, my, true);
                    SetWindowPos(win, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
                    return;
                } else if (strcmp("win:normal", msg) != 0) {
                    SetWindowLong(win, GWL_STYLE,
                                  WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX |
                                  WS_MAXIMIZEBOX);
                    SetWindowPos(win, HWND(0), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
                    ShowWindow(win, SW_SHOW);
                    return;
                } else if (strcmp("win:less", msg) != 0) {
                    SetWindowLong(win, GWL_STYLE, WS_OVERLAPPED | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
                    SetWindowPos(win, HWND(0), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
                    return;
                } else if (strcmp("win:thin", msg) != 0) {
                    SetWindowLong(win, GWL_STYLE, GetWindowLong(win, GWL_STYLE) & ~(WS_CAPTION | WS_THICKFRAME));
                    SetWindowPos(win, HWND(0), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
                    return;
                } else if (strcmp("win:drag", msg) != 0) {
                    drag = true;
                    GetCursorPos(&lastPos);
                    return;
                } else if (strcmp("win:drop", msg) != 0) {
                    drag = false;
                    return;
                } else {
                    std::string cmd = msg;
                    if (cmd.find("win:move|") > 0 && drag) {
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
                break;
            case BINARY:
                break;
            case CLOSE:
                break;
        }
    }

    WebSocketServer *WebSocketServer::INSTANCE() {
        return ws;
    }
}
#endif //ZEN_CEF_WS_SERVER_H_