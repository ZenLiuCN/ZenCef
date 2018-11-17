//
// Created by Zen Liu on 2018-11-15.
//

#ifndef PROJECT_V8HANDLER_H
#define PROJECT_V8HANDLER_H

#include <include/cef_v8.h>
#include "go_server.h"

class V8Handler: public CefV8Handler{
public:
    GoServer go;
    V8Handler();
    bool Execute(const CefString &name, CefRefPtr<CefV8Value> object, const CefV8ValueList &arguments,
                 CefRefPtr<CefV8Value> &retval, CefString &exception) override;
private:
    IMPLEMENT_REFCOUNTING(V8Handler)
};
#endif //PROJECT_V8HANDLER_H
