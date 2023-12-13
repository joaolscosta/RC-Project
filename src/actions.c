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

int check_if_single_word(char buffer[])
{
    int length = 0;
    while (buffer[length] != '\0')
    {
        if (isspace(buffer[length]))
        {
            return 0;
        }
        length++;
    }
    return 1;
}

int check_minimum_selling_value(int bid_valuem, int start_value)
{
    if (bid_valuem < start_value)
    {
        return 0;
    }
    return 1;
}

int check_time_active(int current_time, int max_time_active)
{
    if (current_time > max_time_active)
    {
        return 0;
    }
    return 1;
}

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

int logout_user(char uid[], char pass[])
{
    // Check if is already registered
    if (check_user(uid))
    {
        if (check_user_logged_in(uid))
        {
            delete_login_file(uid);
            // STATUS OK
            return 1;
        }
        // STATUS NOK
        return 0;
    }
    else
    {
        // STATUS UNR
        return 2;
    }
}

int unregister_user(char uid[], char pass[])
{
    // Check if is already registered
    if (check_user(uid))
    {
        if (check_user_logged_in(uid))
        {
            delete_pass_file(uid);
            delete_login_file(uid);
            // STATUS OK
            return 1;
        }
        // STATUS NOK
        return 0;
    }
    else
    {
        // STATUS UNR
        return 2;
    }
}

int myauctions_user(char uid[], AUCTIONLIST *list)
{
    if (!check_user_logged_in(uid))
    {
        // STATUS NLG
        return 2;
    }
    int no_aucs = GetHostedAuctionlist(uid, &list); // N SEI SE TENHO DE PASSAR REF AGAIN

    if (no_aucs == 0)
    {
        // STATUS NOK
        return 0;
    }
    return 1; // STATUS OK
}

int mybids_user(char uid[])
{
    if (!check_user_logged_in(uid))
    {
        // STATUS NLG
        return 2;
    }
    char bids_list[999]; //? É ESTE O MÁXIMO?
    // strcpy(bids_list, GetBidList(...));
    int count = 0;
    for (int i = 0; i < sizeof(bids_list); i++)
    {
        count++;
    }
    if (count == 0)
    {
        // STATUS NOK
        return 0;
    }
    if (check_user(uid))
    {
        // STATUS OK
        return 1;
    }
}

int list_all_auctions()
{
    // N entendi esta funcao mas yah - isto foi mega copypaste, já alterei
    char auctions_list[999]; //? É ESTE O MÁXIMO?
    // strcpy(auctions_list, get_all_auctions());
    int count = 0;
    for (int i = 0; i < sizeof(auctions_list); i++)
    {
        count++;
    }
    if (count == 0)
    {
        // STATUS NOK
        return 0;
    }
    else
    {
        // STATUS OK
        return 1;
    }
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

int remove_user_folder(char uid[], char pass[])
{
    return 0;
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
    char login_file_path[30];
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
    char login_file_path[30];
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

int delete_pass_file(char uid[])
{
    char login_file_path[30];
    sprintf(login_file_path, "USERS/%s/%s_pass.txt", uid, uid);
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
        fprintf(bid_file, "%s %d %4d−%02d−%02d %02d:%02d:%02d %ld", user.uid, bid_value, current_time->tm_year + 1900,
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
    char dirname[13];
    sprintf(dirname, "USERS/%s", uid);
    int n_entries = scandir(dirname, &entrylist, NULL, alphasort);
    if (n_entries < 0)
    {
        return 0;
    }
    printf("devo printar: %d\n", n_entries);
    // Care here on the loop for later should work for now
    int pass_file_found = 0;
    while (n_entries--)
    {
        char pass_file[16];
        sprintf(pass_file, "%s_pass.txt", uid);
        if (entrylist[n_entries]->d_type == DT_REG && strcmp(entrylist[n_entries]->d_name, pass_file) == 0)
        {
            pass_file_found = 1;
            free(entrylist[n_entries]); // Free memory for each entry
            break;
        }
        free(entrylist[n_entries]); // Free memory for each entry
    }
    free(entrylist);
    return pass_file_found;
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
    char dirname[13];
    sprintf(dirname, "USERS/%s", uid);
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
        char login_file[17];
        sprintf(login_file, "%s_login.txt", uid);
        if (entrylist[i]->d_type == DT_REG && strcmp(entrylist[i]->d_name, login_file) == 0)
        {
            logged_in = 1;
            free(entrylist[i]); // Free memory for each entry
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

int GetHostedAuctionlist(char uid[], AUCTIONLIST *list)
{
    struct dirent **filelist;
    int nentries, naucs, len;
    char dirname[21];
    char pathname[32];
    sprintf(dirname, "USERS/%s/HOSTED/", uid);
    nentries = scandir(dirname, &filelist, 0, alphasort);
    if (nentries <= 0)
        return 0;
    naucs = 0;
    list->no_aucs = 0;
    while (nentries--)
    {
        len = strlen(filelist[nentries]->d_name);
        if (len == 10)
        {
            sprintf(pathname, "USERS/%s/HOSTED/%s", uid, filelist[nentries]->d_name);
            if (LoadAuction(pathname, list))
                ++naucs;
        }
        free(filelist[nentries]);
        if (naucs == 50)
            break;
    }
    free(filelist);
    return naucs;
}

int LoadAuction(const char *filepath, AUCTIONLIST *list)
{
    // Extract the AID from the filename
    char *token = strtok(filepath, ".");
    if (token != NULL)
    {
        int auctionID = atoi(token);
        // Store the auction ID in the AUCTIONLIST structure
        if (list->no_aucs < 50)
        {
            list->aucs[list->no_aucs++] = auctionID;
            return 1; // Success
        }
    }
    return 0; // Failed to load auction or list full
}

int GetBiddedAuctionlist(char uid[], AUCTIONLIST *list)
{
    struct dirent **filelist;
    int nentries, naucs, len;
    char dirname[21];
    char pathname[32];
    sprintf(dirname, "USERS/%s/BIDDED/", uid);
    nentries = scandir(dirname, &filelist, 0, alphasort);
    if (nentries <= 0)
        return 0;
    naucs = 0;
    list->no_aucs = 0;
    while (nentries--)
    {
        len = strlen(filelist[nentries]->d_name);
        if (len == 10)
        {
            sprintf(pathname, "USERS/%s/BIDDED/%s", uid, filelist[nentries]->d_name);
            if (LoadAuction(pathname, list))
                ++naucs;
        }
        free(filelist[nentries]);
        if (naucs == 50)
            break;
    }
    free(filelist);
    return naucs;
}

int GetBidList(int AID, BIDLIST *list)
{
    struct dirent **filelist;
    int nentries, nbids, len;
    char dirname[20];
    char pathname[32];
    sprintf(dirname, "AUCTIONS/%03d/BIDS/", AID);
    nentries = scandir(dirname, &filelist, 0, alphasort);
    if (nentries <= 0)
        return 0;
    nbids = 0;
    list->no_bids = 0;
    while (nentries--)
    {
        len = strlen(filelist[nentries]->d_name);
        if (len == 10)
        {
            sprintf(pathname, "AUCTIONS/%03d/BIDS/%s", AID, filelist[nentries]->d_name);
            if (LoadBid(pathname, list))
                ++nbids;
        }
        free(filelist[nentries]);
        if (nbids == 50)
            break;
    }
    free(filelist);
    return nbids;
}

int LoadBid(const char *filepath, BIDLIST *list)
{
    FILE *file = fopen(filepath, "r");
    if (file == NULL)
    {
        perror("Error opening bid file");
        return 0;
    }

    if (list->no_bids < 50)
    {
        BIDINFO *current_bid = &list->bids[list->no_bids];
        if (fscanf(file, "%s %6d %4d−%02d−%02d %02d:%02d:%02d %ld",
                   current_bid->UID,
                   &current_bid->bid_value,
                   &current_bid->bid_datetime->tm_year,
                   &current_bid->bid_datetime->tm_mon, &current_bid->bid_datetime->tm_mday, &current_bid->bid_datetime->tm_hour,
                   &current_bid->bid_datetime->tm_min, &current_bid->bid_datetime->tm_sec, (long)&current_bid->bid_sec_time) == 9)
        {
            list->no_bids++;
            fclose(file);
            return 1; // Return 1 to indicate success
        }
    }
    fclose(file);
    return 0; // Return 0 to indicate failure or exceeding bid limit
}

void DisplayAuctions(AUCTIONLIST *list, char *response)
{
    for (int i = 0; i < list->no_aucs; ++i)
    {
        sprintf(response + strlen(response), " %03d 0", list->aucs[i]);
    }
}

int check_auction_name(char auction_name[])
{
    int auction_name_length = calculate_str_length(auction_name);
    if (auction_name_length > 10)
    {
        printf("Invalid input: auction size wrong.\n");
        return 0;
    }
    for (int i = 0; i < strlen(auction_name); i++)
    {
        if (!isdigit(auction_name[i]) || !isalpha(auction_name[i]))
        {
            printf("Invalid input: auctions not alphanumerical.\n");
            return 0;
        }
    }
    return 1;
}

int check_file_name(char file_name[])
{
    return 1;
}

int check_start_value(int start_value)
{
    int count = 0;

    if (start_value == 0) //! verificar isto
    {
        return 0;
    }

    while (start_value != 0)
    {
        start_value = start_value / 10;
        count++;
    }

    if (count > 6)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int check_time_active_input(int time_active)
{
    int count = 0;

    while (time_active != 0)
    {
        time_active = time_active / 10;
        count++;
    }

    if (count > 5)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int check_open_credentials(char auction_name[], char file_name[], int start_value, int time_active)
{
    int check = 0;

    check = check_auction_name(auction_name);
    check = check_file_name(file_name);
    check = check_start_value(start_value);
    check = check_time_active_input(time_active);

    return check;
}