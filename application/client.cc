
#include "client.h"
#include "downloadBox.hpp"


Client::Client()
        : is_closing_(false),
        win(Singleton<WindowController>::getInstance()) {}

Client::~Client() = default;


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

/*    LOGGERWLn_(L"IsComplete %d  IsInProgress %d %d %ls IsCanceled %d IsValid %d", download_item->IsComplete(),
               download_item->IsInProgress(), download_item->GetPercentComplete(),
               download_item->GetFullPath().ToWString().c_str(), download_item->IsCanceled(), download_item->IsValid())*/

    if (download_item->IsComplete()) {
        downloadBox::getInfo()["path"] = download_item->GetFullPath().ToWString();
        HWND hwndDlg = downloadBox::getHWND();
        SetWindowTextW(GetDlgItem(hwndDlg, IDCANCEL), L"关闭");
        SetWindowTextW(GetDlgItem(hwndDlg, IDD_CONTENT),
                       download_item->GetOriginalUrl().ToWString().insert(0, L"下载文件:\n").append(L" 已完成.").c_str());
        SendMessageW(GetDlgItem(hwndDlg, IDD_PROGRESS_BAR), PBM_SETMARQUEE, static_cast<WPARAM>(100), NULL);
        EnableWindow(GetDlgItem(hwndDlg, IDOK), TRUE);
        if (browser->IsPopup() && !browser->HasDocument()) {
            //browser->GetHost()->ParentWindowWillClose();
            browser->GetHost()->CloseBrowser(true);
        }
    } else if (download_item->IsInProgress() && download_item->GetPercentComplete() == 0) {
        downloadBox::ShowDownloading(
                {
                        {"url",  download_item->GetURL().ToWString()},
                        {"path", download_item->GetFullPath().ToWString()}
                }
        );
        if (browser->IsPopup() && !browser->HasDocument()) {
            PostMessage(browser->GetHost()->GetWindowHandle(), WM_SYSCOMMAND, SC_MINIMIZE, 0);
        }
    } else if (download_item->IsCanceled()) {
        HWND hwndDlg = downloadBox::getHWND();
        DestroyWindow(hwndDlg);
        if (browser->IsPopup() && !browser->HasDocument()) {
            //browser->GetHost()->ParentWindowWillClose();
            browser->GetHost()->CloseBrowser(true);
        }
    } else if (download_item->IsInProgress() && download_item->GetPercentComplete() > 0) {
        HWND hwndDlg = downloadBox::getHWND();
        if (downloadBox::getInfo()["cancel"]) {
            callback->Cancel();
            DestroyWindow(hwndDlg);
            if (browser->IsPopup() && !browser->HasDocument()) {
                //browser->GetHost()->ParentWindowWillClose();
                browser->GetHost()->CloseBrowser(true);
            }
        } else {
            downloadBox::getInfo()["isDone"] = download_item->GetPercentComplete();
            SendMessageW(GetDlgItem(hwndDlg, IDD_PROGRESS_BAR), PBM_SETPOS,
                         static_cast<WPARAM>(download_item->GetPercentComplete()), NULL);
        }

    }
}

HWND Client::getWin() {
    return win.getWin();
}

void Client::setWin(HWND win) {
    this->win.setWin(win);
}



