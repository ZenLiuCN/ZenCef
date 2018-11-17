//
// Created by Zen Liu on 2018-11-16.
//

#include <include/base/cef_logging.h>
#include "logger.h"
#include "WindowController.h"

namespace {
    static std::vector<HHOOK> hooker;
    static const int mx = GetSystemMetrics(SM_CXSCREEN);
    static const int my = GetSystemMetrics(SM_CYSCREEN);
}

//region Methods

HWND WindowController::getWin() {
    return win;
}

void WindowController::setWin(HWND win) {
    this->win = win;
}

WindowController::WindowController() {
}

bool WindowController::check() {
    return win != nullptr;
}

WindowController::~WindowController() {
    if (!hooker.empty()) {
        unhook();
    }
}

bool WindowController::Move(int x, int y) {
    if (check() && moving) {
        int ox = x - savedMousePoint.x;
        int oy = y - savedMousePoint.y;
        int tx = savedMousePoint.x + ox;
        int ty = savedMousePoint.y + oy;
        RECT re;
        GetWindowRect(win, &re);
        if (ox != 0 || oy != 0) {
            OffsetRect(&re, ox, oy);
            if (tx > mx || ty > my || tx < 0 || ty < 0) {
                SetCursorPos(savedMousePoint.x, savedMousePoint.y);
                return true;
            }
            MoveWindow(win, re.left, re.top, re.right - re.left, re.bottom - re.top, false);
            savedMousePoint.x = savedMousePoint.x + ox;
            savedMousePoint.y = savedMousePoint.y + oy;
            SetCursorPos(savedMousePoint.x, savedMousePoint.y);

        }
        return true;
    }
    return false;
}

bool WindowController::Drop() {
    if (!check())return false;
    moving = false;
    return true;
}

bool WindowController::Drag() {
    if (!check())return false;
    moving = true;
    return static_cast<bool>(GetCursorPos(&savedMousePoint));
}

bool WindowController::NotTopMost() {
    if (!check())return false;
    if (!top) {
        return true;
    } else {
        top = false;
        return static_cast<bool>(SetWindowPos(win, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE));
    }
}

bool WindowController::TopMost() {
    if (!check())return false;
    if (!top) {
        top = true;
        return static_cast<bool>(SetWindowPos(win, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE));
    } else {
        return true;
    }
}

bool WindowController::Restore() {
    if (!check())return false;
/*    if (top && !NotTopMost()) {
        return false;
    }*/
    if (!hooker.empty()) {
        unhook();
    }
    if (rectSaved) {
        SetWindowLong(win, GWL_STYLE, savedStyle);
        return static_cast<bool>(SetWindowPos(win, HWND_NOTOPMOST, savedRect.left, savedRect.top,
                                              savedRect.right - savedRect.left,
                                              savedRect.bottom - savedRect.top,
                                              SWP_SHOWWINDOW & (~SWP_NOMOVE) & (~SWP_NOSIZE)));
    }
    return static_cast<bool>(PostMessage(win, WM_SYSCOMMAND, SC_RESTORE, 0));
}

bool WindowController::FullScreen() {
    if (!check())return false;
    if (GetWindowRect(win, &savedRect)) {
        rectSaved = true;
    } else {
        return false;
    }
    savedStyle = GetWindowLong(win, GWL_STYLE);
    FrameLess();
    full = true;
    return static_cast<bool>(MoveWindow(win, 0, 0, mx, my, true));
}

bool WindowController::Solo() {
    if (!check())return false;
    if (!FullScreen()) {
        return false;
    }
    if (!TopMost()) {
        return false;
    }
    if (hooker.empty() && !hook()) {
        return false;
    }
    return FrameLess();
}

bool WindowController::Show() {
    if (!check())return false;
    return static_cast<bool>(ShowWindow(win, SW_SHOW));
}

bool WindowController::Hidden() {
    if (!check())return false;
    return static_cast<bool>(ShowWindow(win, SW_HIDE));
}

bool WindowController::Min() {
    if (!check())return false;
    return static_cast<bool>(PostMessage(win, WM_SYSCOMMAND, SC_MINIMIZE, 0));
}

bool WindowController::Max() {
    if (!check())return false;
    return static_cast<bool>(PostMessage(win, WM_SYSCOMMAND, SC_MAXIMIZE, 0));
}

bool WindowController::ShowMax() {
    if (!check())return false;
    return static_cast<bool>(ShowWindow(win, SW_SHOWMAXIMIZED));
}

bool WindowController::ShowMin() {
    if (!check())return false;
    return static_cast<bool>(ShowWindow(win, SW_SHOWMINIMIZED));
}

bool WindowController::Close() {
    if (!check())return false;
    return static_cast<bool>(PostMessage(win, WM_SYSCOMMAND, SC_CLOSE, 0));
}

bool WindowController::FrameLess() {
    if (!check())return false;
    if (frame == 2) {
        return true;
    }
    SetWindowLong(win, GWL_STYLE, WS_OVERLAPPED | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
    frame = 2;
    return static_cast<bool>(SetWindowPos(win, nullptr, 0, 0, 0, 0,
                                          SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED));
}

bool WindowController::FrameThin() {
    if (!check())return false;
    if (frame == 1) {
        return true;
    }
    SetWindowLong(win, GWL_STYLE,
                  (GetWindowLong(win, GWL_STYLE) & ~WS_CAPTION) | WS_THICKFRAME);
    frame = 1;
    return static_cast<bool>(SetWindowPos(win, nullptr, 0, 0, 0, 0,
                                          SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED));
}

bool WindowController::FrameNormal() {
    if (!check())return false;
    if (frame == 0) {
        return true;
    }
    SetWindowLong(win, GWL_STYLE,
                  WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX |
                  WS_MAXIMIZEBOX);
    SetWindowPos(win, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
    frame = 0;
    return static_cast<bool>(ShowWindow(win, SW_SHOW));
}

bool WindowController::SetIcon(HICON ico) {
    if (!SendMessage(win, WM_SETICON, ICON_BIG, (LPARAM) ico)) {
        return false;
    }
    return static_cast<bool>(SendMessage(win, WM_SETICON, ICON_SMALL, (LPARAM) ico));
}

bool WindowController::SetIcon(int res) {
    HICON ico = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(res));
    return SetIcon(ico);
}

bool WindowController::SetIcon(std::string res) {
    auto ico = (HICON) LoadImage(nullptr, res.c_str(), IMAGE_ICON, 0, 0,
                                 LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_LOADFROMFILE);
    return SetIcon(ico);
}
//endregion


//region Static

bool WindowController::SetIcon(HWND win, int res) {
    HICON ico = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(res));
    return SetIcon(win, ico);
}

bool WindowController::SetIcon(HWND win, std::string res) {
    HANDLE ico = LoadImage(nullptr, res.c_str(), IMAGE_ICON, 0, 0,
                           LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_LOADFROMFILE);
    LOGD << fmt::sprintf("load of %x", ico == nullptr);
    return SetIcon(win, ico);
}

bool WindowController::SetIcon(HWND win, void *ico) {
    if (!SendMessage(win, WM_SETICON, ICON_BIG, (LPARAM) ico)) {
        return false;
    }
    return static_cast<bool>(SendMessage(win, WM_SETICON, ICON_SMALL, (LPARAM) ico));
}

bool WindowController::FrameThin(HWND win) {
    SetWindowLong(win, GWL_STYLE,
                  (GetWindowLong(win, GWL_STYLE) & ~WS_CAPTION) | WS_THICKFRAME);
    return static_cast<bool>(SetWindowPos(win, nullptr, 0, 0, 0, 0,
                                          SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED));
}

bool WindowController::FrameNormal(HWND win) {
    SetWindowLong(win, GWL_STYLE,
                  WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX |
                  WS_MAXIMIZEBOX);
    SetWindowPos(win, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
    return static_cast<bool>(ShowWindow(win, SW_SHOW));
}

bool WindowController::FrameLess(HWND win) {
    SetWindowLong(win, GWL_STYLE, WS_OVERLAPPED | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
    return static_cast<bool>(SetWindowPos(win, nullptr, 0, 0, 0, 0,
                                          SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED));
}

bool WindowController::Close(HWND win) {
    return static_cast<bool>(PostMessage(win, WM_SYSCOMMAND, SC_CLOSE, 0));
}

bool WindowController::Max(HWND win) {
    return static_cast<bool>(PostMessage(win, WM_SYSCOMMAND, SC_MAXIMIZE, 0));
}

bool WindowController::Min(HWND win) {
    return static_cast<bool>(PostMessage(win, WM_SYSCOMMAND, SC_MINIMIZE, 0));
}

bool WindowController::Solo(HWND win) {
    if (!FullScreen(win)) {
        return false;
    }
    if (!TopMost(win)) {
        return false;
    }
    return FrameThin(win);
}

bool WindowController::Restore(HWND win) {
    if (!FrameNormal(win)) {
        return false;
    }
    if (!NotTopMost(win)) {
        return false;
    }
    return static_cast<bool>(PostMessage(win, WM_SYSCOMMAND, SC_RESTORE, 0));
}

bool WindowController::TopMost(HWND win) {
    return static_cast<bool>(SetWindowPos(win, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE));
}

bool WindowController::NotTopMost(HWND win) {
    return static_cast<bool>(SetWindowPos(win, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE));
}

bool WindowController::FullScreen(HWND win) {
    FrameLess(win);
    return static_cast<bool>(MoveWindow(win, 0, 0, mx, my, true));
}

bool WindowController::unhook() {
    if (hooker.empty()) {
        LOGD << fmt::sprintf("already unhooked");
        return false;
    }
    auto hk = hooker[hooker.size() - 1];
    if (UnhookWindowsHookEx(hk)) {
        hooker.pop_back();
        LOGD << fmt::sprintf("had unhooked %d", hooker.size());
        return true;
    }
    return false;
}

LRESULT CALLBACK LowLevelKeyboardProc(INT nCode, WPARAM wParam, LPARAM lParam) {
//    LOGD << fmt::sprintf("catching hook: code %d ,wparam:%.8X,lparam:%.8X,status:%d %d", nCode, wParam, lParam,                         nCode == 0, nCode == HC_ACTION);
    static auto lalt = false;
    static auto ralt = false;
    static auto lshift = false;
    static auto rshift = false;
    static auto lctrl = false;
    static auto rctrl = false;
    static auto lwin = false;
    static auto rwin = false;
    if (nCode == HC_ACTION) {
        auto shifted = false;
        auto pKBDHOOK = (LPKBDLLHOOKSTRUCT) lParam;
        /*       LOGD << fmt::sprintf("catching hook  code %d key %X,scan key %X, wparam:%.8X", nCode, pKBDHOOK->vkCode,
                                    pKBDHOOK->scanCode, wParam);*/
        LOGD << fmt::sprintf("catching hook %.2X vk %.2X scan %.2X", wParam, pKBDHOOK->vkCode, pKBDHOOK->scanCode);
        if (wParam == WM_SYSKEYDOWN) {
            switch (pKBDHOOK->vkCode) {
                case VK_LMENU: lalt=true; break;
                case VK_RMENU: ralt=true; break;
                case VK_LCONTROL: lctrl=true; break;
                case VK_RCONTROL: rctrl=true; break;
                case VK_RSHIFT: rshift=true; break;
                case VK_LSHIFT: lshift=true; break;
                case VK_LWIN: lwin=true; break;
                case VK_RWIN: rwin=true; break;
                case VK_TAB:{
                    if(lalt){
                        return true;
                    }
                    break;
                }
                case VK_ESCAPE:{
                    if(lalt){
                        return true;
                    }
                    break;
                }
                default:{
                    if(lwin||rwin){
                        return true;
                    }
                    break;
                }
            }
        } else if (wParam == WM_SYSKEYUP) {
            switch (pKBDHOOK->vkCode) {
                case VK_LMENU: lalt= false; break;
                case VK_RMENU: ralt=false; break;
                case VK_LCONTROL: lctrl=false; break;
                case VK_RCONTROL: rctrl=false; break;
                case VK_RSHIFT: rshift=false; break;
                case VK_LSHIFT: lshift=false; break;
                case VK_LWIN: lwin= false; break;
                case VK_RWIN: rwin= false; break;
                case VK_TAB:{
                    if(lalt){
                        return true;
                    }
                    break;
                }
                case VK_ESCAPE:{
                    if(lalt){
                        return true;
                    }
                    break;
                }
                default:{
                    if(lwin||rwin){
                        return true;
                    }
                    break;
                }

            }
        }else if(wParam==WM_KEYDOWN){
            switch (pKBDHOOK->vkCode) {
                case VK_LMENU: lalt=true; break;
                case VK_RMENU: ralt=true; break;
                case VK_LCONTROL: lctrl=true; break;
                case VK_RCONTROL: rctrl=true; break;
                case VK_RSHIFT: rshift=true; break;
                case VK_LSHIFT: lshift=true; break;
                case VK_LWIN: lwin=true; break;
                case VK_RWIN: rwin=true; break;
                default:{
                    if(lwin||rwin){
                        return true;
                    }
                    break;
                }
            }
        }else if(wParam==WM_KEYUP){
            switch (pKBDHOOK->vkCode) {
                case VK_LMENU: lalt= false; break;
                case VK_RMENU: ralt=false; break;
                case VK_LCONTROL: lctrl=false; break;
                case VK_RCONTROL: rctrl=false; break;
                case VK_RSHIFT: rshift=false; break;
                case VK_LSHIFT: lshift=false; break;
                case VK_LWIN: lwin= false; break;
                case VK_RWIN: rwin= false; break;
                default:{
                    if(lwin||rwin){
                        return true;
                    }
                    break;
                }
            }
        }
    }
    return false;
}

bool WindowController::hook() {
    if (!hooker.empty()) {
        LOGD << fmt::sprintf("already hooked");
        return true;
    }
    auto hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(nullptr), 0);
    if (hook != nullptr && hook != INVALID_HANDLE_VALUE) {
        hooker.push_back(hook);
        LOGD << fmt::sprintf("had hooked %d", hooker.size());
        return true;
    }
    return false;
}

//endregion
