//
// Created by Zen Liu on 2018-11-15.
//

#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#include "helper_win.h"
#include <plog/Log.h>
struct Config{
    const std::string LOG_CEF_FILE ;
    const std::string  LOG_APP_FILE ;
    const int LOG_APP_LEVEL ;
    const std::string CEF_CACHE_DIR;
    const std::string CEF_DEFAULT_LOCALE ;
    const int CEF_DEBUG_PORT;
    Config();
};
extern const Config conf;
#endif //PROJECT_CONFIG_H
