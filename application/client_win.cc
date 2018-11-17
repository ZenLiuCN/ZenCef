
#include "logger.h"
#include "client.h"
#include "httpClient.hpp"


void Client::OnTitleChange(CefRefPtr<CefBrowser> browser,
                           const CefString &title) {
    CEF_REQUIRE_UI_THREAD();
    CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
    SetWindowTextW(hwnd, std::wstring(title).c_str());
}

bool Client::OnKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent &event, MSG *os_event) {
    switch (event.windows_key_code) {
        case VK_F5:
            if (event.type == KEYEVENT_RAWKEYDOWN && event.modifiers == EVENTFLAG_ALT_DOWN) {
                browser->Reload();
                return true;
            }
            return false;
        case VK_F10:
            if (event.type == KEYEVENT_RAWKEYDOWN && event.modifiers == EVENTFLAG_ALT_DOWN) {
                ShowDevTool(browser);
                return true;
            }
            return false;
        default:
            return CefKeyboardHandler::OnKeyEvent(browser, event, os_event);
    }

}

void Client::ShowDevTool(const CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();
//    LOGD<<fmt::sprintf("will show dev tools %s", browser->GetMainFrame()->GetName());
    CefWindowInfo wi;
    wi.transparent_painting_enabled = 1;
    CefBrowserSettings bs;
    wi.SetAsPopup(nullptr, "DEV_TOOLS");
    browser->GetHost()->ShowDevTools(wi, this, bs, CefPoint(0, 0));
}

bool Client::OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                              const CefString &message, const CefString &source,
                              int line) {
    LOGI << fmt::sprintf(L"[console]:%ls :%d message:%ls \n", source.ToWString().c_str(), line,
                         message.ToWString().c_str());
    return false;
}

bool Client::OnPreKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent &event, MSG *os_event,
                           bool *is_keyboard_shortcut) {
    if (is_keyboard_shortcut && event.modifiers == EVENTFLAG_ALT_DOWN && event.windows_key_code == VK_TAB) {
        LOGD << fmt::sprintf("get alt_tab");
        return true;
    }
    return CefKeyboardHandler::OnPreKeyEvent(browser, event, os_event, is_keyboard_shortcut);
}

void Client::OnFaviconURLChange(CefRefPtr<CefBrowser> browser, const std::vector<CefString> &icon_urls) {
    if (!icon_urls.empty()) {
        for (const auto &x:icon_urls) {
            if (x.ToString().rfind(".ico") == x.length() - 4) {
                auto client = new HttpGetIcon(browser->GetHost()->GetWindowHandle());
                client->DoRequest(x);
                return;
            }
        }
    }
}

bool Client::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process,
                                      CefRefPtr<CefProcessMessage> message) {
    auto command = message->GetName().ToString();
    //<editor-fold desc="WINDOW COMMAND">
    if (command == "WINDOW_COMMAND") {
        auto cmd = message->GetArgumentList()->GetString(0).ToString();
        LOGD << fmt::sprintf("Get WINDOW COMMAND %s", cmd);
/*#ifdef DEBUG
        for (int i = 0; i < message->GetArgumentList()->GetSize(); ++i) {
            auto x = message->GetArgumentList()->GetValue(i);
            switch (x->GetType()) {
                case VTYPE_INVALID:
                    LOGD << fmt::sprintf(L"value from v8 INVALID %d", i);
                    break;
                case VTYPE_NULL:
                    LOGD << fmt::sprintf(" value from v8 NULL %d", i);
                    break;
                case VTYPE_BOOL:
                    LOGD << fmt::sprintf(" value from v8 %d<bool> %d", x->GetBool(), i);
                    break;
                case VTYPE_INT:
                    LOGD << fmt::sprintf(" value from v8 %d %d ", x->GetInt(), i);
                    break;
                case VTYPE_DOUBLE:
                    LOGD << fmt::sprintf(" value from v8 %f<double> %d ", x->GetDouble(), i);
                    break;
                case VTYPE_STRING:
                    LOGD << fmt::sprintf(L" value from v8 %s %d ", x->GetString().c_str(), i);
                    break;
                case VTYPE_BINARY:
                case VTYPE_DICTIONARY:
                case VTYPE_LIST:
                default:
                    LOGD << fmt::sprintf(" value from v8 none normal type %d", i);

            }

        }
#endif*/
        if (cmd == "close") {
            return this->win.Close();
        } else if (cmd == "max") {
            return this->win.Max();
        } else if (cmd == "min") {
            return this->win.Min();
        } else if (cmd == "full") {
            return this->win.FullScreen();
        } else if (cmd == "restore") {
            return this->win.Restore();
        } else if (cmd == "topmost") {
            return this->win.TopMost();
        } else if (cmd == "nonetop") {
            return this->win.NotTopMost();
        } else if (cmd == "fullscreen") {
            return this->win.Solo();
        } else if (cmd == "normal") {
            return this->win.FrameNormal();
        } else if (cmd == "less") {
            return this->win.FrameLess();
        } else if (cmd == "thin") {
            return this->win.FrameThin();
        } else if (cmd == "drag") {
            return this->win.Drag();
        } else if (cmd == "drop") {
            return this->win.Drop();
        } else if (cmd == "move" && message->GetArgumentList()->GetSize() == 3) {
            try {
                int x = message->GetArgumentList()->GetInt(1);
                int y = message->GetArgumentList()->GetInt(2);
                return this->win.Move(x, y);
            } catch (std::exception &) {
                return false;
            }
        } else {
            return false;
        }
    }
//</editor-fold>
    return
            CefClient::OnProcessMessageReceived(browser, source_process, message
            );
}

void Client::OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen) {
    if (fullscreen) {
        this->win.FullScreen();
    } else {
        this->win.Restore();
    }
}
