//
// Created by Zen Liu on 2018-10-5.
//



#include "helper_win.h"

using namespace std;



string GetAppDir() {
    char *ret = (char *) malloc(MAX_PATH);
    GetModuleFileName(nullptr, ret, MAX_PATH); // 得到当前执行文件的文件名（包含路径）
    *(strrchr(ret, '\\')) = '\0';   // 删除文件名，只留下目录
    string a = ret;
    free(ret);
    return a;
}
