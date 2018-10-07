#include <utility>

//
// Created by Zen Liu on 2018-10-7.
//

#ifndef ZEN_CEF_JS_ACTION_HANDLER_H
#define ZEN_CEF_JS_ACTION_HANDLER_H

#include <include/cef_v8.h>
#include <functional>

typedef std::function<bool(CefRefPtr<CefV8Value>, const CefV8ValueList, CefRefPtr<CefV8Value>, CefString)> V8Handler;
typedef std::map<CefString, V8Handler> handlers;

class JsActionHandler : public CefV8Handler {
public:
    explicit JsActionHandler(HWND win);

    JsActionHandler *INSTANCE();

    bool Execute(const CefString &name, CefRefPtr<CefV8Value> object, const CefV8ValueList &arguments,
                 CefRefPtr<CefV8Value> &retval, CefString &exception) override {
        if (this->registry.empty() || this->registry.find(name) == this->registry.end())
            return false;
        return this->registry[name].operator()(object, arguments, retval, exception);
    }

    bool Register(const V8Handler &handler, const CefString &name) {
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

private:
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
    JSActionHandler::g_handler = this;

}


#endif //ZEN_CEF_JS_ACTION_HANDLER_H
