#include "actions.h"

int verify_UID(char uid[])
{

    int uid_length = calculate_str_length(uid);
    if ((uid_length - 6) != 0)
    {
        printf("Invalid input: uid size wrong.\n");
        return 0;
    }
    for (int i = 0; i < strlen(uid); i++)
    {
        if (!isdigit(uid[i]))
        {
            printf("Invalid input: uid not a number\n");
            return 0;
        }
    }
    return 1;
}

int verify_user_credentials(char uid[], char pass[])
{
    if (verify_UID(uid) != 1)
    {
        return 0;
    }
    int pass_length = calculate_str_length(pass);
    if ((pass_length - 8) != 0)
    {
        printf("Invalid input: password size wrong\n");
        return 0;
    }
    for (int i = 0; i < strlen(pass); i++)
    {
        if (!isdigit(pass[i]) && !isalpha(pass[i]))
        {
            printf("Invalid input: password\n");
            return 0;
        }
    }

    return 1;
}

int verify_AID(char aid[])
{
    int aid_length = calculate_str_length(aid);
    if ((aid_length - AID_SIZE) != 0)
    {
        printf("Invalid input: aid size wrong.\n");
        return 0;
    }
    for (int i = 0; i < strlen(aid); i++)
    {
        if (!isdigit(aid[i]))
        {
            printf("Invalid input: aid not a number\n");
            return 0;
        }
    }
    return 1;
}

int calculate_str_length(char buffer[])
{
    int length = 0;
    while (buffer[length] != '\0')
    {
        length++;
    }
    return length;
}

// Returns Status
int login_user(char uid[], char pass[])
{
    // Check if is already registered
    if (check_user(uid))
    {
        // User already Registered
        // Check password file
        if (check_user_password(uid, pass))
        {
            // Status OK
            return 1;
        }
        // STATUS NOK
        return 0;
    }
    else
    {
        // Register User
        create_user_folder(uid, pass);
        return 2;
    }
}
/*
int logout_user(char buffer[])
{
    User user = parse_user_credentials(buffer);
    return verify_user_credentials(user);
}
*/

/*
int unregister_user(char buffer[])
{
    User user = parse_user_credentials(buffer);
    return verify_user_credentials(user);
}
*/

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
    return 1;
}

int show_record_user(char buffer[])
{
    return verify_AID(buffer);
}

int create_user_folder(char uid[], char pass[])
{
    // create the User folder
    char folder_Path[13];
    sprintf(folder_Path, "USERS/%s", uid);
    if (mkdir(folder_Path, 0700) == 0)
    {
        if (!create_hosted_folder(uid))
        {
            rmdir(folder_Path);
            return 0;
        }
        if (!create_bidded_folder(uid))
        {
            rmdir(folder_Path);
            return 0;
        }
        if (!create_pass_file(uid, pass))
        {
            rmdir(folder_Path);
            return 0;
        }
        if (!create_login_file(uid))
        {
            rmdir(folder_Path);
            return 0;
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

int create_pass_file(char uid[], char pass[])
{
    char pass_file_path[34];
    FILE *pass_file;
    sprintf(pass_file_path, "USERS/%s/%s_pass.txt", uid, uid);
    pass_file = fopen(pass_file_path, "w");
    if (pass_file != NULL)
    {
        fprintf(pass_file, "%s", pass);
        fclose(pass_file);
        return 1;
    }
    else
    {
        return 0;
    }
}

int create_login_file(char uid[])
{
    char login_file_path[35];
    FILE *login_file;
    // checkam o comprimento de uid no Guia acho q n é preciso once again se fizermos num parse ou num verify
    sprintf(login_file_path, "USERS/%s/%s_login.txt", uid, uid);
    login_file = fopen(login_file_path, "w");
    if (login_file != NULL)
    {
        fclose(login_file);
        return 1;
    }
    else
    {
        return 0;
    }
}

int delete_login_file(char uid[])
{
    char login_file_path[35];
    sprintf(login_file_path, "USERS/%s/%s_login.txt", uid, uid);
    if (unlink(login_file_path) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int create_hosted_folder(char uid[])
{
    char hosted_folder_path[20];
    sprintf(hosted_folder_path, "USERS/%s/HOSTED", uid);
    if (mkdir(hosted_folder_path, 0700) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int create_bidded_folder(char uid[])
{
    char bidded_folder_Path[20];
    sprintf(bidded_folder_Path, "USERS/%s/BIDDED", uid);
    if (mkdir(bidded_folder_Path, 0700) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int create_hosted_auction_file(User user, Auction auc)
{
    char hosted_file_path[28];
    FILE *hosted_file;
    sprintf(hosted_file_path, "USERS/%s/HOSTED/%03d.txt", user.uid, auc.aid);
    hosted_file = fopen(hosted_file_path, "w");
    if (hosted_file != NULL)
    {
        fclose(hosted_file);
        return 1;
    }
    else
    {
        return 0;
    }
}

int create_bidded_auction_file(User user, Auction auc)
{
    char bidded_file_path[28];
    FILE *bidded_file;
    sprintf(bidded_file_path, "USERS/%s/BIDDED/%03d.txt", user.uid, auc.aid);
    bidded_file = fopen(bidded_file_path, "w");
    if (bidded_file != NULL)
    {
        fclose(bidded_file);
        return 1;
    }
    else
    {
        return 0;
    }
}

int create_auction_folder(int aid)
{
    // create the Auction folder
    char folder_Path[15];
    // No Guia eles checkam o AID SIZE acho q n valia a pena ser aqui é melhor probbly na criacao da struct do auction
    sprintf(folder_Path, "AUCTIONS/%03d", aid);
    if (mkdir(folder_Path, 0700) == 0)
    {
        if (!create_asset_folder(aid))
        {
            rmdir(folder_Path);
            return 0;
        }
        if (!create_bids_folder(aid))
        {
            rmdir(folder_Path);
            return 0;
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

int create_start_file(Auction auc, User user)
{
    char start_file_path[30];
    sprintf(start_file_path, "AUCTIONS/%03d/START_%03d.txt", auc.aid, auc.aid);
    FILE *start_file = fopen(start_file_path, "w");
    if (start_file != NULL)
    {
        fprintf(start_file, "%s\n%s\n%s\n%d\n%ld\n%4d−%02d−%02d %02d:%02d:%02d\n%ld", user.uid, auc.name, auc.asset_fname,
                auc.start_value, (long)auc.duration, auc.start_datetime->tm_year + 1900, auc.start_datetime->tm_mon + 1,
                auc.start_datetime->tm_mday, auc.start_datetime->tm_hour, auc.start_datetime->tm_min, auc.start_datetime->tm_sec,
                (long)auc.start_fulltime);
        fclose(start_file);
        return 1;
    }
    else
    {
        return 0;
    }
}

int create_asset_folder(int aid)
{
    char asset_folder_Path[21];
    sprintf(asset_folder_Path, "AUCTIONS/%03d/ASSET", aid);
    if (mkdir(asset_folder_Path, 0700) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int create_asset_file(Auction auc)
{
    printf("...");
    return 0;
}

int create_end_file(Auction auc)
{
    char end_file_path[26];
    sprintf(end_file_path, "AUCTIONS/%3d/END_%03d.txt", auc.aid, auc.aid);
    FILE *end_file = fopen(end_file_path, "w");
    if (end_file != NULL)
    {
        // Care n sei se as contas são assim
        time_t end_time = auc.start_fulltime + auc.duration;
        struct tm *end_datetime = gmtime(&(end_time));
        fprintf(end_file, "%4d−%02d−%02d %02d:%02d:%02d\n%ld", end_datetime->tm_year + 1900, end_datetime->tm_mon + 1,
                end_datetime->tm_mday, end_datetime->tm_hour, end_datetime->tm_min, end_datetime->tm_sec, (long)auc.duration);
        fclose(end_file);
        return 1;
    }
    else
    {
        return 0;
    }
}

int create_bids_folder(int aid)
{
    char bids_folder_Path[20];
    sprintf(bids_folder_Path, "AUCTIONS/%03d/BIDS", aid);
    if (mkdir(bids_folder_Path, 0777) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int create_bid_file(Auction auc, User user, int bid_value)
{
    char bid_file_path[26];
    sprintf(bid_file_path, "AUCTIONS/%03d/%06d.txt", auc.aid, auc.start_value);
    FILE *bid_file = fopen(bid_file_path, "w");
    if (bid_file != NULL)
    {
        // Care n sei se as contas são assim
        time_t full_time;
        struct tm *current_time;
        time(&full_time);
        current_time = gmtime(&full_time);
        fprintf(bid_file, "%s\n%d\n%4d−%02d−%02d %02d:%02d:%02d\n%ld", user.uid, bid_value, current_time->tm_year + 1900,
                current_time->tm_mon + 1, current_time->tm_mday, current_time->tm_hour, current_time->tm_min, current_time->tm_sec,
                (long)(full_time - auc.start_fulltime));
        fclose(bid_file);
        return 1;
    }
    else
    {
        return 0;
    }
}

// Check if user exists
int check_user(char uid[])
{
    struct dirent **entrylist;
    char dirname[7] = "USERS/";
    int n_entries = scandir(dirname, &entrylist, NULL, alphasort);
    if (n_entries < 0)
    {
        perror("scandir");
        return 0;
    }
    // Care here on the loop for later should work for now
    int folderFound = 0;
    for (int i = 0; i < n_entries; ++i)
    {
        if (entrylist[i]->d_type == DT_DIR && strcmp(entrylist[i]->d_name, uid) == 0)
        {
            folderFound = 1;
            break;
        }
        free(entrylist[i]); // Free memory for each entry
    }
    free(entrylist); // Free the entryList array
    return folderFound;
}

// Check password
int check_user_password(char uid[], char pass[])
{
    char pass_file_path[34];
    FILE *pass_file;
    sprintf(pass_file_path, "USERS/%s/%s_pass.txt", uid, uid);
    pass_file = fopen(pass_file_path, "r");
    if (pass_file != NULL)
    {
        char stored_password[PASS_SIZE];
        if (fscanf(pass_file, "%s", stored_password) == 1)
        {
            if (strcmp(pass, stored_password) == 0)
            {
                fclose(pass_file);
                return 1;
            }
        }
        fclose(pass_file);
    }
    return 0;
}

// Check Logged in
int check_user_logged_in(char uid[])
{
    struct dirent **entrylist;
    char dirname[7] = "USERS/";
    int n_entries = scandir(dirname, &entrylist, NULL, alphasort);
    if (n_entries < 0)
    {
        perror("scandir");
        return 0;
    }
    // Care here on the loop for later should work for now
    int logged_in = 0;
    for (int i = 0; i < n_entries; ++i)
    {
        if (entrylist[i]->d_type == DT_DIR && strcmp(entrylist[i]->d_name, uid) == 0)
        {
            logged_in = 1;
            break;
        }
        free(entrylist[i]); // Free memory for each entry
    }
    free(entrylist); // Free the entryList array
    return logged_in;
}

// Checks if asset_file exists, if so returns the files size
int check_asset_file(char *fname)
{
    struct stat filestat;

    if (stat(fname, &filestat) == 0 && filestat.st_size != 0)
    {
        return filestat.st_size;
    }
    else
    {
        return 0;
    }
}