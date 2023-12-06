#include "actions.h"

#define GROUP_PORT "58105"
#define DEFAULT_PORT "58011"

int udp_fd;
int tcp_fd;
ssize_t n;
socklen_t udp_addrlen;
struct addrinfo udp_hints, *udp_res;
struct sockaddr_in udp_addr;
char buffer[128];

char *ASport = DEFAULT_PORT; // Chanhge to GROUO PORT
int verbose = 0;

void server()
{
    // UDP setup
    if ((udp_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    memset(&udp_hints, 0, sizeof udp_hints);

    udp_hints.ai_family = AF_INET;
    udp_hints.ai_socktype = SOCK_DGRAM;
    udp_hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, ASport, &udp_hints, &udp_res) != 0)
    {
        perror("getaddrinfo");
        exit(1);
    }

    if (bind(udp_fd, udp_res->ai_addr, udp_res->ai_addrlen) == -1)
    {
        perror("bind");
        exit(1);
    }

    freeaddrinfo(udp_res);

    fd_set all_fds_read;
    int max_fd;

    // TCP setup
    if ((tcp_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in tcp_addr;
    memset(&tcp_addr, 0, sizeof(tcp_addr));
    tcp_addr.sin_family = AF_INET;
    tcp_addr.sin_addr.s_addr = INADDR_ANY;
    tcp_addr.sin_port = htons(atoi(ASport));

    if (bind(tcp_fd, (struct sockaddr *)&tcp_addr, sizeof(tcp_addr)) == -1)
    {
        perror("bind");
        exit(1);
    }

    if (listen(tcp_fd, SOMAXCONN) == -1)
    {
        perror("listen");
        exit(1);
    }

    while (1)
    {
        FD_ZERO(&all_fds_read);
        FD_SET(udp_fd, &all_fds_read);
        FD_SET(tcp_fd, &all_fds_read);
        max_fd = (udp_fd > tcp_fd) ? udp_fd : tcp_fd;
        if (select(max_fd + 1, &all_fds_read, NULL, NULL, NULL) < 0)
        {
            perror("select");
            exit(1);
        }
        if (FD_ISSET(udp_fd, &all_fds_read))
        {
            udp_addrlen = sizeof(udp_addr);
            if ((n = recvfrom(udp_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&udp_addr, &udp_addrlen)) == -1)
            {
                perror("recvfrom");
                exit(1);
            }

            printf("[UDP] Received: %.*s\n", (int)n, buffer);

            if (sendto(udp_fd, buffer, n, 0, (struct sockaddr *)&udp_addr, udp_addrlen) == -1)
            {
                perror("sendto");
                exit(1);
            }
        }

        if (FD_ISSET(tcp_fd, &all_fds_read))
        {
            struct sockaddr_in client_addr;
            socklen_t client_addrlen = sizeof(client_addr);
            int tcp_socket = accept(tcp_fd, (struct sockaddr *)&client_addr, &client_addrlen);

            if (tcp_socket == -1)
            {
                perror("accept");
                exit(1);
            }

            n = read(tcp_socket, buffer, 128);
            if (n == -1)
                exit(1);
        }
    }

    close(udp_fd);
    close(tcp_fd);
}

void server_arguments(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "p:v")) != -1)
    {
        switch (opt)
        {
        case 'p':
            ASport = optarg;
            break;
        case 'v':
            verbose = 1;
            break;
        default:
            abort();
            // exit(EXIT_FAILURE);
        }
    }

    // validate_port(ASport);   Used in both files may be create new .c file
}

int main(int argc, char const *argv[])
{
    server_arguments(argc, argv);
    server();
    return 0;
}
