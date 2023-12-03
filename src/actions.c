#include "actions.h"
#include "string.h"
#include "stdio.h"
#include "ctype.h"

int verify_user_credentials(char buffer[])
{

    char command[20], uid[8], password[10];
    sscanf(buffer, "%s\t%s\t%s", command, uid, password);
    if (strlen(uid) != 6 || strlen(password) != 8)
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
    for (int i = 0; i < strlen(password); i++)
    {
        if (!isdigit(password[i]) && !isalpha(password[i]))
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
    if (strlen(uid) != 6)
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
    return verify_user_credentials(buffer);
}

int logout_user(char buffer[])
{
    return verify_user_credentials(buffer);
}

int unregister_user(char buffer[])
{
    return verify_user_credentials(buffer);
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
