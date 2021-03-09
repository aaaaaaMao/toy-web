#include "toy_web.h"

int listen_on(char *port) {
    struct sockaddr_in server_addr;
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        error_handling("socket() error");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(port));

    if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        error_handling("bind() error");
    }

    if (listen(sock, 5) == -1) {
        error_handling("listen() error");
    }
}

void client_info(const int& sock, const struct sockaddr_in& addr) {
    cout << "connect client: " << sock
         << ", ip: " << inet_ntoa(addr.sin_addr)
         << ", port: " << ntohs(addr.sin_port)
         << endl;
}

void request_handler(int sock) {
    FILE *_read = fdopen(sock, "r");
    FILE *_write = fdopen(dup(sock), "w");

    /* Read request line and headers */
    char buf[MAX_LINE];
    if (fgets(buf, MAX_LINE, _read) == nullptr) {
        return;
    }
    cout << buf;

    char method[MAX_LINE], uri[MAX_LINE], version[MAX_LINE];
    sscanf(buf, "%s %s %s", method, uri, version);
    if (strcasecmp(method, "GET") != 0) {
        client_error(_write, method, "501", "Not Implemented",
                     "Toy does not implement this method");
        return;
    }

    read_request_headers(_read);

    /* Parse URI from GET request */
    char file_name[MAX_LINE], cgi_args[MAX_LINE];
    bool is_static = parse_uri(uri, file_name, cgi_args);
}

void client_error(FILE *_write, const string &cause, const string &code,
                  const string &short_msg, const string &long_msg) {
    string result = "HTTP/1.0 " + code + " " + short_msg + "\r\n" +
            "Content-type: text/html\r\n\r\n" +
            "<html><title>Tiny Error</title>" +
            R"(<body bgcolor=""ffffff"">\r\n)" +
            code + ": " + short_msg +
            "<p>" + long_msg + ": " + cause + "\r\n" +
            "<hr><em>The Tiny Web server</em>\r\n";
    fputs(result.c_str(), _write);
}

void read_request_headers(FILE *_read) {
    char buf[MAX_LINE];

    fgets(buf, MAX_LINE, _read);
    cout << buf;
    while (strcmp(buf, "\r\n") != 0) {
        fgets(buf, MAX_LINE, _read);
        cout << buf;
    }
}

bool parse_uri(char *uri, char *file_name, char *cgi_args)
{
    char *ptr;

    if (!strstr(uri, "cgi-bin")) {
        /* Static content */
        strcpy(cgi_args, "");
        strcpy(file_name, ".");
        strcat(file_name, uri);
        if (uri[strlen(uri)-1] == '/')
            strcat(file_name, "index.html");
        return true;
    }
    else {
        /* Dynamic content */
        ptr = index(uri, '?');
        if (ptr) {
            strcpy(cgi_args, ptr+1);
            *ptr = '\0';
        }
        else
            strcpy(cgi_args, "");
        strcpy(file_name, ".");
        strcat(file_name, uri);
        return false;
    }
}
