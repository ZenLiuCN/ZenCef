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
                 CefRefPtr<CefV8Value> &retval, CefString &exception) override {
        if (this->registry.empty() || this->registry.find(name) == this->registry.end())
            return false;
        return this->registry[name].operator()(win, 0, 0);
    }

    bool Register(const WinHandler &handler, const CefString &name) {
        if (this->registry.find(name) != this->registry.end()) {
            return false;
        }
        this->registry[name] = handler;
        return true;
    }

    bool Remove(const CefString &name) {
        if (this->registry.empty() || this->registry.find(name) == this->registry.end())
            return false;
        this->registry.erase(name);
        return true;
    }

    static bool WinClose(HWND win, int x, int y) {
        PostMessage(win, WM_SYSCOMMAND, SC_CLOSE, 0);
        return true;
    }

private:
    HWND win;
    handlers registry;
IMPLEMENT_REFCOUNTING(JsActionHandler)
};
namespace JSActionHandler {
    JsActionHandler *g_handler = nullptr;
}

JsActionHandler *JsActionHandler::INSTANCE() {
    return JSActionHandler::g_handler;
}

JsActionHandler::JsActionHandler(HWND win) {
    this->win = win;
    JSActionHandler::g_handler = this;
    this->registry["WindowClose"] = JsActionHandler::WinClose;

}


#endif //ZEN_CEF_JS_ACTION_HANDLER_H
