//
// Created by Zen Liu on 2018-11-15.
//

#include "logger.h"
#include "config.h"
#include <plog/Appenders/ColorConsoleAppender.h>



void InitLogger() {
    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(conf.LOG_APP_FILE.c_str(), 1024 * 1024 * 1024, 10);
#ifdef DEBUG
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::verbose,&consoleAppender).addAppender(&fileAppender);
#else
    plog::init(plog::Severity(conf.LOG_APP_LEVEL),&fileAppender);
#endif
}
