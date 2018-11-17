//
// Created by Zen Liu on 2018-11-15.
//
#include "logger.h"
#include "v8handler.h"
#include "singleton.h"

V8Handler::V8Handler() : go(Singleton<GoServer>::getInstance()) {}


bool V8Handler::Execute(const CefString &name, CefRefPtr<CefV8Value> object, const CefV8ValueList &arguments,
                        CefRefPtr<CefV8Value> &retval, CefString &exception) {
    LOGD << fmt::sprintf(" Called from v8 %s", name.ToString());
/*#ifdef DEBUG
    for (const auto x:arguments) {
        if (x->IsString()) {
            LOGD << fmt::sprintf(L"value from v8 %s", x->GetStringValue().c_str());
        } else if (x->IsInt()) {
            LOGD << fmt::sprintf(" value from v8 %d", x->GetIntValue());
        } else if (x->IsBool()) {
            LOGD << fmt::sprintf(" value from v8 bool %d", x->GetBoolValue());
        } else if (x->IsDouble()) {
            LOGD << fmt::sprintf(" value from v8 %f ", x->GetDoubleValue());
        } else {
            LOGD << fmt::sprintf(" value from v8 none normal type");
        }
    }
#endif*/
    if (name == "WinControl") {
        if (arguments.empty() ||
            !arguments[0]->IsString()) {
            exception = "internal: argument passed are wrong type or number";
            LOGE << fmt::sprintf("WinFrame Called with wrong arguments");
            return true;// without returning true the exception is not passed to JS
        }
        auto cmd = arguments[0]->GetStringValue().ToString();
        if (cmd.empty()) {
            exception = "internal: argument passed are wrong type or number";
            LOGE << fmt::sprintf("WinFrame Called with empty arguments");
            return true;// without returning true the exception is not passed to JS
        }
        CefRefPtr<CefBrowser> browser = CefV8Context::GetCurrentContext()->GetBrowser();
        CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create("WINDOW_COMMAND");
        message->GetArgumentList()->SetSize(arguments.size());
        int i = 0;
        for (const auto x:arguments) {
            if (x->IsString()) {
                message->GetArgumentList()->SetString(i, x->GetStringValue());
//                LOGD << fmt::sprintf(L"add arg %s %d", x->GetStringValue().c_str(), i);
            } else if (x->IsInt()) {
                message->GetArgumentList()->SetInt(i, x->GetIntValue());
//                LOGD << fmt::sprintf("add arg %d %d", x->GetIntValue(), i);
            } else if (x->IsBool()) {
                message->GetArgumentList()->SetBool(i, x->GetBoolValue());
//                LOGD << fmt::sprintf("add arg %d<bool> %d", x->GetBoolValue(), i);
            } else if (x->IsDouble()) {
                message->GetArgumentList()->SetDouble(i, x->GetDoubleValue());
//                LOGD << fmt::sprintf("add arg %f<double> %d", x->GetBoolValue(), i);
            } else {
                exception = "internal: argument passed are wrong type (only string int uint double bool allowed)";
                LOGE << fmt::sprintf("WinFrame Called with wrong arguments");
                return true;// without returning true the exception is not passed to JS
            }
            i++;
        }
        retval = CefV8Value::CreateBool(browser->SendProcessMessage(PID_BROWSER, message));
        return true;
    } else if (name == "DBControl") {
        if (arguments.empty() ||
            !arguments[0]->IsString()) {
            exception = "internal: argument passed are wrong type or number";
            LOGE << fmt::sprintf("WinFrame Called with wrong arguments");
            return true;// without returning true the exception is not passed to JS
        }
        auto cmd = arguments[0]->GetStringValue().ToString();
        if (cmd.empty()) {
            exception = "internal: argument passed are wrong type or number";
            LOGE << fmt::sprintf("WinFrame Called with empty arguments");
            return true;// without returning true the exception is not passed to JS
        }
        if (cmd == "open") {
            if (arguments.size() != 3 || !arguments[1]->IsString() || !arguments[2]->IsString()) {
                exception = "internal: argument passed are wrong size or type";
                LOGE << fmt::sprintf("database Called with wrong number of arguments");
                return true;
            }
            retval = CefV8Value::CreateString(
                    go.openDB(arguments[1]->GetStringValue().ToString(), arguments[2]->GetStringValue().ToString()));
            return true;
        } else if (cmd == "export") {
            if (arguments.size() != 2 || !arguments[1]->IsString()) {
                exception = "internal: argument passed are wrong size or type";
                LOGE << fmt::sprintf("database Called with wrong number of arguments");
                return true;
            }
            retval = CefV8Value::CreateString(go.exportDB(arguments[1]->GetStringValue().ToString()));
            return true;
        } else if (cmd == "close") {
            if (arguments.size() != 2 || !arguments[1]->IsString()) {
                exception = "internal: argument passed are wrong size or type";
                LOGE << fmt::sprintf("database Called with wrong number of arguments");
                return true;
            }
            retval = CefV8Value::CreateInt(go.closeDB(arguments[1]->GetStringValue().ToString()));
            return true;
        } else if (cmd == "delete") {
            if (arguments.size() != 2 || !arguments[1]->IsString()) {
                exception = "internal: argument passed are wrong size or type";
                LOGE << fmt::sprintf("database Called with wrong number of arguments");
                return true;
            }
            retval = CefV8Value::CreateInt(go.delDB(arguments[1]->GetStringValue().ToString()));
            return true;
        } else if (cmd == "exec") {
            if (arguments.size() != 3 || !arguments[1]->IsString() || !arguments[2]->IsString()) {
                exception = "internal: argument passed are wrong size or type";
                LOGE << fmt::sprintf("database Called with wrong number of arguments");
                return true;
            }
            retval = CefV8Value::CreateString(
                    go.execDB(arguments[1]->GetStringValue().ToString(), arguments[2]->GetStringValue().ToString()));
            return true;
        } else if (cmd == "execs") {
            if (arguments.size() != 3 || !arguments[1]->IsString() || !arguments[2]->IsArray() ||
                arguments[2]->GetArrayLength() == 0) {
                exception = "internal: argument passed are wrong size or type";
                LOGE << fmt::sprintf("database Called with wrong number of arguments");
                return true;
            }
            auto len = arguments[2]->GetArrayLength();
            if (len == 0) {
                exception = "internal: argument passed are wrong size or type";
                LOGE << fmt::sprintf("database Called with wrong number of arguments");
                return true;
            }
            std::string param = "[";
            for (int i = 0; i < len; ++i) {
                if (!arguments[1]->GetValue(i)->IsString()) {
                    exception = "internal: argument passed are wrong size or type";
                    LOGE << fmt::sprintf("database Called with wrong number of arguments");
                    return true;
                }
                if (i != 0) {
                    param.append(",");
                }
                param.append("\"").append(arguments[1]->GetValue(i)->GetStringValue().ToString()).append("\"");
            }
            param.append("]");
            retval = CefV8Value::CreateString(
                    go.execsDB(arguments[1]->GetStringValue().ToString(), param));
            return true;
        } else if (cmd == "query") {
            if (arguments.size() != 3 || !arguments[1]->IsString() || !arguments[2]->IsString()) {
                exception = "internal: argument passed are wrong size or type";
                LOGE << fmt::sprintf("database Called with wrong number of arguments");
                return true;
            }
            retval = CefV8Value::CreateString(go.queryDB(arguments[1]->GetStringValue().ToString(),
                                                         arguments[2]->GetStringValue().ToString()));
            return true;
        } else if (cmd == "querys") {
            if (arguments.size() != 3 || !arguments[1]->IsString() || !arguments[2]->IsArray() ||
                arguments[2]->GetArrayLength() == 0) {
                exception = "internal: argument passed are wrong size or type";
                LOGE << fmt::sprintf("database Called with wrong number of arguments");
                return true;
            }
            auto len = arguments[2]->GetArrayLength();
            if (len == 0) {
                exception = "internal: argument passed are wrong size or type";
                LOGE << fmt::sprintf("database Called with wrong number of arguments");
                return true;
            }
            std::string param = "[";
            for (int i = 0; i < len; ++i) {
                if (!arguments[1]->GetValue(i)->IsString()) {
                    exception = "internal: argument passed are wrong size or type";
                    LOGE << fmt::sprintf("database Called with wrong number of arguments");
                    return true;
                }
                if (i != 0) {
                    param.append(",");
                }
                param.append("\"").append(arguments[1]->GetValue(i)->GetStringValue().ToString()).append("\"");
            }
            param.append("]");
            retval = CefV8Value::CreateString(
                    go.querysDB(arguments[1]->GetStringValue().ToString(), param));
            return true;
        } else if (cmd == "opened") {
            retval = CefV8Value::CreateString(go.openedDB());
            return true;
        } else if (cmd == "closeAll") {
            go.closeAllDB();
            return true;
        } else {
            exception = "internal: invalid command";
            LOGE << fmt::sprintf("database Called with wrong number of arguments");
            return true;
        }
    }
    return false;
}
