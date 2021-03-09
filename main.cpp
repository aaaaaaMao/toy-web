#include "toy_web.h"

int main(int argc, char **argv)
{
    int server_sock, client_sock;
    socklen_t client_addr_len;
    struct sockaddr_in client_addr;

    /* Check command line args */
    if (argc != 2) {
        cerr << "usage: " << argv[0] << " <port>" << endl;
        exit(1);
    }

    server_sock = listen_on(argv[1]);
    while (true) {
        client_addr_len = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_sock == -1) {
            error_handling("accept() error");
        }

        client_info(client_sock, client_addr);

        request_handler(client_sock);

        close(client_sock);
    }
    close(server_sock);
}

