#include "actions.h"
#include "string.h"
#include "stdio.h"
#include "ctype.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

User parse_user_credentials(char buffer[])
{
    User user;
    sscanf(buffer, "%*s\t%s\t%s", user.uid, user.password);
    return user;
}

int verify_user_credentials(User user)
{
    if (strlen(user.uid) != UID_SIZE || strlen(user.password) != PASS_SIZE)
    {
        printf("Invalid input: uid or password size wrong\n");
        return 0;
    }
    for (int i = 0; i < strlen(user.uid); i++)
    {
        if (!isdigit(user.uid[i]))
        {
            printf("Invalid input: uid\n");
            return 0;
        }
    }
    for (int i = 0; i < strlen(user.password); i++)
    {
        if (!isdigit(user.password[i]) && !isalpha(user.password[i]))
        {
            printf("Invalid input: password\n");
            return 0;
        }
    }

    return 1;
}

int verify_UID(char buffer[])
{
    char command[20], uid[8];
    sscanf(buffer, "%s\t%s", command, uid);
    if (strlen(uid) != UID_SIZE)
    {
        printf("Invalid input: uid or password size wrong\n");
        return 0;
    }
    for (int i = 0; i < strlen(uid); i++)
    {
        if (!isdigit(uid[i]))
        {
            printf("Invalid input: uid\n");
            return 0;
        }
    }
    return 1;
}

int verify_AID(char buffer[])
{
    char command[20], aid[8];
    sscanf(buffer, "%s\t%s", command, aid);
    if (strlen(aid) != 3)
    {
        printf("Invalid input: uid or password size wrong\n");
        return 0;
    }
    for (int i = 0; i < strlen(aid); i++)
    {
        if (!isdigit(aid[i]))
        {
            printf("Invalid input: uid\n");
            return 0;
        }
    }
    return 1;
}

int login_user(char buffer[])
{
    User user = parse_user_credentials(buffer);
    create_user_folder(user);
    return verify_user_credentials(user);
}

int logout_user(char buffer[])
{
    User user = parse_user_credentials(buffer);
    return verify_user_credentials(user);
}

int unregister_user(char buffer[])
{
    User user = parse_user_credentials(buffer);
    return verify_user_credentials(user);
}

int myactions_user(char buffer[])
{
    return verify_UID(buffer);
}

int mybids_user(char buffer[])
{
    return verify_UID(buffer);
}

int list_user(char buffer[])
{
    return verify_UID(buffer);
}

int show_record_user(char buffer[])
{
    return verify_AID(buffer);
}

void create_user_folder(User user)
{
    // create the User folder
    char folder_Path[19];
    sprintf(folder_Path, "ASDIR/USERS/%s", user.uid);
    if (mkdir(folder_Path, 0777) == 0)
    {
        create_pass_file(user);
        create_login_file(user);
        create_hosted_folder(user);
        create_bidded_folder(user);
    }
    else
    {
        perror("Error creating user folder");
    }
}

void create_pass_file(User user)
{
    char pass_file_path[37];
    // N sei se é necessário checkar se o folder do user existe primeiro
    sprintf(pass_file_path, "ASDIR/USERS/%s/%s_pass.txt", user.uid, user.uid);

    FILE *pass_file = fopen(pass_file_path, "w");

    if (pass_file != NULL)
    {
        fprintf(pass_file, "%s", user.password);
        fclose(pass_file);
        // printf("Password file created successfully: %s\n", pass_file_path);
    }
    else
    {
        perror("Error creating password file");
    }
}

void create_login_file(User user)
{
    char login_file_path[38];
    // N sei se é necessário checkar se o folder do user existe primeiro
    sprintf(login_file_path, "ASDIR/USERS/%s/%s_login.txt", user.uid, user.uid);

    FILE *login_file = fopen(login_file_path, "w");

    if (login_file != NULL)
    {
        fclose(login_file);
        // printf("Login file created successfully: %s\n", login_file_path);
    }
    else
    {
        perror("Error creating login file");
    }
}

void create_hosted_folder(User user)
{
    char hosted_folder_path[26];
    // N sei se é necessário checkar se o folder do user existe primeiro
    sprintf(hosted_folder_path, "ASDIR/USERS/%s/HOSTED", user.uid);
    if (mkdir(hosted_folder_path, 0777))
    {
        printf("Created Hosted folder sucessfully");
    }
    else
    {
        perror("Error creating user folder");
    }
}

void create_bidded_folder(User user)
{
    char bidded_folder_Path[26];
    // N sei se é necessário checkar se o folder do user existe primeiro
    sprintf(bidded_folder_Path, "ASDIR/USERS/%s/BIDDED", user.uid);
    if (mkdir(bidded_folder_Path, 0777) == 0)
    {
        printf("test");
    }
}
