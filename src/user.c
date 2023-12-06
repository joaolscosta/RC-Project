#include "actions.h"

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

char tcp_input[][11] = {"open", "close", "show_asset", "bid"};

char *ASIP = IP;
char *ASport = DEFAULT_PORT;

void send_udp_message(char buffer[])
{
    // printf("message to send->%s", buffer);

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
    n = sendto(fd, buffer, 20, 0, res->ai_addr, res->ai_addrlen);
    if (n == -1)
    {
        exit(1);
    }
}

void message_handle(ssize_t n, char buffer[], char message_sent[])
{
    char message_received[30];
    char status[30]; // Variável para armazenar o status

    sscanf(buffer, "%s %s", message_received, status);

    printf("%s %s\n", message_received, status);

    if (strcmp(message_received, "RLI") == 0) // Resposta do Login
    {
        if (strcmp(status, "OK") == 0)
        {
            //! NECESSÁRIO VERIFICAR SE JÁ DEU LOGIN E SE SIM MENSAGEM DIFERENTE?
            printf("User logged in successfully.\n");
        }
        else if (strcmp(status, "NOK") == 0)
        {
            printf("Credentials invalid.\n");
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
    else if (strcmp(message_received, "RLO") == 0) // Reposta de Logout
    {
        if (strcmp(status, "OK") == 0)
        {
            printf("User loggout successfully.\n");
        }
        else if (strcmp(status, "NOK") == 0)
        {
            printf("Impossible to loggout user.\n");
        }
        else
        {
            printf("Unknown status received: %s\n", status);
        }
    }
    else if (strcmp(message_received, "RUR") == 0) // Resposta de Unregister
    {
        if (strcmp(status, "OK") == 0)
        {
            printf("User unregisted successfully.\n");
        }
        else if (strcmp(status, "NOK") == 0)
        {
            printf("Can't unregisterer user.\n");
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
    else if (strcmp(message_received, "RMA") == 0) // Resposta de MyActions
    {
        if (strcmp(status, "NOK") == 0)
        {
            printf("User has no ongoing actions.\n");
        }
        else if (strcmp(status, "OK") == 0)
        {
            printf("User Actions:\n");
            write(1, buffer, n);
            memset(buffer, 0, sizeof(buffer));
        }
        else
        {
            printf("Unknown status received: %s\n", status);
        }
    }
    else if (strcmp(message_received, "RMB") == 0) // Resposta de MyBids
    {
        if (strcmp(status, "NOK") == 0)
        {
            printf("User has no ongoing bids.\n");
        }
        else if (strcmp(status, "OK") == 0)
        {
            printf("User Bids:\n");
            write(1, buffer, n);
            memset(buffer, 0, sizeof(buffer));
        }
        else
        {
            printf("Unknown status received: %s\n", status);
        }
    }
    else if (strcmp(message_received, "RLS") == 0) // Resposta de List
    {
        if (strcmp(status, "NOK") == 0)
        {
            printf("No action was yet started.\n");
        }
        else if (strcmp(status, "OK") == 0)
        {
            printf("All actions list:\n");
            write(1, buffer, n);
            memset(buffer, 0, sizeof(buffer));
        }
        else
        {
            printf("Unknown status received: %s\n", status);
        }
    }
    else if (strcmp(message_received, "RRC") == 0) // Resposta de List
    {
        if (strcmp(status, "NOK") == 0)
        {
            printf("AID does not exist.\n");
        }
        else if (strcmp(status, "OK") == 0)
        {
            printf("Record:\n");
            write(1, buffer, n);
            memset(buffer, 0, sizeof(buffer));
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

void received_udp_message(char buffer[])
{
    char message_sent[100];
    strcpy(message_sent, buffer);

    addrlen = sizeof(addr);

    memset(buffer, 0, sizeof(buffer));
    n = recvfrom(fd, buffer, 8192, 0, (struct sockaddr *)&addr, &addrlen);
    if (n == -1)
    {
        exit(1);
    }

    message_handle(n, buffer, message_sent);

    memset(buffer, 0, sizeof(buffer));
    freeaddrinfo(res);
    close(fd);
}

void udp(char buffer[])
{

    char command[33];
    sscanf(buffer, "%s", command);

    if (strcmp(command, "login") == 0)
    {
        strcpy(buffer, "LIN");
        strcat(buffer, buffer + strlen(command));
        if (login_user(buffer))
        {
            send_udp_message(buffer);
            received_udp_message(buffer);
        }
    }

    else if (strcmp(command, "logout") == 0)
    {
        strcpy(buffer, "LOU");
        strcat(buffer, buffer + strlen(command));
        if (logout_user(buffer))
        {
            send_udp_message(buffer);
            received_udp_message(buffer);
        }
    }
    else if (strcmp(command, "unregister") == 0)
    {
        strcpy(buffer, "UNR");
        strcat(buffer, buffer + strlen(command));
        if (unregister_user(buffer))
        {
            send_udp_message(buffer);
            received_udp_message(buffer);
        }
    }
    else if ((strcmp(command, "myauctions") == 0) || (strcmp(command, "ma") == 0))
    {
        strcpy(buffer, "LMA");
        strcat(buffer, buffer + strlen(command));
        if (myactions_user(buffer) == 1)
        {
            send_udp_message(buffer);
            received_udp_message(buffer);
        }
    }
    else if ((strcmp(command, "mybids") == 0) || (strcmp(command, "mb") == 0))
    {
        strcpy(buffer, "LMB");
        strcat(buffer, buffer + strlen(command));
        if (mybids_user(buffer) == 1)
        {
            send_udp_message(buffer);
            received_udp_message(buffer);
        }
    }
    else if ((strcmp(command, "list") == 0) || (strcmp(command, "l") == 0))
    {
        strcpy(buffer, "LST");
        strcat(buffer, buffer + strlen(command));
        if (list_user(buffer) == 1)
        {
            send_udp_message(buffer);
            received_udp_message(buffer);
        }
    }
    else if ((strcmp(command, "show_record") == 0) || (strcmp(command, "sr") == 0))
    {
        strcpy(buffer, "SRC");
        strcat(buffer, buffer + strlen(command));
        if (show_record_user(buffer) == 1)
        {
            send_udp_message(buffer);
            received_udp_message(buffer);
        }
    }
    else if (strcmp(command, "exit") == 0)
    {
        exit(1);
    }
    else
    {
        perror("invalid input");
        exit(EXIT_FAILURE);
    }
    memset(buffer, 0, sizeof(buffer));
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

void tcp(char buffer[])
{

    char command[33];
    sscanf(buffer, "%s", command);

    if (strcmp(command, "open") == 0)
    {
        strcpy(buffer, "OPA");
        strcat(buffer, buffer + strlen(command));
    }
    else if (strcmp(command, "close") == 0)
    {
        strcpy(buffer, "CLS");
        strcat(buffer, buffer + strlen(command));
    }
    else if (strcmp(command, "show_asset") == 0)
    {
        strcpy(buffer, "SAS");
        strcat(buffer, buffer + strlen(command));
    }
    else if (strcmp(command, "bid") == 0)
    {
        strcpy(buffer, "BID");
        strcat(buffer, buffer + strlen(command));
    }
    else
    {
        perror("invalid input");
        exit(EXIT_FAILURE);
    }
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
                tcp(buffer);
            }
            else
            {
                udp(buffer);
            }
        }
    }
    return 0;
}