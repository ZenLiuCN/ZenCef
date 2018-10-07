//
// Created by Zen Liu on 2018-10-5.
//

#ifndef ZEN_CEF_DEBUG_H
#define ZEN_CEF_DEBUG_H

#include <time.h>
#include <fcntl.h>

#ifndef NOLOG
#include "goserver.h"
#include <string>
#if (defined(WIN32))||(defined(_WIN32))
#define UNICODE_OUT(){  _setmode(_fileno(stdout), _O_U16TEXT);}
#define NORMAL_OUT(){  _setmode(_fileno(stdout), _O_TEXT);}
#endif
#define LOG_INFO(){\
    time_t t;struct tm *p;time(&t);p=localtime(&t);\
    printf("[CEF] %d-%d-%d %d:%d:%d %s:%d <%s> :\n",1900+p->tm_year,1+p->tm_mon,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec,\
            __FILE__,__LINE__,__func__);\
            }
#define LOGGER_(format, ...){\
    LOG_INFO();\
    char str2[4096 + 1];\
    snprintf(str2, 4096, format, ##__VA_ARGS__);\
    printf(" %s\n", str2);\
    }
#define WCHAR_LOGGER_(format, ...){\
    LOG_INFO();\
    wchar_t str2[4096 + 1];\
    snwprintf(str2, 4096, format, ##__VA_ARGS__);\
    UNICODE_OUT();\
    wprintf(L"%ls",str2);\
    NORMAL_OUT();\
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
#endif //ZEN_CEF_DEBUG_H
