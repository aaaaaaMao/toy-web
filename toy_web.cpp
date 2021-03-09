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
    return sock;
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
    cout << buf << endl;

    char method[MAX_LINE], uri[MAX_LINE], version[MAX_LINE];
    sscanf(buf, "%s %s %s", method, uri, version);
    if (strcasecmp(method, "GET") != 0) {
        client_error(_write, method, "501", "Not Implemented",
                     "Toy does not implement this method");
        return;
    }

    read_request_headers(_read);
    fclose(_read);

    /* Parse URI from GET request */
    char file_name[MAX_LINE], cgi_args[MAX_LINE];
    bool is_static = parse_uri(uri, file_name, cgi_args);
    struct stat file_stat;
    if (stat(file_name, &file_stat) < 0) {
        client_error(_write, file_name, "404", "Not found",
                    "Toy couldn't find this file");
        return;
    }

    if (is_static) {
        /* Serve static content */
        if (!(S_ISREG(file_stat.st_mode)) || !(S_IRUSR & file_stat.st_mode)) {
            client_error(_write, file_name, "403", "Forbidden",
                        "Toy couldn't read the file");
            return;
        }
        serve_static(_write, file_name, file_stat.st_size);
    } else {
        /* Serve dynamic content */
        if (!(S_ISREG(file_stat.st_mode)) || !(S_IXUSR & file_stat.st_mode)) {
            client_error(_write, file_name, "403", "Forbidden",
                        "Toy couldn't run the CGI program");
            return;
        }
        serve_dynamic(_write, file_name, cgi_args);
    }

    fflush(_write);
    fclose(_write);
}

void client_error(FILE *_write, const string &cause, const string &code,
                  const string &short_msg, const string &long_msg) {
    string result = "HTTP/1.0 " + code + " " + short_msg + "\r\n" +
            "Content-type: text/html\r\n\r\n" +
            "<html><title>Tiny Error</title>" +
            "<body>\r\n" +
            code + ": " + short_msg +
            "<p>" + long_msg + ": " + cause + "\r\n" +
            "<hr><em>The Toy Web server</em>\r\n</body></html>";
    fputs(result.c_str(), _write);
    fflush(_write);
    fclose(_write);
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

bool parse_uri(char *uri, char *file_name, char *cgi_args) {
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

void serve_static(FILE *_write, char *filename, int filesize) {

    char filetype[MAX_LINE], buf[MAX_LINE];

    /* Send response headers to client */
    get_filetype(filename, filetype);
    fputs("HTTP/1.0 200 OK\r\n", _write);
    fputs("Server: Toy Web Server\r\n", _write);

    sprintf(buf, "Content-length: %d\r\n", filesize);
    fputs(buf, _write);

    sprintf(buf, "Content-type: %s\r\n\r\n", filetype);
    fputs(buf, _write);

    /* Send response body to client */
    FILE *content = fopen(filename, "r");
    while (fgets(buf, MAX_LINE, content) != nullptr) {
        fputs(buf, _write);
    }
    fclose(content);
}

void get_filetype(char *filename, char *filetype) {
    if (strstr(filename, ".html"))
        strcpy(filetype, "text/html");
    else if (strstr(filename, ".gif"))
        strcpy(filetype, "image/gif");
    else if (strstr(filename, ".png"))
        strcpy(filetype, "image/png");
    else if (strstr(filename, ".jpg"))
        strcpy(filetype, "image/jpeg");
    else
        strcpy(filetype, "text/plain");
}

void serve_dynamic(FILE *_write, char *filename, char *cgi_args)
{
    char buf[MAX_LINE], *empty_list[] = { nullptr };

    /* Return first part of HTTP response */
    fputs("HTTP/1.0 200 OK\r\n", _write);
    fputs("Server: Toy Web Server\r\n", _write);

//    if (Fork() == 0) { /* Child */ //line:netp:servedynamic:fork
//        /* Real server would set all CGI vars here */
//        setenv("QUERY_STRING", cgiargs, 1); //line:netp:servedynamic:setenv
//        Dup2(fd, STDOUT_FILENO);         /* Redirect stdout to client */ //line:netp:servedynamic:dup2
//        Execve(filename, emptylist, environ); /* Run CGI program */ //line:netp:servedynamic:execve
//    }
//    Wait(NULL); /* Parent waits for and reaps child */ //line:netp:servedynamic:wait
}

void error_handling(string msg) {
    cerr << msg << endl;
}