//
// Created by Zen Liu on 2018-11-16.
//

#ifndef PROJECT_WINDOWCONTRL_H
#define PROJECT_WINDOWCONTRL_H

#include <windows.h>
#include <string>
#include <vector>
class WindowController {
public:
    WindowController();
    ~WindowController();
    bool hook();
    bool unhook();
    bool FrameThin();
    bool FrameNormal();
    bool FrameLess();
    bool Close();
    bool Max();
    bool ShowMax();
    bool Min();
    bool ShowMin();
    bool Hidden();
    bool Show();
    bool Solo();
    bool FullScreen();
    bool Restore();
    bool TopMost();
    bool NotTopMost();
    bool Drag();
    bool Drop();
    bool Move(int x ,int y);
    bool SetIcon(HICON ico);
    bool SetIcon(int res);
    bool SetIcon(std::string res);
    static bool SetIcon(HWND win,void* ico);
    static bool SetIcon(HWND win,int res);
    static bool SetIcon(HWND win,std::string res);

    static bool FullScreen(HWND win);
    static bool NotTopMost(HWND win);
    static bool TopMost(HWND win);
    static bool Restore(HWND win);
    static bool Solo(HWND win);
    static bool Min(HWND win);
    static bool Max(HWND win);
    static bool Close(HWND win);
    static bool FrameLess(HWND win);
    static bool FrameNormal(HWND win);
    static bool FrameThin(HWND win);

    HWND getWin();
    void setWin(HWND win);
private:
    bool check();
    bool top, full, moving,rectSaved=false,movable=true;
    int frame=0;
    LONG savedStyle;
    HWND win= nullptr;
    RECT savedRect;
    POINT savedMousePoint;
};


#endif //PROJECT_WINDOWCONTRL_H
