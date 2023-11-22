#include "aux.h"
#include "string.h"
#include "stdio.h"
#include "ctype.h"


int loginUser(char buffer[]){

    char command[20], uid[8], password[10];
    sscanf(buffer, "%s\t%s\t%s", command, uid, password);
    if(strlen(uid)!=6 || strlen(password)!=8){
        printf("Invalid input: uid or password size wrong\n");
        return 0;
    }
    for (int i=0; i<strlen(uid);i++){
        if(!isdigit(uid[i])){
            printf("Invalid input: uid\n");
            return 0;
        }
    }
    for (int i=0; i<strlen(password);i++){
        if(!isdigit(password[i]) && !isalpha(password[i])){
            printf("Invalid input: password\n");
            return 0;
        }
    }

    return 1;
}
