//
// Created by Zen Liu on 2018-10-6.
//

#include "go_server.h"

void GoServer::start(std::string port) {
    goStartServer(const_cast<char *>(port.c_str()));
}

void GoServer::enableApiServer() {
    goUserApiServer();
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

GoServer::GoServer() = default;

