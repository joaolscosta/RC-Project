#include "actions.h"

#define GROUP_PORT "58105"
#define DEFAULT_PORT "58011"

int udp_fd;
int tcp_fd;
ssize_t n;
socklen_t udp_addrlen;
struct addrinfo udp_hints, *udp_res;
struct sockaddr_in udp_addr;
char buffer[10106128];

char *ASport = GROUP_PORT; // Chanhge to GROUO PORT
int verbose = 0;

void udp_message_handle(ssize_t n, char buffer[])
{
    char *reply = (char *)malloc(9 * sizeof(char));
    char code[4];
    char reply_code[4];
    sscanf(buffer, "%s ", code);
    if (strcmp(code, "LIN") == 0) // Login
    {
        char uid[UID_SIZE], pass[PASS_SIZE];
        sscanf(buffer, "%*s %s %s", uid, pass);
        strcpy(reply_code, "RLI");
        if (verify_user_credentials(uid, pass))
        {
            char status[4];
            int result = login_user(uid, pass);
            switch (result)
            {
            case 0:
                strcpy(status, "NOK");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("Wrong credentials.\n");
                break;
            case 1:
                strcpy(status, "OK");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("User logged in.\n");
                break;
            case 2:
                strcpy(status, "REG");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("User not registered.\n");
                break;
            }
        }
        else
        {
            strcpy(reply_code, "ERR");
            sprintf(reply, "%s\n", reply_code);
            if (verbose)
                printf("Invalid credentials.\n");
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
            int result = logout_user(uid);
            switch (result)
            {
            case 0:
                strcpy(status, "NOK");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("User not logged in.\n");
                break;
            case 1:
                strcpy(status, "OK");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("User logged out.\n");
                break;
            case 2:
                strcpy(status, "UNR");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("User not registered.\n");
                break;
            }
        }
        else
        {
            strcpy(reply_code, "ERR");
            sprintf(reply, "%s\n", reply_code);
            if (verbose)
                printf("Invalid credentials.\n");
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
            int result = unregister_user(uid);
            switch (result)
            {
            case 0:
                strcpy(status, "NOK");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("User not logged in.\n");
                break;
            case 1:
                strcpy(status, "OK");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("User unregistered.\n");
                break;
            case 2:
                strcpy(status, "UNR");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("User not registered.\n");
                break;
            }
        }
        else
        {
            strcpy(reply_code, "ERR");
            sprintf(reply, "%s\n", reply_code);
            if (verbose)
                printf("Invalid credentials.\n");
        }
    }
    else if (strcmp(code, "LMA") == 0) // MyActions
    {
        char uid[UID_SIZE];
        sscanf(buffer, "%*s %s", uid);
        strcpy(reply_code, "RMA");
        if (verify_UID(uid))
        {
            AUCTIONLIST list;
            char status[4];
            int result = myauctions_user(uid, &list);
            switch (result)
            {
            case 0:
                strcpy(status, "NOK");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("User has no ongoing auctions.\n");
                break;
            case 2:
                strcpy(status, "NLG");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("User not logged in.\n");
                break;
            case 1:
            {
                char *response = (char *)malloc((list.no_aucs * 6 + 1) * sizeof(char));
                DisplayAuctions(&list, response);
                printf("response: %s\n", response);
                strcpy(status, "OK");
                reply = realloc(reply, (strlen(reply_code) + strlen(status) + strlen(response) + 3) * sizeof(char));
                sprintf(reply, "%s %s%s\n", reply_code, status, response);
                free(response);
                if (verbose)
                    printf("User has %d auctions.\n", list.no_aucs);
                break;
            }
            }
        }
        else
        {
            strcpy(reply_code, "ERR");
            sprintf(reply, "%s\n", reply_code);
            if (verbose)
                printf("Invalid credentials.\n");
        }
    }
    else if (strcmp(code, "LMB") == 0) // MyBids
    {
        char uid[UID_SIZE];
        sscanf(buffer, "%*s %s", uid);
        strcpy(reply_code, "RMB");
        if (verify_UID(uid))
        {
            AUCTIONLIST list;
            char status[4];
            int result = mybids_user(uid, &list);
            switch (result)
            {
            case 0:
                strcpy(status, "NOK");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("User has no ongoing bids.\n");
                break;
            case 2:
                strcpy(status, "NLG");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("User not logged in.\n");
                break;
            case 1:
            {
                char *response = (char *)malloc((list.no_aucs * 6 + 1) * sizeof(char));
                DisplayAuctions(&list, response);
                strcpy(status, "OK");
                reply = realloc(reply, (strlen(reply_code) + strlen(status) + strlen(response) + 3) * sizeof(char));
                sprintf(reply, "%s %s%s\n", reply_code, status, response);
                free(response);
                if (verbose)
                    printf("User has %d bids.\n", list.no_aucs);
                break;
            }
            }
        }
        else
        {
            strcpy(reply_code, "ERR");
            sprintf(reply, "%s\n", reply_code);
            if (verbose)
                printf("Invalid credentials.\n");
        }
    }
    else if (strcmp(code, "LST") == 0) // List
    {
        strcpy(reply_code, "RLS");
        char status[4];
        AUCTIONLIST list;
        int result = list_all_auctions(&list);
        switch (result)
        {
        case 0:
            strcpy(status, "NOK");
            sprintf(reply, "%s %s\n", reply_code, status);
            if (verbose)
                printf("No auctions started.\n");
            break;
        case 1:
        {
            char *response = (char *)malloc((list.no_aucs * 6 + 1) * sizeof(char));
            DisplayAuctions(&list, response);
            strcpy(status, "OK");
            reply = realloc(reply, (strlen(reply_code) + strlen(status) + strlen(response) + 3) * sizeof(char));
            sprintf(reply, "%s %s%s\n", reply_code, status, response);
            free(response);
            if (verbose)
                printf("There are %d auctions.\n", list.no_aucs);
            break;
        }
        }
    }
    else if (strcmp(code, "SRC") == 0) // Show_record
    {
        char aid_s[AID_SIZE];
        sscanf(buffer, "%*s %s", aid_s);
        strcpy(reply_code, "RRC");
        if (verify_AID(aid_s))
        {
            int aid = atoi(aid_s);
            AUCTIONINFO info;
            BIDLIST list;
            char status[4];
            int result = show_record_user(aid, &info, &list);
            switch (result)
            {
            case 0:
                strcpy(status, "NOK");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("Auction not found.\n");
                break;
            case 1:
            {
                // TODO Dar display aos uids com 0S atras para preencher o seu tamanho de 6
                // Acho eu q é preciso not sure
                strcpy(status, "OK");
                char *response = DisplayRecord(&info, &list);
                // printf("response: %s\n", response);
                reply = realloc(reply, (strlen(reply_code) + strlen(status) + strlen(response) + 4) * sizeof(char));
                sprintf(reply, "%s %s ", reply_code, status);
                strcat(reply, response);
                free(response);
                if (verbose)
                    printf("Auction found.\n");
                break;
            }
            }
        }
        else
        {
            strcpy(reply_code, "ERR");
            sprintf(reply, "%s\n", reply_code);
            if (verbose)
                printf("Invalid credentials.\n");
        }
    }
    else
    {
        strcpy(reply_code, "ERR");
        sprintf(reply, "%s\n", reply_code);
        if (verbose)
            printf("Invalid Code Recived.\n");
    }

    // Send the Code Reply
    if (sendto(udp_fd, reply, strlen(reply), 0, (struct sockaddr *)&udp_addr, udp_addrlen) == -1)
    {
        free(reply);
        perror("sendto");
        exit(1);
    }
    free(reply);
}

void tcp_message_handle(char buffer[], int tcp_socket)
{
    char *reply = (char *)malloc(9);
    char code[4];
    char reply_code[4];
    sscanf(buffer, "%s ", code);
    if (strcmp(code, "OPA") == 0) // Open
    {
        char uid[UID_SIZE], pass[PASS_SIZE];
        char auction_name[AUCTION_NAME_SIZE], start_value[7], time_active[6];
        char file_name[FILE_NAME_SIZE], file_size[9];
        sscanf(buffer, "%*s %s %s %s %s %s %s %s", uid, pass, auction_name, start_value, time_active, file_name, file_size);
        strcpy(reply_code, "ROA");
        if (check_open_credentials(auction_name, file_name, start_value, time_active) && verify_user_credentials(uid, pass))
        {
            char *file_data = (char *)malloc((atoi(file_size) + 1) * sizeof(char));
            char *file_data_start = buffer;
            for (int i = 0; i < 8; i++)
            {
                file_data_start = strchr(file_data_start, ' ');
                if (file_data_start == NULL)
                {
                    // handle error
                }
                file_data_start++; // skip past the space
            }
            size_t file_data_size = buffer + n - file_data_start;
            memcpy(file_data, file_data_start, file_data_size);
            char status[4];
            AUCTIONINFO info;
            FILEINFO file;
            info.start_value = atoi(start_value);
            info.timeactive = atoi(time_active);
            file.file_size = atoi(file_size);
            strcpy(info.uid, uid);
            strcpy(info.asset_fname, file_name);
            strcpy(file.file_name, file_name);
            strcpy(info.name, auction_name);
            int aid;
            int result = open_auction(info, file, file_data, &aid);
            switch (result)
            {
            case 0:
                strcpy(status, "NOK");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("Auction could not be started.\n");
                break;
            case 1:
                strcpy(status, "OK");
                sprintf(reply, "%s %s %03d\n", reply_code, status, aid);
                if (verbose)
                    printf("Auction started.\n");
                break;
            case 2:
                strcpy(status, "NLG");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("User not logged in.\n");
                break;
            }
        }
        else
        {
            strcpy(reply_code, "ERR");
            sprintf(reply, "%s\n", reply_code);
            if (verbose)
                printf("Invalid credentials.\n");
        }
    }
    else if (strcmp(code, "CLS") == 0) // Close
    {
        char uid[UID_SIZE], pass[PASS_SIZE], aid_s[AID_SIZE];
        sscanf(buffer, "%*s %s %s %s", uid, pass, aid_s);
        strcpy(reply_code, "RCL");
        if (verify_user_credentials(uid, pass) && verify_AID(aid_s))
        {
            char status[4];
            int aid = atoi(aid_s);
            USERINFO info;
            strcpy(info.uid, uid);
            strcpy(info.password, pass);
            int result = close_auction(info, aid);
            switch (result)
            {
            case 0:
                strcpy(status, "NLG");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("User not logged in.\n");
                break;
            case 1:
                strcpy(status, "EAU");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("Auction does not exist.\n");
                break;
            case 2:
                strcpy(status, "EOW");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("Auction owned by User.\n");
                break;
            case 3:
                strcpy(status, "END");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("Auction has already ended.\n");
                break;
            case 4:
                strcpy(status, "OK");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("Auction closed.\n");
                break;
            }
        }
        else
        {
            strcpy(reply_code, "ERR");
            sprintf(reply, "%s\n", reply_code);
            if (verbose)
                printf("Invalid credentials.\n");
        }
    }
    else if (strcmp(code, "SAS") == 0) // Show_asset
    {
        char aid_s[AID_SIZE];
        sscanf(buffer, "%*s %s", aid_s);
        strcpy(reply_code, "RSA");
        if (verify_AID(aid_s))
        {
            // Read File funciton
            FILEINFO info;
            char *file_data = NULL;
            char status[4];
            int aid = atoi(aid_s);
            int result = show_asset(aid, &info, &file_data);
            switch (result)
            {
            case 0:
                strcpy(status, "NOK");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("There was a problem with the file.\n");
                break;
            case 1:
                strcpy(status, "OK");
                reply = realloc(reply, (strlen(reply_code) + strlen(status) + strlen(info.file_name) + 8 + info.file_size + 6) * sizeof(char)); // 8- File size max
                sprintf(reply, "%s %s %s %d %s\n", reply_code, status, info.file_name, info.file_size, file_data);
                if (verbose)
                    printf("File sent.\n");
                break;
            }
            free(file_data);
        }
        else
        {
            strcpy(reply_code, "ERR");
            sprintf(reply, "%s\n", reply_code);
            if (verbose)
                printf("Invalid credentials.\n");
        }
    }
    else if (strcmp(code, "BID") == 0) // Bid
    {
        char uid[UID_SIZE], pass[PASS_SIZE], aid_s[AID_SIZE], bid_value_s[BID_VALUE_SIZE];
        sscanf(buffer, "%*s %s %s %s %s ", uid, pass, aid_s, bid_value_s);
        strcpy(reply_code, "RBD");
        if (verify_user_credentials(uid, pass) && verify_AID(aid_s) && verify_bid_value(bid_value_s))
        {
            char status[4];
            USERINFO info;
            strcpy(info.uid, uid);
            strcpy(info.password, pass);
            int aid = atoi(aid_s);
            int bid_value = atoi(bid_value_s);
            int result = bid(info, aid, bid_value);
            switch (result)
            {
            case 0:
                strcpy(status, "NLG");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("User not logged in.\n");
                break;
            case 1:
                strcpy(status, "NOK");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("Auction not active.\n");
                break;
            case 2:
                strcpy(status, "ILG");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("Auction hosted by user.\n");
                break;
            case 3:
                strcpy(status, "REF");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("Bid value is lower than the current bid.\n");
                break;
            case 4:
                strcpy(status, "ACC");
                sprintf(reply, "%s %s\n", reply_code, status);
                if (verbose)
                    printf("Bid accepted.\n");
                break;
            case 5:
                strcpy(reply_code, "ERR");
                sprintf(reply, "%s\n", reply_code);
                if (verbose)
                    printf("Invalid credentials.\n");
            }
        }
        else
        {
            strcpy(reply_code, "ERR");
            sprintf(reply, "%s\n", reply_code);
            if (verbose)
                printf("Invalid credentials.\n");
        }
    }
    else
    {
        strcpy(reply_code, "ERR");
        sprintf(reply, "%s\n", reply_code);
        if (verbose)
            printf("Invalid Code Received.\n");
    }

    /* Envia a mensagem recebida (atualmente presente no buffer) para a socket */
    n = write(tcp_socket, reply, strlen(reply));
    if (n == -1)
    {
        free(reply);
        perror("write");
        exit(1);
    }
    free(reply);
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

            n = read(tcp_socket, buffer, sizeof(buffer));
            if (n == -1)
                exit(1);

            // Este n ta mt scuffed
            tcp_message_handle(buffer, tcp_socket);

            close(tcp_socket);
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
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char const *argv[])
{
    server_arguments(argc, argv);
    server();
    return 0;
}
