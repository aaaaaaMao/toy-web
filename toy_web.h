//
// Created by bmiem on 2021/3/8.
//

#ifndef TOY_WEB_TOY_WEB_H
#define TOY_WEB_TOY_WEB_H

#define MAX_LINE 8192
#include <string>
#include <iostream>

#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

using std::string;
using std::cerr;
using std::cout;
using std::endl;

int listen_on(char *port);
void error_handling(string msg);
void client_info(const int& sock, const struct sockaddr_in& addr) {
    cout << "connect client: " << sock
        << ", ip: " << inet_ntoa(addr.sin_addr)
        << ", port: " << ntohs(addr.sin_port)
        << endl;
}

#endif //TOY_WEB_TOY_WEB_H
