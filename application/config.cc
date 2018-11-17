//
// Created by Zen Liu on 2018-11-15.
//

#include "config.h"

const Config conf = Config();

Config::Config() :
        LOG_CEF_FILE(GetAppDir().append("\\logs\\cef.log")),
        LOG_APP_FILE(GetAppDir().append("\\logs\\app.log")),
        LOG_APP_LEVEL(plog::info),
        CEF_CACHE_DIR(GetAppDir().append("\\caches")),
        CEF_DEFAULT_LOCALE("zh_CN"),
        CEF_DEBUG_PORT(9222) {
    mkdir(GetAppDir().append("\\logs").c_str());
    mkdir(GetAppDir().append("\\caches").c_str());
}