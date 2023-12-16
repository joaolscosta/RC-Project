#include "actions.h"
#include <sys/sendfile.h>

#define GROUP_PORT "58105"
#define DEFAULT_PORT "58011"
#define LOCAL_HOST "localhost" // CASO DEFAULT DPS DO TEJO
// #define IP "193.136.138.142"
#define IP "tejo.tecnico.ulisboa.pt"

int fd;
ssize_t n;
socklen_t addrlen; // Tamanho do endereço
/*
hints - Estrutura que contém informações sobre o tipo de conexão que será estabelecida.
        Podem-se considerar, literalmente, dicas para o sistema operacional sobre como
        deve ser feita a conexão, de forma a facilitar a aquisição ou preencher dados.

res - Localização onde a função getaddrinfo() armazenará informações sobre o endereço.
*/
struct addrinfo hints, *res;
struct sockaddr_in addr;
char buffer[128]; // buffer para onde serão escritos os dados recebidos do servidor

char tcp_input[][11] = {"open", "close", "show_asset", "sa", "bid", "b"};

char *ASIP = IP;
char *ASport = DEFAULT_PORT;

char current_login_uid[7];
char current_login_pass[9];

void udp_message_handle(ssize_t n, char buffer[], size_t buffer_size)
{
    char code[4];
    char status[4];

    sscanf(buffer, "%s %s", code, status);

    if (strcmp(code, "RLI") == 0) // Resposta do Login
    {
        if (strcmp(status, "OK") == 0)
        {
            printf("Successful login.\n");
        }
        else if (strcmp(status, "NOK") == 0)
        {
            printf("Incorrect login attempt. Credentials may be invalid.\n");
        }
        else if (strcmp(status, "REG") == 0)
        {
            printf("New user registered.\n");
        }
        else
        {
            printf("Unknown status received: %s\n", status);
        }
    }
    else if (strcmp(code, "RLO") == 0) // Reposta de Logout
    {

        if (strcmp(status, "OK") == 0)
        {
            strcpy(current_login_uid, "no");
            strcpy(current_login_pass, "no");
            printf("Successfull logout.\n");
        }
        else if (strcmp(status, "NOK") == 0)
        {
            printf("Can't logout user. User not logged in.\n");
        }
        else if (strcmp(status, "UNR") == 0)
        {
            printf("User not registered.\n");
        }
        else
        {
            printf("Unknown status received: %s\n", status);
        }
    }
    else if (strcmp(code, "RUR") == 0) // Resposta de Unregister
    {
        if (strcmp(status, "OK") == 0)
        {
            strcpy(current_login_uid, "no");
            strcpy(current_login_uid, "no");
            printf("Successful unregister.\n");
        }
        else if (strcmp(status, "NOK") == 0)
        {
            printf("Can't unregisterer user. User not logged in.\n");
        }
        else if (strcmp(status, "UNR") == 0)
        {
            printf("User is not registered.\n");
        }
        else
        {
            printf("Unknown status received: %s\n", status);
        }
    }
    else if (strcmp(code, "RMA") == 0) // Resposta de MyActions
    {
        if (strcmp(status, "NOK") == 0)
        {
            printf("User has no ongoing actions.\n");
        }
        else if (strcmp(status, "OK") == 0)
        {
            // char list[8192];
            // sscanf(buffer, "%*s %*s %[^\n]", list);
            // printf("%s\n", list);
            printf("%s", buffer);
            memset(buffer, 0, buffer_size);
        }
        else if (strcmp(status, "NLG") == 0)
        {
            printf("User not logged in.\n");
        }
        else
        {
            printf("Unknown status received: %s\n", status);
        }
    }
    else if (strcmp(code, "RMB") == 0) // Resposta de MyBids
    {
        if (strcmp(status, "NOK") == 0)
        {
            printf("User has no ongoing bids.\n");
        }
        else if (strcmp(status, "OK") == 0)
        {
            write(1, buffer, n);
            memset(buffer, 0, buffer_size);
        }
        else if (strcmp(status, "NLG") == 0)
        {
            printf("User not logged in.\n");
        }
        else
        {
            printf("Unknown status received: %s\n", status);
        }
    }
    else if (strcmp(code, "RLS") == 0) // Resposta de List
    {
        if (strcmp(status, "NOK") == 0)
        {
            printf("No action was yet started.\n");
        }
        else if (strcmp(status, "OK") == 0)
        {
            printf("All actions list:\n");
            write(1, buffer, n);
            memset(buffer, 0, buffer_size);
        }
        else
        {
            printf("Unknown status received: %s\n", status);
        }
    }
    else if (strcmp(code, "RRC") == 0) // Resposta de Record
    {
        if (strcmp(status, "NOK") == 0)
        {
            printf("AID does not exist.\n");
        }
        else if (strcmp(status, "OK") == 0)
        {
            printf("Record:\n");
            write(1, buffer, n);
            memset(buffer, 0, buffer_size);
        }
        else
        {
            printf("Unknown status received: %s\n", status);
        }
    }
    else
    {
        printf("Invalid handle input.\n");
    }
}

void udp_message(char buffer[], size_t buffer_size)
{

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (fd == -1)
    {
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket

    if (getaddrinfo(ASIP, ASport, &hints, &res) != 0)
    {
        printf("error in getaddrinfo\n");
        exit(1);
    }

    n = sendto(fd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1)
    {
        perror("sendto");
        exit(EXIT_FAILURE);
    }

    memset(buffer, 0, buffer_size);
    addrlen = sizeof(addr);
    char recv_buffer[8192];
    n = recvfrom(fd, recv_buffer, 8192, 0, (struct sockaddr *)&addr, &addrlen);
    if (n == -1)
    {
        perror("recvfrom");
        exit(EXIT_FAILURE);
    }

    size_t receive_buffer_size = sizeof(recv_buffer) / sizeof(recv_buffer[0]);
    udp_message_handle(n, recv_buffer, receive_buffer_size);

    memset(recv_buffer, 0, receive_buffer_size);
    freeaddrinfo(res);
    close(fd);
}

void udp(char buffer[], size_t size)
{
    char command[33];
    char reply[33];
    sscanf(buffer, "%s", command);

    if (strcmp(command, "login") == 0)
    {
        char uid[7], pass[9];
        sscanf(buffer, "login %s %s", uid, pass);
        sprintf(reply, "LIN %s %s\n", uid, pass);
        if (verify_user_credentials(uid, pass) == 1)
        {
            strcpy(current_login_uid, uid);
            strcpy(current_login_pass, pass);
            size_t buffer_size = sizeof(reply) / sizeof(reply[0]);
            udp_message(reply, buffer_size);
        }
    }
    else if (strcmp(command, "logout") == 0)
    {
        if (strcmp("no", current_login_uid) == 0)
        {
            printf("User not logged in.\n");
        }
        else
        {
            sprintf(reply, "LOU %s %s\n", current_login_uid, current_login_pass);
            size_t buffer_size = sizeof(reply) / sizeof(reply[0]);
            udp_message(reply, buffer_size);
        }
    }
    else if (strcmp(command, "unregister") == 0)
    {
        if (strcmp("no", current_login_uid) == 0)
        {
            printf("User not logged in.\n");
        }
        else
        {
            sprintf(reply, "UNR %s %s\n", current_login_uid, current_login_pass);
            size_t buffer_size = sizeof(reply) / sizeof(reply[0]);
            udp_message(reply, buffer_size);
        }
    }
    else if ((strcmp(command, "myauctions") == 0) || (strcmp(command, "ma") == 0))
    {
        if (strcmp("no", current_login_uid) == 0)
        {
            printf("User not logged in.\n");
        }
        else
        {
            sprintf(reply, "LMA %s\n", current_login_uid);
            size_t buffer_size = sizeof(reply) / sizeof(reply[0]);
            udp_message(reply, buffer_size);
        }
    }
    else if ((strcmp(command, "mybids") == 0) || (strcmp(command, "mb") == 0))
    {
        if (strcmp("no", current_login_uid) == 0)
        {
            printf("User not logged in.\n");
        }
        else
        {
            sprintf(reply, "LMB %s\n", current_login_uid);
            size_t buffer_size = sizeof(reply) / sizeof(reply[0]);
            udp_message(reply, buffer_size);
        }
    }
    else if ((strcmp(command, "list") == 0) || (strcmp(command, "l") == 0))
    {
        sprintf(reply, "LST\n");
        size_t buffer_size = sizeof(reply) / sizeof(reply[0]);
        udp_message(reply, buffer_size);
    }
    else if ((strcmp(command, "show_record") == 0) || (strcmp(command, "sr") == 0))
    {
        char aid[10];
        sscanf(buffer, "%*s %s", aid);
        if (strlen(aid) != 3)
        {
            printf("Invalid input: aid size wrong.\n");
        }
        else
        {
            sprintf(reply, "SRC %s\n", aid);
            if (verify_AID(aid))
            {
                size_t buffer_size = sizeof(reply) / sizeof(reply[0]);
                udp_message(reply, buffer_size);
            }
        }
    }
    else if (strcmp(command, "exit") == 0)
    {
        if (strcmp("no", current_login_uid) == 0)
        {
            exit(0);
        }
        else
        {
            printf("User still logged in.\n");
        }
    }
    else
    {
        printf("Invalid input: %s\n", command);
    }
    memset(buffer, 0, size);
}

void tcp_message_handle(char buffer[], size_t buffer_size)
{
    char message_received[30];
    char status[30];

    sscanf(buffer, "%s %s", message_received, status);

    if (strcmp(message_received, "ROA") == 0) // Resposta do open
    {
        if (strcmp(status, "OK") == 0)
        {
            char new_aid[4];
            sscanf(buffer, "%*s %*s %s", new_aid);
            printf("Auction opened with AID: %s\n", new_aid);
            memset(buffer, 0, buffer_size);
        }
        else if (strcmp(status, "NOK") == 0)
        {
            printf("Action could not be started.\n");
        }
        else if (strcmp(status, "NLG") == 0)
        {
            printf("User not logged in.\n");
        }
        else
        {
            printf("Unknown status received: %s\n", status);
        }
    }
    else if (strcmp(message_received, "RCL") == 0) // Resposta do Close
    {
        if (strcmp(status, "OK") == 0)
        {
            printf("Auction sucessfully closed.\n");
        }
        else if (strcmp(status, "EAU") == 0)
        {
            printf("Auctions does not exist.\n");
        }
        else if (strcmp(status, "EOW") == 0)
        {
            printf("Auctions not owned by this user.\n");
        }
        else if (strcmp(status, "END") == 0)
        {
            printf("Auctions already finished.\n");
        }
        else if (strcmp(status, "NLG") == 0)
        {
            printf("User not logged in.\n");
        }
        else
        {
            printf("Unknown status received: %s\n", status);
        }
    }
    else if (strcmp(message_received, "RSA") == 0) // Resposta do Show_Asset
    {
        if (strcmp(status, "OK") == 0)
        {
            char file_name[33], file_data[8192], file_size[8192];

            sscanf(buffer, "%*s %*s %s %s %*s", file_name, file_size);

            int count_spaces = 0;
            int start_file_data_position = 0;
            for (size_t i = 0; i < strlen(buffer); i++)
            {
                if (buffer[i] == ' ')
                {
                    count_spaces++;
                }
                if (count_spaces == 4)
                {
                    start_file_data_position = i + 1;
                    break;
                }
            }

            int size = atoi(file_size);

            memcpy(file_data, buffer + start_file_data_position, size);

            file_data[size] = '\0';

            FILE *file = fopen(file_name, "w");
            if (file == NULL)
            {
                printf("Failed to open file: %s\n", file_name);
            }
            else
            {
                fprintf(file, "%s", file_data);

                printf("File %s received with size %s bytes.\n", file_name, file_size);

                fclose(file);
                memset(buffer, 0, buffer_size);
            }
        }
        else if (strcmp(status, "NOK") == 0)
        {
            printf("No file to be sent or other problem.\n");
        }
        else
        {
            printf("Unknown status received: %s\n", status);
        }
    }
    else if (strcmp(message_received, "RBD") == 0) // Resposta do Bid
    {
        if (strcmp(status, "NLG") == 0)
        {
            printf("User not logged in.\n");
        }
        else if (strcmp(status, "NOK") == 0)
        {
            printf("Auction is not active.\n");
        }
        else if (strcmp(status, "ACC") == 0)
        {
            printf("Auction ongoing was accepeted.\n");
        }
        else if (strcmp(status, "REF") == 0)
        {
            printf("Recused because there is already a bigger bid placed.\n");
        }
        else if (strcmp(status, "ILG") == 0)
        {
            printf("Recused for trying to bid in a auctions hosted by yourself.\n");
        }
        else
        {
            printf("Unknown status received: %s\n", status);
        }
    }
}

void tcp_message(char buffer[], size_t size)
{
    int fd, errcode;
    ssize_t n;
    struct addrinfo hints, *res;
    char receive_buffer[8192];

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM; // TCP socket

    errcode = getaddrinfo(ASIP, ASport, &hints, &res);
    if (errcode != 0)
    {
        exit(1);
    }
    n = connect(fd, res->ai_addr, res->ai_addrlen);
    if (n == -1)
    {
        exit(1);
    }

    n = write(fd, buffer, strlen(buffer));
    if (n == -1)
    {
        perror("write");
        exit(1);
    }

    char aux[8192];
    memset(aux, 0, sizeof(aux));
    while ((n = read(fd, aux, sizeof(aux) - 1)) > 0)
    {
        strcat(receive_buffer, aux);
    }

    printf("ola\n");
    memset(buffer, 0, size);

    size_t buffer_size = sizeof(receive_buffer) / sizeof(receive_buffer[0]);
    tcp_message_handle(receive_buffer, buffer_size);

    memset(receive_buffer, 0, sizeof(receive_buffer));

    freeaddrinfo(res);
    close(fd);
}

void tcp(char buffer[], size_t size)
{

    char command[33];
    char reply[50];
    sscanf(buffer, "%s", command);
    if (strcmp(command, "open") == 0) // Open auction
    {
        if (strcmp("no", current_login_uid) == 0)
        {
            printf("User not logged in.\n");
        }
        else
        {
            char auction_name[33], file_name[33];
            char start_value[8192], time_active[8192];
            long int file_size;
            int file_data;
            sscanf(buffer, "open %s %s %s %s", auction_name, file_name, start_value, time_active);
            if (check_open_credentials(auction_name, file_name, start_value, time_active))
            {
                int in_fd = open(file_name, O_RDONLY);
                if (in_fd == -1)
                {
                    perror("open");
                    return;
                }

                struct stat stat_buf;
                if (fstat(in_fd, &stat_buf) == -1)
                {
                    perror("fstat");
                    return;
                }
                char *file_data = malloc(stat_buf.st_size);
                if (file_data == NULL)
                {
                    perror("malloc");
                    return;
                }

                ssize_t read_bytes = read(in_fd, file_data, stat_buf.st_size);
                if (read_bytes == -1)
                {
                    perror("read");
                    return;
                }
                file_data[stat_buf.st_size] = '\0';

                close(in_fd);

                file_size = get_file_size(file_name);
                sprintf(reply, "OPA %s %s %s %s %s %s %ld %s\n", current_login_uid, current_login_pass, auction_name, start_value, time_active, file_name, file_size, file_data);
                size_t buffer_size = sizeof(reply) / sizeof(reply[0]);

                tcp_message(reply, buffer_size);

                free(file_data);
            }
        }
    }
    else if (strcmp(command, "close") == 0)
    {
        if (strcmp("no", current_login_uid) == 0)
        {
            printf("User not logged in.\n");
        }
        else
        {
            char aid[4];
            sscanf(buffer, "close %s", aid);
            sprintf(reply, "CLS %s %s %s\n", current_login_uid, current_login_pass, aid);
            size_t buffer_size = sizeof(reply) / sizeof(reply[0]);
            tcp_message(reply, buffer_size);
        }
    }
    else if ((strcmp(command, "show_asset") == 0) || (strcmp(command, "sa") == 0))
    {
        char aid[4];
        sscanf(buffer, "%*s %s", aid);
        if (strlen(aid) != 3)
        {
            printf("Invalid input: aid size wrong.\n");
        }
        else
        {
            sprintf(reply, "SAS %s\n", aid);
            if (verify_AID(aid))
            {
                size_t buffer_size = sizeof(reply) / sizeof(reply[0]);
                tcp_message(reply, buffer_size);
            }
        }
    }
    else if ((strcmp(command, "bid") == 0) || (strcmp(command, "b") == 0))
    {
        if (strcmp("no", current_login_uid) == 0)
        {
            printf("User not logged in.\n");
        }
        else
        {
            char aid[4];
            char bid_value[10];
            sscanf(buffer, "%*s %s %s", aid, bid_value);
            sprintf(reply, "BID %s %s %s %s\n", current_login_uid, current_login_pass, aid, bid_value);
            size_t buffer_size = sizeof(reply) / sizeof(reply[0]);
            tcp_message(reply, buffer_size);
        }
    }
    else
    {
        perror("invalid input");
        exit(EXIT_FAILURE);
    }
}

int check_if_tcp(char buffer[])
{
    size_t buffer_len = strlen(buffer);

    for (size_t i = 0; i < sizeof(tcp_input) / sizeof(tcp_input[0]); ++i)
    {
        size_t input_len = strlen(tcp_input[i]);

        // Verifica se o tamanho da palavra no buffer é igual ao tamanho do input
        if (buffer_len >= input_len && strncmp(buffer, tcp_input[i], input_len) == 0)
        {
            return 1;
        }
    }

    return 0;
}

void user_arguments(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "n:p:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            ASIP = optarg;
            break;
        case 'p':
            // Probbly cant convert immediatly may cause errors pass through string and use validate_port function
            ASport = optarg;
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
    user_arguments(argc, argv);

    fd_set inputs, newfds;
    int max_fd = STDIN_FILENO;

    strcpy(current_login_uid, "no");
    strcpy(current_login_pass, "no");

    while (1)
    {

        FD_ZERO(&inputs);
        FD_SET(max_fd, &inputs);
        newfds = inputs;

        if (select(max_fd + 1, &newfds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(STDIN_FILENO, &newfds))
        {
            char buffer[8192];
            if (fgets(buffer, sizeof(buffer), stdin) == NULL)
            {
                perror("fgets");
                exit(EXIT_FAILURE);
            }

            if (check_if_tcp(buffer))
            {
                tcp(buffer, sizeof(buffer));
            }
            else
            {
                udp(buffer, sizeof(buffer));
            }
        }
    }

    return 0;
}