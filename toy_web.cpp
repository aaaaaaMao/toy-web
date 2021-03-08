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
