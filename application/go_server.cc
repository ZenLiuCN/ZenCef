//
// Created by Zen Liu on 2018-10-6.
//

#include "go_server.h"
#include "goserver.h"

bool GoServer::start(std::string port) {
    return static_cast<bool>(goStartServer(const_cast<char *>(port.c_str())));
}


int GoServer::enableHttpServer(std::string dir) {
    return int(goUseHttpServer(const_cast<char *>(dir.c_str())));
}

void GoServer::close() {
    goStopServer();

}

void GoServer::setDebug(int i) {
    goSetDebug(i);
}

GoServer::~GoServer() {
    this->close();
}

void GoServer::closeAllDB() {
    goCloseAllDB();
}

string GoServer::openDB(string name, string password) {
    return string(goOpenDB(const_cast<char *>(name.c_str()), const_cast<char *>(password.c_str())));
}

string GoServer::queryDB(string name, string query) {
    return string(goQueryDB(const_cast<char *>(name.c_str()), const_cast<char *>(query.c_str())));
}

string GoServer::execDB(string name, string query) {
    return string(goExecDB(const_cast<char *>(name.c_str()), const_cast<char *>(query.c_str())));
}

string GoServer::execsDB(string name, string query) {
    return string(goExecsDB(const_cast<char *>(name.c_str()), const_cast<char *>(query.c_str())));
}

string GoServer::querysDB(string name, string query) {
    return string(goQuerysDB(const_cast<char *>(name.c_str()), const_cast<char *>(query.c_str())));
}

string GoServer::exportDB(string name) {
    return string(goExportDB(const_cast<char *>(name.c_str())));
}

int GoServer::delDB(string name) {
    return goDelDB(const_cast<char *>(name.c_str()));
}

int GoServer::closeDB(string name) {
    return goCloseDB(const_cast<char *>(name.c_str()));
}

string GoServer::openedDB() {
    return goOpened();
}

