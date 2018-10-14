#include "app.h"



App::App(std::string root, std::string port, std::string url, CefBrowserSettings browser_settings, bool enableFlash) {
    this->go = (new GoServer);
    this->uri = std::move(url);
    this->portStr = std::move(port);
    this->root = std::move(root);
    this->browserSettings = browser_settings;
    this->enableFlash = enableFlash;

};

void App::OnContextInitialized() {

    CEF_REQUIRE_UI_THREAD();

    // Information used when creating the native window.
    CefWindowInfo window_info;

#if defined(OS_WIN)
    // On Windows we need to specify certain flags that will be passed to
    // CreateWindowEx().
    window_info.SetAsPopup(nullptr, "CEF");
#endif

    // LifeSpanHandler implements browser-level callbacks.
    CefRefPtr<Client> handler(new Client());

    std::string url, port, dir, flash;


    //<editor-fold desc="ParseCommandLine">
    CefRefPtr<CefCommandLine> command_line =
            CefCommandLine::GetGlobalCommandLine();
    url = command_line->GetSwitchValue("url");
    if (url.empty())
        url = this->uri;//"file:///" + GetAppDir() + "/test.html";
    dir = command_line->GetSwitchValue("dir");
    if (dir.empty())
        dir = this->root;
    port = command_line->GetSwitchValue("port");
    if (port.empty())
        port = this->portStr;
    flash = command_line->GetSwitchValue("flash");
    if (!flash.empty()) {
        this->enableFlash = flash == "true";
    }
    //</editor-fold>
    // Create the first browser window.
    auto br = CefBrowserHost::CreateBrowserSync(window_info, handler.get(), url, browserSettings, nullptr);


    //set window border
    HWND win = br->GetHost()->GetWindowHandle();
//    this->handler=new JsActionHandler(win);
#ifndef DEBUG
    SetWindowLong(win, GWL_STYLE, GetWindowLong(win, GWL_STYLE) ^ (WS_CAPTION));
    SetWindowPos(win, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
#endif
    //<editor-fold desc="SetIconFromBrowserWindow">
    HICON ico=LoadIcon(GetModuleHandle(nullptr),MAKEINTRESOURCE(IDI_BIG));
    SendMessage(win,WM_SETICON,ICON_BIG,(LPARAM)ico);
    SendMessage(win,WM_SETICON,ICON_SMALL,(LPARAM)ico);
    //</editor-fold>
#ifndef DEBUG
    LOGGER_("get browser window handler %p", win)
    go->setDebug(1);
#endif
    goSetHwnd(win);
    go->enableWinServer();
    go->enableDBServer();
    //开启目录服务

    if (!dir.empty())
        go->enableHttpServer(dir);
//    go->startInnerWs(win,stoi(port));

    go->start(port);
    CefRegisterSchemeHandlerFactory("window","local",new WindowSchemeHandlerFactory(win));
    CefRegisterSchemeHandlerFactory("ws","window",new WebSocketSchemeHandlerFactory());
}

//TODO 2018-10-5 14:42 Zen Liu: NOT EFFECTED
void App::OnWebKitInitialized() {
/*    auto js = "var WinClose=function(){native function WindowClose();return WindowClose();};";
    LOGGER_("register js code :%s", js)
    CefRegisterExtension("v8/test", CefStringUTF16(js), this->handler);*/
//    CefRenderProcessHandler::OnWebKitInitialized();
}

void App::OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar) {
    registrar->AddCustomScheme("window:", true,true,false);

//    CefApp::OnRegisterCustomSchemes(registrar);
}


void App::OnBeforeCommandLineProcessing(const CefString &process_type, CefRefPtr<CefCommandLine> command_line) {
    if (enableFlash) {
        command_line->AppendSwitchWithValue("ppapi-flash-version", "30.0.0.118");
        command_line->AppendSwitchWithValue("ppapi-flash-path", "pepflashplayer32_31_0_0_118.dll");
    }
    CefApp::OnBeforeCommandLineProcessing(process_type, command_line);
}



