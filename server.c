#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main()
{

    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    struct addrinfo hints, *res;
    int yes = 1;
    int sfd, new_fd;
    int max_len = 1000;
    char request[1000];

    char *status_line = "HTTP/1.1 200 OK\r\n";
    char *headers = "Content-Type: text/html\r\n\r\n";
    char *response_body = "<html><p>aek</p></html>";

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;     
    hints.ai_socktype = SOCK_STREAM; 
    hints.ai_flags = AI_PASSIVE; 

    getaddrinfo(NULL, "3490", &hints, &res);

    sfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

    bind(sfd, res->ai_addr, res->ai_addrlen);

    listen(sfd, 20);

    while(1)
    {
        new_fd = accept(sfd, (struct sockaddr *)&their_addr, &addr_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }
        memset(request, 0, max_len);
        recv(new_fd, request, max_len, 0);

        if (strncmp(request, "GET", 3) == 0)
        {

            send(new_fd, status_line, strlen(status_line), 0);
            send(new_fd, headers, strlen(headers), 0);

            FILE *index_file = fopen("index.html", "r");
            char c;
            while ((c = getc(index_file)) != EOF)
            {
                send(new_fd, &c, 1, 0);
            }
        }

        close(new_fd);
    }

    close(new_fd);
    close(sfd);
}