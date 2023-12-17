#include "actions.h"

int reverseAlphasort(const struct dirent **a, const struct dirent **b)
{
    return alphasort(b, a);
}

int verify_UID(char uid[])
{
    int uid_length = calculate_str_length(uid);
    if ((uid_length - 6) != 0)
    {
        printf("Invalid input: uid size wrong.\n");
        return 0;
    }
    size_t i;
    for (i = 0; i < strlen(uid); i++)
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
    size_t i;
    for (i = 0; i < strlen(pass); i++)
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
    size_t i;
    for (i = 0; i < strlen(aid); i++)
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
    if (LookUpUser(uid))
    {
        // User already Registered
        // Check password file
        if (LookUpUserPassword(uid, pass))
        {
            create_login_file(uid);
            // Status OK
            return 1;
        }
        // STATUS NOK
        return 0;
    }
    else
    {
        // Register User
        // TODO METER NUM IF PROBBLY ESTE CREATE USER FOLDER
        create_user_folder(uid, pass);
        return 2;
    }
}

int logout_user(char uid[])
{
    // Check if is already registered
    if (LookUpUser(uid))
    {
        if (LookUpUserLogin(uid))
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

int unregister_user(char uid[])
{
    // Check if is already registered
    if (LookUpUser(uid))
    {
        if (LookUpUserLogin(uid))
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
    if (!LookUpUserLogin(uid))
    {
        // STATUS NLG
        return 2;
    }
    int no_aucs = GetHostedAuctionlist(uid, list);
    if (no_aucs == 0)
    {
        // STATUS NOK
        return 0;
    }
    return 1; // STATUS OK
}

int mybids_user(char uid[], AUCTIONLIST *list)
{
    if (!LookUpUserLogin(uid))
    {
        // STATUS NLG
        return 2;
    }
    int no_aucs = GetBiddedAuctionlist(uid, list);
    if (no_aucs == 0)
    {
        // STATUS NOK
        return 0;
    }
    return 1; // STATUS OK
}

int list_all_auctions(AUCTIONLIST *list)
{
    int no_aucs = GetAuctionlist(list);
    if (no_aucs == 0)
    {
        // STATUS NOK
        return 0;
    }
    return 1; // STATUS OK
}

int show_record_user(int aid, AUCTIONINFO *auc_info, BIDLIST *bid_list)
{
    if (!LookUpAuction(aid, auc_info, bid_list))
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

int open_auction(AUCTIONINFO info, FILEINFO file, char *file_data, int *aid)
{
    // Check if user is logged in
    // PQ É Q EU RECEBO A PASS NO OPEN??? É PARA VERIFICAR SE TA LOGGED IN? EU PROCURO SÓ PELO FICHEIRO LOGIN
    if (!LookUpUserLogin(info.uid))
    {
        // STATUS NLG
        return 2;
    }
    // Create Auction Folder
    // Generate AID
    *aid = GenerateAid();
    if (create_auction_folder(*aid, info, file, file_data) && create_hosted_auction_file(info.uid, *aid))
    {
        // STATUS OK
        return 1;
    }
    // STATUS NOK
    return 0;
}

int close_auction(USERINFO info, int aid)
{
    // Check if user is logged in
    // PQ É Q EU RECEBO A PASS NO CLOSE??? É PARA VERIFICAR SE TA LOGGED IN? EU PROCURO SÓ PELO FICHEIRO LOGIN
    if (!LookUpUserLogin(info.uid))
    {
        // STATUS NLG
        return 0;
    }
    int result = CheckAuction(info, aid);
    switch (result)
    {
    case 0:
        // STATUS EAU
        return 1;
    case 1:
        // STATUS EOW
        return 2;
    case 2:
        // STATUS END
        return 3;
    case 3:
        create_end_file(aid);
        // STATUS OK
        return 4;
    }
}

int show_asset(int aid, FILEINFO *file, char **file_data)
{
    if (LookUpAssetFile(aid, file, file_data))
    {
        Download_Asset(file, *file_data);
        // STATUS OK
        return 1;
    }
    // STATUS NOK
    return 0;
}

int bid(USERINFO info, int aid, int bid_value)
{
    // ESTRANHO NO ENUNCIADO N DIZEM PARA CHECKAR SE O AUC EXISTE OU N
    // PQ É Q EU RECEBO A PASS NO BID??? É PARA VERIFICAR SE TA LOGGED IN? EU PROCURO SÓ PELO FICHEIRO LOGIN
    // Check if user is logged in
    if (!auctionExists(aid))
    {
        return 5;
    }
    if (!LookUpUserLogin(info.uid))
    {
        // STATUS NLG
        return 0;
    }
    if (isAuctionEnded(aid))
    {
        // STATUS NOK
        return 1;
    }
    if (isOwner(aid, info.uid))
    {
        // STATUS ILG
        return 2;
    }
    if (bid_value <= GetHighestBid(aid) || bid_value <= getAuctionStartValue(aid))
    {
        // STATUS LOW
        return 3;
    }
    create_bid_file(info.uid, aid, bid_value);
    create_bidded_auction_file(info.uid, aid);
    // STATUS ACC
    return 4;
}

// Register User
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
        // TODO VER pq é q isto ta aqui meti mas ja n sei se faz sentido
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

int create_hosted_auction_file(char uid[], int aid)
{
    char hosted_file_path[28];
    FILE *hosted_file;
    sprintf(hosted_file_path, "USERS/%s/HOSTED/%03d.txt", uid, aid);
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

int create_bidded_auction_file(char uid[], int aid)
{
    char bidded_file_path[28];
    FILE *bidded_file;
    sprintf(bidded_file_path, "USERS/%s/BIDDED/%03d.txt", uid, aid);
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

int create_auction_folder(int aid, AUCTIONINFO auc, FILEINFO file, char *file_data)
{
    // create the Auction folder
    char folder_Path[15];
    sprintf(folder_Path, "AUCTIONS/%03d", aid);
    if (mkdir(folder_Path, 0700) == 0)
    {
        if (!create_start_file(aid, auc))
        {
            rmdir(folder_Path);
            return 0;
        }
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
        if (!create_asset_file(aid, file, file_data))
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

int create_start_file(int aid, AUCTIONINFO auc)
{
    char start_file_path[30];
    sprintf(start_file_path, "AUCTIONS/%03d/START_%03d.txt", aid, aid);
    FILE *start_file = fopen(start_file_path, "w");
    if (start_file != NULL)
    {
        // Calculate start_fulltime
        time_t fulltime;
        struct tm *current_time;
        time(&fulltime);
        current_time = gmtime(&fulltime);
        // Write the start file
        fprintf(start_file, "%s %s %s %d %d %4d-%02d-%02d %02d:%02d:%02d %ld", auc.uid, auc.name, auc.asset_fname,
                auc.start_value, auc.timeactive, current_time->tm_year + 1900, current_time->tm_mon + 1,
                current_time->tm_mday, current_time->tm_hour, current_time->tm_min, current_time->tm_sec,
                (long)fulltime);
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

int create_asset_file(int aid, FILEINFO file, char *file_data)
{
    char asset_file_path[45];
    sprintf(asset_file_path, "AUCTIONS/%03d/ASSET/%s", aid, file.file_name);
    FILE *asset_file = fopen(asset_file_path, "wb");
    if (asset_file != NULL)
    {
        // printf("file size: %d\n", file.file_size);
        size_t bytes_written = fwrite(file_data, 1, file.file_size, asset_file);
        // printf("bytes written: %ld\n", bytes_written);
        if (bytes_written != file.file_size)
        {
            fprintf(stderr, "Error: Could not write the entire file_data to the file.\n");
            fclose(asset_file);
            return 0;
        }
        fclose(asset_file);
        return 1;
    }
    else
    {
        return 0;
    }
}

int create_end_file(int aid)
{
    // Get the current time
    time_t end_time = time(NULL);

    long start_fulltime = getAuctionTime(aid);

    // Calculate the duration of the auction
    double duration = (long)end_time - start_fulltime;

    // Convert the end time to a string in the format YYYY-MM-DD HH:MM:SS
    char end_datetime[20];
    strftime(end_datetime, 20, "%Y-%m-%d %H:%M:%S", localtime(&end_time));

    // Create the filename
    char filename[19];
    sprintf(filename, "AUCTIONS/%03d/END_%03d.txt", aid, aid);
    // Open the end file
    FILE *file = fopen(filename, "w");
    if (file != NULL)
    {
        // Write the end datetime and duration to the file
        fprintf(file, "%s %d\n", end_datetime, (int)duration);
        // Close the file
        fclose(file);
        return 1;
    }
    return 0;
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

int create_bid_file(char uid[], int aid, int bid_value)
{
    // TODO get Auction only for start time
    long start_fulltime = getAuctionTime(aid);
    char bid_file_path[30];
    sprintf(bid_file_path, "AUCTIONS/%03d/BIDS/%06d.txt", aid, bid_value);
    FILE *bid_file = fopen(bid_file_path, "w");
    if (bid_file != NULL)
    {
        // Care n sei se as contas são assim
        time_t full_time;
        struct tm *current_time;
        time(&full_time);
        current_time = gmtime(&full_time);
        fprintf(bid_file, "%s %d %4d-%02d-%02d %02d:%02d:%02d %ld", uid, bid_value, current_time->tm_year + 1900,
                current_time->tm_mon + 1, current_time->tm_mday, current_time->tm_hour, current_time->tm_min, current_time->tm_sec,
                (long)((long)full_time - start_fulltime));
        fclose(bid_file);
        return 1;
    }
    else
    {
        return 0;
    }
}

// Check if user exists
int LookUpUser(char uid[])
{
    struct dirent **entrylist;
    char dirname[13];
    sprintf(dirname, "USERS/%s", uid);
    int n_entries = scandir(dirname, &entrylist, NULL, alphasort);
    if (n_entries < 0)
    {
        return 0;
    }
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
int LookUpUserPassword(char uid[], char pass[])
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
int LookUpUserLogin(char uid[])
{
    // TODO CHECKAR SE A PASS É IGUAL TMB
    struct dirent **entrylist;
    char dirname[13];
    sprintf(dirname, "USERS/%s", uid);
    int n_entries = scandir(dirname, &entrylist, NULL, alphasort);
    if (n_entries < 0)
    {
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

int LookUpAssetFile(int aid, FILEINFO *file, char **file_data)
{
    struct dirent **entrylist;
    char dirname[21];
    sprintf(dirname, "AUCTIONS/%03d/ASSET", aid);
    int n_entries = scandir(dirname, &entrylist, NULL, alphasort);
    if (n_entries < 0)
    {
        // TODO REMOVE Perror
        perror("scandir");
        return 0;
    }
    int asset_file_found = 0;
    while (n_entries--)
    {
        if (entrylist[n_entries]->d_type == DT_REG)
        {
            char asset_file_path[50]; // Adjust the size as needed
            sprintf(asset_file_path, "AUCTIONS/%03d/ASSET/%s", aid, entrylist[n_entries]->d_name);
            FILE *asset_file = fopen(asset_file_path, "rb");
            if (asset_file != NULL)
            {
                // Determine the file size
                fseek(asset_file, 0, SEEK_END);
                size_t file_size = ftell(asset_file);
                fseek(asset_file, 0, SEEK_SET);

                // Allocate memory for the binary data
                *file_data = (char *)malloc(file_size);
                if (*file_data == NULL)
                {
                    perror("Error allocating memory");
                    fclose(asset_file);
                    continue;
                }
                // Read the binary data
                size_t bytes_read = fread(*file_data, 1, file_size, asset_file);
                if (bytes_read != file_size)
                {
                    perror("Error reading file");
                    free(*file_data);
                    fclose(asset_file);
                    continue;
                }
                strcpy(file->file_name, entrylist[n_entries]->d_name);
                file->file_size = file_size;
                asset_file_found = 1;

                fclose(asset_file);
            }
        }
        free(entrylist[n_entries]); // Free memory for each entry
    }
    free(entrylist); // Free the entryList array after the loop
    return asset_file_found;
}

int Download_Asset(FILEINFO *file, char *file_data)
{
    FILE *downloaded_file = fopen(file->file_name, "wb");
    if (downloaded_file != NULL)
    {
        size_t bytes_written = fwrite(file_data, 1, file->file_size, downloaded_file);
        if (bytes_written != file->file_size)
        {
            fprintf(stderr, "Error: Could not write the entire file_data to the file.\n");
            fclose(downloaded_file);
            return 0;
        }
        fclose(downloaded_file);
        return 1;
    }
    return 0;
}

int LookUpAuction(int aid, AUCTIONINFO *auc, BIDLIST *list)
{
    int auction_folder_found = 0; // QUANDO SE ENCONTRA O START FILE
    char start_file[21];
    char end_file[19];
    sprintf(start_file, "START_%03d.txt", aid);
    sprintf(end_file, "END_%03d.txt", aid);
    auc->start_datetime = (struct tm *)malloc(sizeof(struct tm));
    auc->end_datetime = (struct tm *)malloc(sizeof(struct tm));
    auction_folder_found = ReadStartFile(aid, auc);
    ReadEndFile(aid, auc);
    GetBidList(aid, list);
    return auction_folder_found;
}

int ReadStartFile(int aid, AUCTIONINFO *auc)
{
    struct dirent **entrylist;
    int n_entries;
    char dirname[21];
    char start_file[32];
    sprintf(start_file, "START_%03d.txt", aid);
    sprintf(dirname, "AUCTIONS/%03d", aid);
    n_entries = scandir(dirname, &entrylist, 0, NULL);
    if (n_entries <= 0)
    {
        free(entrylist);
        return 0;
    }
    while (n_entries--)
    {
        if (entrylist[n_entries]->d_type == DT_REG && strcmp(entrylist[n_entries]->d_name, start_file) == 0)
        {
            char start_file_path[30];
            sprintf(start_file_path, "AUCTIONS/%03d/START_%03d.txt", aid, aid);
            FILE *start_file = fopen(start_file_path, "r");
            if (start_file != NULL)
            {
                int year, month, day, hour, minute, second;
                fscanf(start_file, "%s %s %s %d %d %4d-%02d-%02d %02d:%02d:%02d %d",
                       auc->uid, auc->name, auc->asset_fname, &auc->start_value, &auc->timeactive,
                       &year, &month, &day, &hour, &minute, &second, &auc->start_fulltime);
                auc->start_datetime->tm_year = year;
                auc->start_datetime->tm_mon = month;
                auc->start_datetime->tm_mday = day;
                auc->start_datetime->tm_hour = hour;
                auc->start_datetime->tm_min = minute;
                auc->start_datetime->tm_sec = second;
                fclose(start_file);
                return 1;
            }
        }
        free(entrylist[n_entries]);
    }
    free(entrylist);
    return 0;
}

int ReadEndFile(int aid, AUCTIONINFO *auc)
{
    struct dirent **entrylist;
    int n_entries;
    char dirname[21];
    char end_file[32];
    sprintf(end_file, "END_%03d.txt", aid);
    sprintf(dirname, "AUCTIONS/%03d", aid);
    n_entries = scandir(dirname, &entrylist, 0, NULL);
    if (n_entries <= 0)
    {
        free(entrylist);
        return 0;
    }
    while (n_entries--)
    {
        if (entrylist[n_entries]->d_type == DT_REG && strcmp(entrylist[n_entries]->d_name, end_file) == 0)
        {
            char end_file_path[30];
            sprintf(end_file_path, "AUCTIONS/%03d/END_%03d.txt", aid, aid);
            FILE *end_file = fopen(end_file_path, "r");
            if (end_file != NULL)
            {
                int year, month, day, hour, minute, second;
                fscanf(end_file, "%4d-%02d-%02d %02d:%02d:%02d %d",
                       &year, &month, &day, &hour, &minute, &second,
                       &auc->end_sec_time);
                auc->end_datetime->tm_year = year;
                auc->end_datetime->tm_mon = month;
                auc->end_datetime->tm_mday = day;
                auc->end_datetime->tm_hour = hour;
                auc->end_datetime->tm_min = minute;
                auc->end_datetime->tm_sec = second;
                fclose(end_file);
                return 1;
            }
        }
        free(entrylist[n_entries]);
    }
    free(entrylist);
    return 0;
}

int GetHostedAuctionlist(char uid[], AUCTIONLIST *list)
{
    struct dirent **filelist;
    int nentries, naucs, len;
    char dirname[21];
    char pathname[32];
    sprintf(dirname, "USERS/%s/HOSTED/", uid);
    nentries = scandir(dirname, &filelist, 0, NULL);
    if (nentries <= 0)
        return 0;
    naucs = 0;
    list->no_aucs = 0;
    while (nentries--)
    {
        len = strlen(filelist[nentries]->d_name);
        if (len == 7)
        {
            sprintf(pathname, "USERS/%s/HOSTED/%s", uid, filelist[nentries]->d_name);
            if (LoadAuction(filelist[nentries]->d_name, list))
                ++naucs;
        }
        free(filelist[nentries]);
    }
    free(filelist);
    return naucs;
}

int LoadAuction(const char *filepath, AUCTIONLIST *list)
{
    if (sscanf(filepath, "%03d.txt", &list->aucs[list->no_aucs]) == 1)
    {
        ++list->no_aucs;
        return 1;
    }
    else
    {
        return 0;
    }
}

int GetBiddedAuctionlist(char uid[], AUCTIONLIST *list)
{
    struct dirent **filelist;
    int nentries, naucs, len;
    char dirname[21];
    char pathname[32];
    sprintf(dirname, "USERS/%s/BIDDED/", uid);
    nentries = scandir(dirname, &filelist, 0, NULL);
    if (nentries <= 0)
        return 0;
    naucs = 0;
    list->no_aucs = 0;
    while (nentries--)
    {
        len = strlen(filelist[nentries]->d_name);
        if (len == 7)
        {
            sprintf(pathname, "USERS/%s/BIDDED/%s", uid, filelist[nentries]->d_name);
            if (LoadAuction(filelist[nentries]->d_name, list))
                ++naucs;
        }
        free(filelist[nentries]);
    }
    free(filelist);
    return naucs;
}

int GetBidList(int AID, BIDLIST *list)
{
    struct dirent **filelist;
    int nentries, nbids, len;
    char dirname[32];
    char pathname[32];
    sprintf(dirname, "AUCTIONS/%03d/BIDS", AID);
    nentries = scandir(dirname, &filelist, 0, reverseAlphasort);
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

int GetAuctionlist(AUCTIONLIST *list)
{
    struct dirent **filelist;
    int nentries, naucs, len;
    char dirname[11];
    char pathname[32];
    sprintf(dirname, "AUCTIONS/");
    nentries = scandir(dirname, &filelist, 0, reverseAlphasort);
    if (nentries <= 0)
        return 0;
    naucs = 0;
    list->no_aucs = 0;
    while (nentries--)
    {
        len = strlen(filelist[nentries]->d_name);
        if (len == 3)
        {
            sprintf(pathname, "AUCTIONS/%s", filelist[nentries]->d_name);
            if (LoadAuction(filelist[nentries]->d_name, list))
                ++naucs;
        }
        free(filelist[nentries]);
    }
    free(filelist);
    return naucs;
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
        list->bids[list->no_bids].bid_datetime = (struct tm *)malloc(sizeof(struct tm));
        int year, month, day, hour, minute, second;
        if (fscanf(file, "%s %d %4d-%02d-%02d %02d:%02d:%02d %ld",
                   list->bids[list->no_bids].UID, &list->bids[list->no_bids].bid_value,
                   year, month, day, hour, minute, second, &list->bids[list->no_bids].bid_sec_time) == 9)
        {

            list->bids[list->no_bids].bid_datetime->tm_year = year;
            list->bids[list->no_bids].bid_datetime->tm_mon = month;
            list->bids[list->no_bids].bid_datetime->tm_mday = day;
            list->bids[list->no_bids].bid_datetime->tm_hour = hour;
            list->bids[list->no_bids].bid_datetime->tm_min = minute;
            list->bids[list->no_bids].bid_datetime->tm_sec = second;
            list->no_bids++;
            fclose(file);
            return 1; // Return 1 to indicate success
        }
    }
    fclose(file);
    return 0; // Return 0 to indicate failure or exceeding bid limit
}

int GetHighestBid(int AID)
{
    // TMB N SEI SE TENHO DE VER O START_VALUE DO AUCTION PRIMEIRO TMB??? N DIZEM ND SOBRE ISSO
    // Assumo q sim
    struct dirent **filelist;
    int nentries, len, highest_bid = 0;
    char dirname[20];
    sprintf(dirname, "AUCTIONS/%03d/BIDS/", AID);
    nentries = scandir(dirname, &filelist, 0, alphasort);
    if (nentries <= 0)
        return 0;
    while (nentries--)
    {
        int len = strlen(filelist[nentries]->d_name);
        if (len == 10)
        {
            int bid_value;
            char bid_value_str[7];
            sscanf(filelist[nentries]->d_name, "%s.txt", bid_value_str);
            bid_value = atoi(bid_value_str);
            if (bid_value > highest_bid)
                highest_bid = bid_value;
        }
        free(filelist[nentries]);
    }
    free(filelist);
    return highest_bid;
}

void DisplayAuctions(AUCTIONLIST *list, char *response)
{
    // Clear the response string before using it
    response[0] = '\0';

    for (int i = 0; i < list->no_aucs; ++i)
    {
        // TODO consideracao de auction ativo ou nao
        int active = CheckAuctionActive(list->aucs[i]);
        if (active == 2)
        {
            active = 0;
            create_end_file(list->aucs[i]);
        }
        // Append the contents of each iteration to the response string
        char temp[100]; // Adjust the size accordingly
        sprintf(temp, " %03d %d", list->aucs[i], active);
        strcat(response, temp);
    }
}

int CheckAuctionActive(int aid)
{
    char end_file_path[30];
    sprintf(end_file_path, "AUCTIONS/%03d/END_%03d.txt", aid, aid);
    if (access(end_file_path, F_OK) != -1)
    {
        // End_File exists
        return 0;
    }
    else
    {
        // End_File does not exist
        // Check if duration of the auction has passed
        long start_fulltime = getAuctionTime(aid);
        int duration = getAuctionDuration(aid);
        time_t end_time;
        time(&end_time);
        double time_passed = (long)end_time - start_fulltime;
        if (time_passed >= duration)
        {
            return 2;
        }
        else
        {
            return 1;
        }
    }
}

char *DisplayRecord(AUCTIONINFO *auc, BIDLIST *list)
{
    // Calculate the size of the response string
    // 6 = start_value, 5 = timeactive, 19 = start_date-time, 1 = '\0', 5 = spaces
    int size = UID_SIZE + strlen(auc->name) + strlen(auc->asset_fname) + 6 + 5 + 19 + 1 + 5;
    if (list != NULL)
    {
        // 3 = ' B ', 6 = bid_value, 19 = bid_date-time, 6 = bid_sec_time, 1 = '\0', 3 = spaces
        size += list->no_bids * (3 + UID_SIZE + 6 + 19 + 6 + 1 + 3);
    }
    if (auc->end_datetime != NULL)
    {
        // 3 = ' E ', 19 = end_date-time, 6 = end_sec_time, 1 = '\0', 1 = spaces
        size += list->no_bids * (3 + 19 + 6 + 1 + 1);
    }

    // Allocate memory for the response string
    char *response = (char *)malloc(size * sizeof(char));

    // Write the auction info to the response string
    sprintf(response, "%s %s %s %d %d %4d-%02d-%02d %02d:%02d:%02d %d",
            auc->uid, auc->name, auc->asset_fname, auc->start_value, auc->timeactive,
            auc->start_datetime->tm_year, auc->start_datetime->tm_mon, auc->start_datetime->tm_mday,
            auc->start_datetime->tm_hour, auc->start_datetime->tm_min, auc->start_datetime->tm_sec, auc->timeactive);

    // Write the bid info to the response string
    if (list != NULL)
    {
        for (int i = 0; i < list->no_bids; ++i)
        {
            if (list->bids[i].bid_datetime != NULL)
            {
                char temp[512];
                sprintf(temp, " B %s %d %4d-%02d-%02d %02d:%02d:%02d %d",
                        list->bids[i].UID, list->bids[i].bid_value,
                        list->bids[i].bid_datetime->tm_year, list->bids[i].bid_datetime->tm_mon, list->bids[i].bid_datetime->tm_mday,
                        list->bids[i].bid_datetime->tm_hour, list->bids[i].bid_datetime->tm_min, list->bids[i].bid_datetime->tm_sec,
                        list->bids[i].bid_sec_time);
                strcat(response, temp);
            }
        }
    }

    if (auc->end_datetime != NULL)
    {
        sprintf(response + strlen(response), " E %4d-%02d-%02d %02d:%02d:%02d %d",
                auc->end_datetime->tm_year, auc->end_datetime->tm_mon, auc->end_datetime->tm_mday,
                auc->end_datetime->tm_hour, auc->end_datetime->tm_min, auc->end_datetime->tm_sec, auc->end_sec_time);
    }
    strcat(response, "\n");
    return response;
}

int GenerateAid()
{
    // LookpUp for the last AID
    struct dirent **filelist;
    int nentries, len;
    char dirname[11];
    char pathname[32];
    sprintf(dirname, "AUCTIONS/");
    nentries = scandir(dirname, &filelist, 0, alphasort);
    if (nentries <= 0)
        return 0;
    int last_aid = 0;
    while (nentries--)
    {
        len = strlen(filelist[nentries]->d_name);
        if (len == 3)
        {
            last_aid++;
        }
        free(filelist[nentries]);
    }
    free(filelist);
    return last_aid + 1;
}

int CheckAuction(USERINFO info, int aid)
{
    if (!auctionExists(aid))
    {
        return 0; // EAU
    }
    if (!isOwner(aid, info.uid))
    {
        return 1; // EOW
    }
    // Auction active or not
    if (isAuctionEnded(aid))
    {
        return 2; // END
    }
    return 3; // OK
}

int auctionExists(int AID)
{
    DIR *dir;
    char dirname[20];
    // Create the directory name for the auction
    sprintf(dirname, "AUCTIONS/%03d", AID);
    dir = opendir(dirname);
    if (dir)
    {
        // Directory exists
        closedir(dir);
        return 1;
    }
    else
    {
        // Directory does not exist
        return 0;
    }
}

int isOwner(int aid, char uid[])
{
    // TODO mudar dps pa lookup ao uid
    char uid_test[UID_SIZE];
    getAuctionUID(aid, uid_test);
    // Check if the user is the owner of the auction
    if (strcmp(uid, uid_test) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// Check auction active or not
int isAuctionEnded(int aid)
{
    // Check for End file
    // CUIDADO CHECKAR TMB SE O TEMPO NAO EXPIROU !!!
    struct dirent **entrylist;
    char dirname[15];
    sprintf(dirname, "AUCTIONS/%03d", aid);
    int n_entries = scandir(dirname, &entrylist, NULL, alphasort);
    if (n_entries < 0)
    {
        perror("scandir");
        return 0;
    }
    // Care here on the loop for later should work for now
    int end_file_found = 0;
    char end_file[19];
    sprintf(end_file, "END_%03d.txt", aid);
    while (n_entries--)
    {
        if (entrylist[n_entries]->d_type == DT_REG && strcmp(entrylist[n_entries]->d_name, end_file) == 0)
        {
            end_file_found = 1;
            free(entrylist[n_entries]); // Free memory for each entry
            break;
        }
        free(entrylist[n_entries]); // Free memory for each entry
    }
    free(entrylist); // Free the entryList array
    return end_file_found;
}

void getAuctionUID(int aid, char uid[])
{
    char filename[30];
    sprintf(filename, "AUCTIONS/%03d/START_%03d.txt", aid, aid);
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error opening start file");
        return NULL;
    }
    fscanf(file, "%s ", uid);
    fclose(file);
    return uid;
}

long getAuctionTime(int aid)
{
    char filename[30];
    sprintf(filename, "AUCTIONS/%03d/START_%03d.txt", aid, aid);
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        return 0;
    }
    long start_fulltime;
    fscanf(file, "%*s %*s %*s %*d %*d %*d-%*d-%*d %*d:%*d:%*d %ld", &start_fulltime);
    fclose(file);
    return start_fulltime;
}

int getAuctionDuration(int aid)
{
    char filename[30];
    sprintf(filename, "AUCTIONS/%03d/START_%03d.txt", aid, aid);
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        return 0;
    }
    int duration;
    fscanf(file, "%*s %*s %*s %*d %d", &duration);
    fclose(file);
    return duration;
}

int getAuctionStartValue(int aid)
{
    char filename[30];
    sprintf(filename, "AUCTIONS/%03d/START_%03d.txt", aid, aid);
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        return 0;
    }
    int start_value;
    fscanf(file, "%*s %*s %*s %d ", &start_value);
    fclose(file);
    return start_value;
}

int check_auction_name(char auction_name[])
{
    int auction_name_length = calculate_str_length(auction_name);
    if (auction_name_length > 10)
    {
        printf("Invalid input: auction size wrong.\n");
        return 0;
    }
    for (size_t i = 0; i < strlen(auction_name); i++)
    {
        if (!isdigit(auction_name[i]) && !isalpha(auction_name[i]))
        {
            printf("Invalid input: auctions not alphanumerical.\n");
            return 0;
        }
    }
    return 1;
}

int check_file_name(char file_name[])
{
    // TODO TAS SO A CHECKAR NA DIRETORIA TENS DE CHECKAR NO PC EM GERAL E DEVOLVER O REAL PATH ACHO EU MAS YAH
    FILE *file_readed;
    file_readed = fopen(file_name, "r");
    if (file_readed == NULL)
    {
        printf("File not found.\n");
        return 0;
    }
    return 1;
}

char *read_file_data(char *file_name)
{
    FILE *file = fopen(file_name, "r");
    if (file != NULL)
    {
        // Find the size of the file
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        // Allocate memory for the file content
        char *data = (char *)malloc(file_size + 1);
        if (data != NULL)
        {
            // Read the entire file
            size_t read_size = fread(data, 1, file_size, file);

            // Null-terminate the string
            data[read_size] = '\0';

            fclose(file);
            return data;
        }
        fclose(file);
    }
    return "No data.";
}

int get_file_size(char file_name[])
{
    FILE *file_readed;
    file_readed = fopen(file_name, "r");
    if (file_readed == NULL)
    {
        printf("File not found.\n");
        return 0;
    }
    fseek(file_readed, 0, SEEK_END);
    long file_size = ftell(file_readed);
    fseek(file_readed, 0, SEEK_SET);
    fclose(file_readed);
    return file_size;
}

int check_start_value(char start_value[])
{
    if (strlen(start_value) > 6)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int check_time_active_input(char time_active[])
{
    if (strlen(time_active) > 5)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int check_open_credentials(char auction_name[], char file_name[], char start_value[], char time_active[])
{
    int check = 1;
    check = check_auction_name(auction_name);
    if (check == 0)
        return check;
    check = check_file_name(file_name);
    if (check == 0)
        return check;
    check = check_start_value(start_value);
    if (check == 0)
        return check;
    check = check_time_active_input(time_active);
    return check;
}

int verify_file_name(char file_name[])
{
    if (strlen(file_name) > 32)
    {
        return 0;
    }

    for (size_t i = 0; i < strlen(file_name); i++)
    {
        if (!isdigit(file_name[i]) && !isalpha(file_name[i]) && file_name[i] != '.' && file_name[i] != '_' && file_name[i] != '-')
        {
            return 0;
        }
    }

    if (file_name[strlen(file_name) - 4] != '.') // Verify if has the .txt extension
    {
        return 0;
    }
    {
        return 0;
    }

    for (size_t i = strlen(file_name) - 1; i < strlen(file_name); i++) // Verify if has the .txt extension
    {
        if (!isalpha(file_name[i]))
        {
            return 0;
        }
    }
}

int verify_bid_value(char bid_value[])
{
    if (strlen(bid_value) > 6)
    {
        return 0;
    }

    for (size_t i = 0; i < strlen(bid_value); i++)
    {
        if (!isdigit(bid_value[i]))
        {
            return 0;
        }
    }
    return 1;
}