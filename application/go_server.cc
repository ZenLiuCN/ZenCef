//
// Created by Zen Liu on 2018-10-6.
//

#include "go_server.h"


void GoServer::start(std::string port) {
    goStartServer(const_cast<char *>(port.c_str()));
}

void GoServer::enableWinServer() {
    goUseWinServer();
}

int GoServer::enableHttpServer(std::string dir) {
    return int(goUseHttpServer(const_cast<char *>(dir.c_str())));
}

void GoServer::close() {
    goStopServer();

}

GoServer::~GoServer() {
    this->close();
}

void GoServer::setDebug(int i) {
    goSetDebug(i);
}

void GoServer::runSchemeCommand(std::string url) {
    goRunSchemeCommand(const_cast<char *>(url.c_str()));
}

void GoServer::enableDBServer() {
    goUseDBServer();
}

/*void GoServer::startInnerWs(HWND win,int port) {
    this->ws=WebSocketServer(win,port).INSTANCE();
}*/

GoServer::GoServer() = default;

