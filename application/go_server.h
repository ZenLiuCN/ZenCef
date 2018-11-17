//
// Created by Zen Liu on 2018-10-6.
//

#ifndef ZEN_CEF_GO_SERVER_H
#define ZEN_CEF_GO_SERVER_H

#include  "goserver.h"
//#include "WsServer.hpp"
#include <string>

using std::string;

class GoServer {
public:
    GoServer() = default;

    ~GoServer();

    bool start(string port);

    int enableHttpServer(string dir);

    void close();

    void setDebug(int i);

    string openDB(string name, string password);

    string queryDB(string name, string query);

    string execDB(string name, string query);

    string execsDB(string name, string query);

    string querysDB(string name, string query);

    string exportDB(string name);
    string openedDB();

    int delDB(string name);

    int closeDB(string name);

    void closeAllDB();

private:
//   WebSocketServer *ws= nullptr;
};

#endif //ZEN_CEF_GO_SERVER_H
