//
// Created by Zen Liu on 2018-10-6.
//

#ifndef PROJECT_GO_SERVER_H
#define PROJECT_GO_SERVER_H

#import "goserver.h"
#include <string>

class GoServer {
public:
    GoServer();
    ~GoServer();
    void start(std::string port);

    void enableApiServer();

    int enableHttpServer(std::string dir);
    void close();

    void setDebug(int i);
};

#endif //PROJECT_GO_SERVER_H
