#include <utility>

//
// Created by Zen Liu on 2018-10-7.
//

#ifndef ZEN_CEF_JS_ACTION_HANDLER_H
#define ZEN_CEF_JS_ACTION_HANDLER_H

#include <include/cef_v8.h>
#include <functional>

typedef std::function<bool(HWND, int, int)> WinHandler;
typedef std::map<CefString, WinHandler> handlers;

class JsActionHandler : public CefV8Handler {
public:
    explicit JsActionHandler(HWND win);

    JsActionHandler *INSTANCE();

    bool Execute(const CefString &name, CefRefPtr<CefV8Value> object, const CefV8ValueList &arguments,
                 CefRefPtr<CefV8Value> &retval, CefString &exception) override;

    bool Register(const WinHandler &handler, const CefString &name);

    bool Remove(const CefString &name);

    static bool WinClose(HWND win, int x, int y);

private:
    HWND win;
    handlers registry;
IMPLEMENT_REFCOUNTING(JsActionHandler)
};



#endif //ZEN_CEF_JS_ACTION_HANDLER_H
