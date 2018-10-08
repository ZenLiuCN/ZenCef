//
// Created by Zen Liu on 2018-10-6.
//

#ifndef ZEN_CEF_GO_SERVER_H
#define ZEN_CEF_GO_SERVER_H

#include  "goserver.h"
//#include "WsServer.hpp"
#include <string>

//using namespace WebSocket;
class GoServer {
public:
    GoServer();
    ~GoServer();
    void start(std::string port);

    void enableApiServer();
//    void startInnerWs(HWND win,int port);

    int enableHttpServer(std::string dir);
    void close();
    static void runSchemeCommand(std::string url);
    void setDebug(int i);

private:
//   WebSocketServer *ws= nullptr;
};

#endif //ZEN_CEF_GO_SERVER_H
