//
// Created by bmiem on 2021/3/8.
//

#ifndef TOY_WEB_TOY_WEB_H
#define TOY_WEB_TOY_WEB_H

#define MAX_LINE 8192
#include <string>
#include <iostream>

#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>

using std::string;
using std::cerr;
using std::cout;
using std::endl;

int listen_on(char *port);
void error_handling(string msg);
void client_info(const int& sock, const struct sockaddr_in& addr);
void request_handler(int sock);
void client_error(FILE *_write, const string &cause, const string &code,
                  const string &short_msg, const string &long_msg);
void read_request_headers(FILE *_read);
bool parse_uri(char *uri, char *filename, char *cgi_args);
void serve_static(FILE *_write, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(FILE *_write, char *filename, char *cgi_args);

#endif //TOY_WEB_TOY_WEB_H
