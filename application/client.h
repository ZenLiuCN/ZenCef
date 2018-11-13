
#ifndef ZEN_CEF_HANDLER_H_
#define ZEN_CEF_HANDLER_H_

#include "include/cef_client.h"
#include <goserver.h>
#include <include/wrapper/cef_helpers.h>
#include "include/wrapper/cef_closure_task.h"
#include "include/base/cef_bind.h"
#include <include/cef_app.h>
#include "helper_win.h"
#include "debug.h"
#include <list>

enum MenuID {
    MENU_SHOW_DEV_TOOLS
};

class Client : public CefClient,
               public CefDisplayHandler,
               public CefLifeSpanHandler,
               public CefContextMenuHandler,
               public CefKeyboardHandler,
               public CefDownloadHandler,
               public CefLoadHandler {
public:
    Client();

    ~Client() override;

    bool OnConsoleMessage(CefRefPtr<CefBrowser> browser, const CefString &message, const CefString &source,
                          int line) override;

    // Provide access to the single global instance of this object.
    static Client *INSTANCE();

    //<editor-fold desc="CefClientMethods">
    CefRefPtr<CefDownloadHandler> GetDownloadHandler() OVERRIDE {
        return this;
    }
    CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE {
        return this;
    }

    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE {
        return this;
    }

    CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE {
        return this;
    }

    CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override { return this; }

    CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() override { return this; }
    //</editor-fold>

    //<editor-fold desc="CefDisplayHandler methods">

    void OnTitleChange(CefRefPtr<CefBrowser> browser,
                       const CefString &title) OVERRIDE;
    //</editor-fold>

    //<editor-fold desc="CefLifeSpanHandler methods">


    void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;

    bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

    void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
    //</editor-fold>

    //<editor-fold desc="CefLoadHandlerMethods">

    void OnLoadError(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefFrame> frame,
                     ErrorCode errorCode,
                     const CefString &errorText,
                     const CefString &failedUrl) OVERRIDE;
    //</editor-fold>

    //<editor-fold desc="CefContextMenuHandlerMethods">

    void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                             CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) override;

    bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                              CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags) override;
    //</editor-fold>


    //<editor-fold desc="CefKeyboardHandlerMethods">
    bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent &event, MSG *os_event,
                       bool *is_keyboard_shortcut) override;
    bool OnKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent &event, MSG *os_event) override;
    //</editor-fold>

    // Request that all existing browser windows close.
    void CloseAllBrowsers(bool force_close);

    bool IsClosing() const { return is_closing_; }

    //<editor-fold desc="Download Handler">
    void OnBeforeDownload(
            CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefDownloadItem> download_item,
            const CefString& suggested_name,
            CefRefPtr<CefBeforeDownloadCallback> callback) override;
    void OnDownloadUpdated(
            CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefDownloadItem> download_item,
            CefRefPtr<CefDownloadItemCallback> callback) override;
    //</editor-fold>


private:
    // List of existing browser windows. Only accessed on the CEF UI thread.
    typedef std::list<CefRefPtr<CefBrowser> > BrowserList;
    BrowserList browser_list_;

    bool is_closing_;
    void ShowDevTool(const CefRefPtr<CefBrowser> browser);

    // Include the default reference counting implementation.
IMPLEMENT_REFCOUNTING(Client);
};

#endif  // ZEN_CEF_HANDLER_H_
