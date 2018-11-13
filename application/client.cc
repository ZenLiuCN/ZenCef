
#include "client.h"

namespace {

    Client *g_instance = nullptr;

}  // namespace

Client::Client()
        : is_closing_(false) {
    DCHECK(!g_instance);
    g_instance = this;
}

Client::~Client() {
    g_instance = nullptr;
}

// static
Client *Client::INSTANCE() {
    return g_instance;
}

void Client::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();

    // Add to the list of existing browsers.
    browser_list_.push_back(browser);
}

bool Client::DoClose(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();

    // Closing the main window requires special handling. See the DoClose()
    // documentation in the CEF header for a detailed destription of this
    // process.
    if (browser_list_.size() == 1) {
        // Set a flag to indicate that the window close should be allowed.
        is_closing_ = true;
    }

    // Allow the close. For windowed browsers this will result in the OS close
    // event being sent.
    return false;
}

void Client::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();

    // Remove from the list of existing browsers.
    auto bit = browser_list_.begin();
    for (; bit != browser_list_.end(); ++bit) {
        if ((*bit)->IsSame(browser)) {
            browser_list_.erase(bit);
            break;
        }
    }

    if (browser_list_.empty()) {
        // All browser windows have closed. Quit the application message loop.
        CefQuitMessageLoop();
    }
}

void Client::OnLoadError(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         ErrorCode errorCode,
                         const CefString &errorText,
                         const CefString &failedUrl) {
    CEF_REQUIRE_UI_THREAD();

    // Don't display an error for downloaded files.
    if (errorCode == ERR_ABORTED)
        return;

    // Display a load error message.
    std::stringstream ss;
    ss << "<html><body bgcolor=\"white\">"
          "<h2>Failed to load URL " << std::string(failedUrl) <<
       " with error " << std::string(errorText) << " (" << errorCode <<
       ").</h2></body></html>";
    frame->LoadString(ss.str(), failedUrl);
}

void Client::CloseAllBrowsers(bool force_close) {
    if (!CefCurrentlyOn(TID_UI)) {
        // Execute on the UI thread.
        CefPostTask(TID_UI, base::Bind(&Client::CloseAllBrowsers, this, force_close));
        return;
    }

    if (browser_list_.empty())
        return;

    BrowserList::const_iterator it = browser_list_.begin();
    for (; it != browser_list_.end(); ++it)
        (*it)->GetHost()->CloseBrowser(force_close);
}




void Client::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                 CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) {
    model->Clear();
#ifdef DEBUG
    model->AddItem(MENU_SHOW_DEV_TOOLS, "开发者选项");
#endif
//    CefContextMenuHandler::OnBeforeContextMenu(browser, frame, params, model);
}

bool Client::OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                  CefRefPtr<CefContextMenuParams> params, int command_id,
                                  CefContextMenuHandler::EventFlags event_flags) {

    switch (command_id) {
        case (int) MENU_SHOW_DEV_TOOLS:
            ShowDevTool(browser);
            return true;
        default:
            return false;
    }
    // return CefContextMenuHandler::OnContextMenuCommand(browser, frame, params, command_id, event_flags);
}

void Client::OnBeforeDownload(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item,
                              const CefString &suggested_name, CefRefPtr<CefBeforeDownloadCallback> callback) {
    callback->Continue(download_item->GetURL(), true);
}

void Client::OnDownloadUpdated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item,
                               CefRefPtr<CefDownloadItemCallback> callback) {
    if (download_item->IsComplete())
    {
        auto str=download_item->GetFullPath().ToWString();
        auto path=download_item->GetFullPath().ToWString();
        WCHAR_LOGGER_(L"download path %ls",str.c_str())
        MessageBoxW(nullptr,str.append(L"下载完成").c_str(),L"下载完成",MB_OK|MB_ICONINFORMATION);
        ShellExecuteW(nullptr, nullptr,L"explorer.exe",path.insert(0,L"/select,\"").append(L"\"").c_str(), nullptr, SW_SHOWNORMAL);
        WCHAR_LOGGER_(L"download param %ls",path.c_str())
        if (browser->IsPopup() && !browser->HasDocument())
        {
            //browser->GetHost()->ParentWindowWillClose();
            browser->GetHost()->CloseBrowser(true);
        }
    }
}



