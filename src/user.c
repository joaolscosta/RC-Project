#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "actions.h"

#define PORT "58011"
//#define IP "193.136.138.142"
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

void send_message(char buffer[]){
    printf("message to send->%s", buffer);
    /* Cria um socket UDP (SOCK_DGRAM) para IPv4 (AF_INET).
    É devolvido um descritor de ficheiro (fd) para onde se deve comunicar. */
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        exit(1);
    }

    /* Preenche a estrutura com 0s e depois atribui a informação já conhecida da ligação */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket

    /* Busca informação do host "tejo.tecnico.ulisboa.pt", na porta especificada,
    guardando a informação nas `hints` e na `res`. Caso o host seja um nome
    e não um endereço IP (como é o caso), efetua um DNS Lookup. */
    if (getaddrinfo(IP, PORT, &hints, &res) != 0) {
        printf("error in getaddrinfo");
        exit(1);
    }

    /* Envia para o `fd` (socket) a mensagem "Hello!\n" com o tamanho 20.
       Não são passadas flags (0), e é passado o endereço de destino.
       É apenas aqui criada a ligação ao servidor. */
    n = sendto(fd, buffer, 20, 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) {
        exit(1);
    }

    /* Recebe 128 Bytes do servidor e guarda-os no buffer.
       As variáveis `addr` e `addrlen` não são usadas pois não foram inicializadas. */
    addrlen = sizeof(addr);
    n = recvfrom(fd, buffer, 128, 0, (struct sockaddr *)&addr, &addrlen);
    if (n == -1) {
        exit(1);
    }

    /* Imprime a mensagem "echo" e o conteúdo do buffer (ou seja, o que foi recebido
    do servidor) para o STDOUT (fd = 1) */
    write(1, "echo: ", 6);
    write(1, buffer, n);

    /* Desaloca a memória da estrutura `res` e fecha o socket */
    freeaddrinfo(res);
    close(fd);
}

void udp(char buffer[]) {

    char command[33];  
    sscanf(buffer, "%s", command);
        
    if(strcmp(command, "login") == 0){ 
        strcpy(buffer, "LIN");
        strcat(buffer, buffer + strlen(command));
        if(login_user(buffer) == 1){
            send_message(buffer);
        }
    } else if(strcmp(command, "logout") == 0){ 
        strcpy(buffer, "LOU");
        strcat(buffer, buffer + strlen(command));
        if(logout_user(buffer) == 1){
            send_message(buffer);
        }
    } else if(strcmp(command, "unregister") == 0){ 
        strcpy(buffer, "UNR");
        strcat(buffer, buffer + strlen(command));
        if(unregister_user(buffer) == 1){
            send_message(buffer);
        }
    } else if(strcmp(command, "myactions") == 0){ 
        strcpy(buffer, "LMA");
        strcat(buffer, buffer + strlen(command));
        if(myactions_user(buffer) == 1){
            send_message(buffer);
        }
    } else if(strcmp(command, "mybids") == 0){ 
        strcpy(buffer, "LMB");
        strcat(buffer, buffer + strlen(command));
        if(mybids_user(buffer) == 1){
            send_message(buffer);
        }
    }
    else {
        perror("invalid input");
        exit(EXIT_FAILURE);
    }

}

int check_if_tcp(char buffer[]) {
    size_t buffer_len = strlen(buffer);

    for (size_t i = 0; i < sizeof(tcp_input) / sizeof(tcp_input[0]); ++i) {
        size_t input_len = strlen(tcp_input[i]);

        // Verifica se o tamanho da palavra no buffer é igual ao tamanho do input
        if (buffer_len >= input_len && strncmp(buffer, tcp_input[i], input_len) == 0) {
            return 1;
        }
    }

    return 0; 
}

void tcp(char buffer[]) {

    char command[33]; 
    sscanf(buffer, "%s", command);

    if(strcmp(command, "open") == 0){ 
        strcpy(buffer, "OPA");
        strcat(buffer, buffer + strlen(command)); 
    }
    else if(strcmp(command, "close") == 0){ 
        strcpy(buffer, "CLS");
        strcat(buffer, buffer + strlen(command)); 
    }
    else if(strcmp(command, "show_asset") == 0){ 
        strcpy(buffer, "SAS");
        strcat(buffer, buffer + strlen(command)); 
    }
    else if(strcmp(command, "bid") == 0){ 
        strcpy(buffer, "BID");
        strcat(buffer, buffer + strlen(command)); 
    }
    else {
        perror("invalid input");
        exit(EXIT_FAILURE);
    }
}


int main() {
    fd_set inputs, newfds;
    int max_fd = STDIN_FILENO;

    while(1) {

        FD_ZERO(&inputs); 
        FD_SET(max_fd, &inputs);
        newfds = inputs;


        if (select(max_fd + 1, &newfds, NULL, NULL, NULL) == -1) {

            perror("select");
            exit(EXIT_FAILURE);
        }
        
        if (FD_ISSET(STDIN_FILENO, &newfds)) {

            char buffer[1024];

            if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                perror("fgets");
                exit(EXIT_FAILURE);
            }

            if (check_if_tcp(buffer)) {
                tcp(buffer);
            }
            else {
                udp(buffer);
            }
            
        }
        
    }

}