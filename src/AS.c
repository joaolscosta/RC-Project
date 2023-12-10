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

void udp_message_handle(ssize_t n, char buffer[])
{
    char reply[128];
    char code[4];
    char reply_code[4];
    sscanf(buffer, "%s ", code);
    if (strcmp(code, "LIN") == 0) // Login
    {
        char uid[UID_SIZE], pass[PASS_SIZE];
        sscanf(buffer, "%*s %s %s", uid, pass);
        strcpy(reply_code, "RLI");
        // DUVIDA PRECISO DE MANDAR ALGO AO USER A DIZER Q N DEU OU SO ESCREVO NO TERMINAL DO SERVER?
        if (verify_user_credentials(uid, pass))
        {
            char status[4];
            int result = login_user(uid, pass);
            switch (result)
            {
            case 0:
                strcpy(status, "NOK");
                sprintf(reply, "%s %s", reply_code, status);
                break;
            case 1:
                strcpy(status, "OK");
                sprintf(reply, "%s %s", reply_code, status);
                break;
            case 2:
                strcpy(status, "REG");
                sprintf(reply, "%s %s", reply_code, status);
                break;
            }
        }
    }
    else if (strcmp(code, "LOU") == 0) // Logout
    {
        char uid[UID_SIZE], pass[PASS_SIZE];
        sscanf(buffer, "%*s %s %s", uid, pass);
        strcpy(reply_code, "RLO");
        if (verify_user_credentials(uid, pass))
        {
            char status[4];
            int result = logout_user(uid, pass);
            switch (result)
            {
            case 0:
                strcpy(status, "NOK");
                sprintf(reply, "%s %s", reply_code, status);
                break;
            case 1:
                strcpy(status, "OK");
                sprintf(reply, "%s %s", reply_code, status);
                break;
            case 2:
                strcpy(status, "UNR");
                sprintf(reply, "%s %s", reply_code, status);
                break;
            }
        }
    }
    else if (strcmp(code, "UNR") == 0) // Unregister
    {
        char uid[UID_SIZE], pass[PASS_SIZE];
        sscanf(buffer, "%*s %s %s", uid, pass);
        strcpy(reply_code, "RUR");
        if (verify_user_credentials(uid, pass))
        {
            char status[4];
            int result = unregister_user(uid, pass);
            switch (result)
            {
            case 0:
                strcpy(status, "NOK");
                sprintf(reply, "%s %s", reply_code, status);
                break;
            case 1:
                strcpy(status, "OK");
                sprintf(reply, "%s %s", reply_code, status);
                break;
            case 2:
                strcpy(status, "UNR");
                sprintf(reply, "%s %s", reply_code, status);
                break;
            }
        }
    }
    else if (strcmp(code, "LMA") == 0) // MyActions
    {
        char uid[UID_SIZE];
        sscanf(buffer, "%*s %s", uid);
        strcpy(reply_code, "RMA");
        if (verify_UID(uid))
        {
            char status[4];
            int result = myactions_user(uid); //! Aqui supostamente já tenho que passar os aid e os estados de cada um não
            // Não bro aqui não passas nada passo eu simplesmente no reply só
            switch (result)
            {
            case 0:
                strcpy(status, "NOK");
                sprintf(reply, "%s %s", reply_code, status);
            case 2:
                strcpy(status, "NLG");
                sprintf(reply, "%s %s", reply_code, status);
                break;
            case 1:
                char auctions_list[999] = get_auctions_list(uid);
                strcpy(status, "OK");
                sprintf(reply, "%s %s %s", reply_code, status, auctions_list); //* Falta tratar da lista de auctions
            }
        }
    }
    else if (strcmp(code, "LMB") == 0) // MyBids
    {
        char uid[UID_SIZE];
        sscanf(buffer, "%*s %s", uid);
        strcpy(reply_code, "RMB");
        if (verify_UID(uid))
        {
            char status[4];
            int result = mybids_user(uid); //! Aqui supostamente já tenho que passar os aid e os estados de cada um
            switch (result)
            {
            case 0:
                strcpy(status, "NOK");
                sprintf(reply, "%s %s", reply_code, status);
            case 2:
                strcpy(status, "NLG");
                sprintf(reply, "%s %s", reply_code, status);
                break;
            case 1:
                char bid_list[999] = get_bid_list(uid);
                strcpy(status, "OK");
                sprintf(reply, "%s %s %s", reply_code, status, bid_list); //* Falta tratar da lista de bids
            }
        }
    }
    else if (strcmp(code, "LST") == 0) // List
    {
        strcpy(reply_code, "RLS");
        char status[4];
        int result = list_all_auctions();
        switch (result)
        {
        case 0:
            strcpy(status, "NOK");
            sprintf(reply, "%s %s", reply_code, status);
        case 1:
            char auctions_list[999] = get_all_auctions();
            strcpy(status, "OK");
            sprintf(reply, "%s %s %s", reply_code, status, auctions_list); //* Falta tratar da lista de bids
        }
    }
    else if (strcmp(code, "SRC") == 0) // Show_record
    {
    }
    else
    {
        printf("Invalid handle input.\n");
    }

    // Send the Code Reply
    if (sendto(udp_fd, reply, strlen(reply), 0, (struct sockaddr *)&udp_addr, udp_addrlen) == -1)
    {
        perror("sendto");
        exit(1);
    }
}

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

            udp_message_handle(n, buffer);
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
