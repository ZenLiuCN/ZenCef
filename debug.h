//
// Created by Zen Liu on 2018-10-5.
//

#ifndef PROJECT_DEBUG_H
#define PROJECT_DEBUG_H

#include <time.h>
#include <fcntl.h>

#ifndef NOLOG

#include "goserver.h"
#include <string>

#define LOGGER_(format, ...){\
    time_t t;struct tm *p;time(&t);p=localtime(&t);\
    printf("[CEF] %d-%d-%d %d:%d:%d %s:%d <%s> :\n",1900+p->tm_year,1+p->tm_mon,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec,\
            __FILE__,__LINE__,__func__);\
    char str2[4096 + 1];\
    snprintf(str2, 4096, format, ##__VA_ARGS__);\
    printf(" %s\n", str2);\
    }
#define GO_LOGGER_(format, ...){\
    char str2[4096 + 1];\
    snprintf(str2, 4096, format, ##__VA_ARGS__);\
    std::string f=format;\
    std::string m=str2;\
    std::string fn=__FUNCTION__;\
    std::string fl=__FILE__;\
goLog(const_cast<char*>(fl.c_str()),__LINE__, const_cast<char *>(fn.c_str()), const_cast<char*>(f.c_str()),const_cast<char*>(m.c_str()));\
    }
#endif
#define CONSOLE_LOGGER_(m){\
    time_t t;struct tm *p;time(&t);p=localtime(&t);\
    printf("[CON] %d-%d-%d %d:%d:%d %s:%d :\n\n",1900+p->tm_year,1+p->tm_mon,p->tm_mday\
    ,p->tm_hour,p->tm_min,p->tm_sec,\
            __FILE__,__LINE__);\
        wprintf(L"%ls\n",m);\
    }
#endif //PROJECT_DEBUG_H
