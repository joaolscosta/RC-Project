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

void udp_message_handle(ssize_t n, char buffer[])
{
    char code[4];
    char status[4];
    sscanf(buffer, "%s %s", code, status);
    if (strcmp(code, "RLI") == 0) // Resposta do Login
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
    else if (strcmp(code, "RLO") == 0) // Reposta de Logout
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
    else if (strcmp(code, "RUR") == 0) // Resposta de Unregister
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
    else if (strcmp(code, "RMA") == 0) // Resposta de MyActions
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
    else if (strcmp(code, "RMB") == 0) // Resposta de MyBids
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
            memset(buffer, 0, sizeof(buffer));
        }
        else
        {
            printf("Unknown status received: %s\n", status);
        }
    }
    else if (strcmp(code, "RRC") == 0) // Resposta de List
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

void udp_message(char buffer[])
{
    // printf("message to send->%s\n", buffer);

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
    // Clean buffer pq é reutilizado para receber e enviar o msm buffer
    memset(buffer, 0, sizeof(buffer));
    // Recebimento da resposta do servidor
    addrlen = sizeof(addr);
    n = recvfrom(fd, buffer, 8192, 0, (struct sockaddr *)&addr, &addrlen);
    if (n == -1)
    {
        perror("recvfrom");
        exit(EXIT_FAILURE);
    }

    udp_message_handle(n, buffer);

    memset(buffer, 0, sizeof(buffer));
    freeaddrinfo(res);
    close(fd);
}

void udp(char buffer[])
{
    char command[33];
    char reply[33];
    sscanf(buffer, "%s", command);

    if (strcmp(command, "login") == 0)
    {
        char uid[7], pass[9];
        sscanf(buffer, "login %s %s", uid, pass);
        sprintf(reply, "LIN %s %s", uid, pass);
        if (verify_user_credentials(uid, pass) == 1)
        {
            udp_message(reply);
        }
    }

    else if (strcmp(command, "logout") == 0)
    {
        char uid[7], pass[9];
        sscanf(buffer, "%*s %s %s", uid, pass);
        sprintf(reply, "LOU %s %s", uid, pass);
        if (verify_user_credentials(uid, pass))
        {
            udp_message(reply);
        }
    }
    else if (strcmp(command, "unregister") == 0)
    {
        char uid[7], pass[9];
        sscanf(buffer, "%*s %s %s", uid, pass);
        sprintf(reply, "UNR %s %s", uid, pass);
        if (verify_user_credentials(uid, pass))
        {
            udp_message(reply);
        }
    }
    else if ((strcmp(command, "myauctions") == 0) || (strcmp(command, "ma") == 0))
    {
        char uid[7];
        sscanf(buffer, "%*s %s", uid);
        sprintf(reply, "LMA %s", uid);
        if (verify_UID(uid))
        {
            udp_message(reply);
        }
    }
    else if ((strcmp(command, "mybids") == 0) || (strcmp(command, "mb") == 0))
    {
        char uid[7];
        sscanf(buffer, "%*s %s", uid);
        sprintf(reply, "LMB %s", uid);
        if (verify_UID(uid))
        {
            udp_message(reply);
        }
    }
    else if ((strcmp(command, "list") == 0) || (strcmp(command, "l") == 0))
    {
        strcpy(reply, "LST");
        udp_message(reply);
    }
    else if ((strcmp(command, "show_record") == 0) || (strcmp(command, "sr") == 0))
    {
        char aid[7];
        sscanf(buffer, "%*s %s", aid);
        sprintf(reply, "SRC %s", aid);
        if (verify_AID(aid))
        {
            udp_message(reply);
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

void tcp_message_handle(ssize_t n, char buffer[])
{
    char message_received[30];
    char status[30];

    sscanf(buffer, "%s %s", message_received, status);

    if (strcmp(message_received, "ROA") == 0) // Resposta do open
    {
        if (strcmp(status, "OK") == 0)
        {
            printf("AID: A_FAZER\n");
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
            printf("ENVIAR FILE.\n");
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
    else if (strcmp(message_received, "RBD") == 0) // Resposta do Show_Asset
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

void tcp_message(char buffer[])
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(ASIP, ASport, &hints, &res) != 0)
    {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }

    if (connect(fd, res->ai_addr, res->ai_addrlen) == -1)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    if (write(fd, buffer, strlen(buffer)) == -1)
    {
        perror("write");
        exit(EXIT_FAILURE);
    }

    memset(buffer, 0, sizeof(buffer));

    ssize_t bytes_received = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_received == -1)
    {
        perror("read");
        exit(EXIT_FAILURE);
    }
    else if (bytes_received == 0)
    {
        printf("Connection closed by server.\n");
    }
    else
    {
        buffer[bytes_received] = '\0';
        tcp_message_handle(bytes_received, buffer);
    }

    freeaddrinfo(res);
    close(fd);
}

void tcp(char buffer[])
{

    char command[33];
    char reply[33];
    sscanf(buffer, "%s", command);

    if (strcmp(command, "open") == 0) // Open auction
    {
        char uid[7], pass[9], auction_name[33], file_name[33], file_data[8192]; //! cuidado com os tamanhos dos chars
        int start_value, time_active;
        ssize_t file_size;
        sscanf(buffer, "open %s %s %d %d", auction_name, file_name, start_value, time_active);
        sprintf(reply, "LIN %s %s %s %d %d %s %d %s", uid, pass, auction_name, start_value, time_active, file_name, file_size, file_data);

        tcp_message(buffer);
    }
    else if (strcmp(command, "close") == 0)
    {
        char uid[7], pass[9], aid[4];
        sscanf(buffer, "close %s", aid);
        sprintf(reply, "CLS %s %s %s", uid, pass, aid);

        tcp_message(buffer);
    }
    else if ((strcmp(command, "show_asset") == 0) || (strcmp(command, "sa") == 0))
    {
        char aid[4];
        sscanf(buffer, "show_asset %s", aid);
        sprintf(reply, "SAS %s", aid);

        tcp_message(buffer);
    }
    else if (strcmp(command, "bid") == 0)
    {
        char uid[7], pass[9], aid[4];
        int bid_value;
        sscanf(buffer, "bid %s %d", aid, bid_value);
        sprintf(reply, "BID %s %s %s", uid, pass, aid, bid_value);

        tcp_message(buffer);
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