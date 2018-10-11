//
// Created by Zen Liu on 2018-10-8.
//

#ifndef PROJECT_COSOLE_H
#define PROJECT_COSOLE_H

#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <windows.h>
void InitConsole() {
    int nRet = 0;
    FILE *fp;
    AllocConsole();
    nRet = _open_osfhandle((long) GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
    fp = _fdopen(nRet, "w");
    *stdout = *fp;
    setvbuf(stdout, nullptr, _IONBF, 0);
}
#endif //PROJECT_COSOLE_H
