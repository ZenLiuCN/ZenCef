#include "res/resource.h"
#include "logger.h"
#include "app.h"
#include "v8handler.h"
#include "singleton.h"

App::App(std::string root, std::string port, std::string url, CefBrowserSettings browser_settings, bool enableFlash) {
    this->go = Singleton<GoServer>::getInstance();
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
    CefRefPtr<Client> handler(&Singleton<Client>::getInstance());
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
    handler->setWin(win);

#ifdef DEBUG
    handler->win.FrameThin();
#endif
    handler->win.SetIcon(IDI_BIG);
#ifdef DEBUG
    LOGD<<fmt::sprintf("get browser window handler 0x%.8X", (int64_t)win);
    go.setDebug(0);
#else
    go.setDebug(2);
#endif
    goSetHwnd(win);
/*    go->enableWinServer();
    go->enableDBServer();*/
    //开启目录服务
    if (!dir.empty())
        go.enableHttpServer(dir);

    if (!go.start(port)) {
        CefShutdown();
    }

}


void App::OnWebKitInitialized() {
    auto code = "var CefWin={"
                "moving:false,"
                "thin:function(){native function WinControl();return WinControl('thin')},"
                "full:function(){native function WinControl();return WinControl('full')},"
                "normal:function(){native function WinControl();return WinControl('normal')},"
                "less:function(){native function WinControl();return WinControl('less')},"
                "fullscreen:function(){native function WinControl();return WinControl('fullscreen')},"
                "min:function(){native function WinControl();return WinControl('min')},"
                "max:function(){native function WinControl();return WinControl('max')},"
                "restore:function(){native function WinControl();return WinControl('restore')},"
                "topmost:function(){native function WinControl();return WinControl('topmost')},"
                "nonetop:function(){native function WinControl();return WinControl('nonetop')},"
                "close:function(){native function WinControl();this.moving=true;return WinControl('close')},"
                "drag:function(){native function WinControl();this.moving=true;return WinControl('drag')},"
                "drop:function(){native function WinControl();this.moving=false;return WinControl('drop')},"
                "move:function(ev){eve=window.event||ev;if(this.moving&&eve.button===0){return this._domove(eve.screenX,eve.screenY)}},"
                "onDrag:function(){this.drag();document.addEventListener('mousemove',(e)=>this.move(e),true)},"
                "offDrag:function(){this.drop();document.removeEventListener('mousemove',(e)=>this.move(e),true)},"
                "_domove:function(x,y){native function WinControl();return WinControl('move',x,y)}"
                "};"
                "var CefDB={"
                "Open:function(name,password){native function DBControl();return JSON.parse(DBControl('open',name,password))},"
                "Close:function(name){native function DBControl();return DBControl('close',name)},"
                "Export:function(name){native function DBControl();return JSON.parse(DBControl('export',name))},"
                "Delete:function(name){native function DBControl();return DBControl('delete',name)},"
                "Exec:function(name,query){native function DBControl();return JSON.parse(DBControl('exec',name,query))},"
                "Query:function(name,query){native function DBControl();return JSON.parse(DBControl('query',name,query))},"
                "ExecBatch:function(name,querys){native function DBControl();return JSON.parse(DBControl('execs',name,querys))},"
                "QueryBatch:function(name,querys){native function DBControl();return JSON.parse(DBControl('querys',name,querys))},"
                "OpenList:function(){native function DBControl();return JSON.parse(DBControl('opened'))},"
                "CloseAll:function(){native function DBControl();return DBControl('closeAll')},"
                "}";
    CefRegisterExtension("v8/cef", code, &Singleton<V8Handler>::getInstance());
}

void App::OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar) {

}


void App::OnBeforeCommandLineProcessing(const CefString &process_type, CefRefPtr<CefCommandLine> command_line) {
    if (enableFlash) {
        command_line->AppendSwitchWithValue("ppapi-flash-version", "30.0.0.118");
        command_line->AppendSwitchWithValue("ppapi-flash-path", "pepflashplayer32_31_0_0_118.dll");
    }
    CefApp::OnBeforeCommandLineProcessing(process_type, command_line);
}



