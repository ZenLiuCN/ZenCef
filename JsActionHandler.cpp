//
// Created by Zen Liu on 2018-10-7.
//
#include "JsActionHandler.h"
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

bool JsActionHandler::WinClose(HWND win, int x, int y) {
    PostMessage(win, WM_SYSCOMMAND, SC_CLOSE, 0);
    return true;
}

bool JsActionHandler::Remove(const CefString &name) {
    if (this->registry.empty() || this->registry.find(name) == this->registry.end())
        return false;
    this->registry.erase(name);
    return true;
}

bool JsActionHandler::Register(const WinHandler &handler, const CefString &name) {
    if (this->registry.find(name) != this->registry.end()) {
        return false;
    }
    this->registry[name] = handler;
    return true;
}

bool JsActionHandler::Execute(const CefString &name, CefRefPtr<CefV8Value> object, const CefV8ValueList &arguments,
                              CefRefPtr<CefV8Value> &retval, CefString &exception) {
    if (this->registry.empty() || this->registry.find(name) == this->registry.end())
        return false;
    return this->registry[name].operator()(win, 0, 0);
}
