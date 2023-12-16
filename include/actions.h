#ifndef ACTIONS_H
#define ACTIONS_H

#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <sys/select.h>
#include <dirent.h>

#define UID_SIZE 7
#define AID_SIZE 4
#define PASS_SIZE 9
#define AUCTION_NAME_SIZE 11
#define FILE_NAME_SIZE 25 // 24 + /0
#define BID_VALUE_SIZE 7  // 6 + /0

typedef struct USERINFO
{
    char uid[UID_SIZE];
    char password[PASS_SIZE];
} USERINFO;

typedef struct FILEINFO
{
    char file_name[FILE_NAME_SIZE];
    int file_size;
} FILEINFO;

typedef struct AUCTIONINFO
{
    // START
    char uid[UID_SIZE];               // owner
    char name[AUCTION_NAME_SIZE];     // Define 11
    char asset_fname[FILE_NAME_SIZE]; // Define 255 TODO MUDAR DPS
    int start_value;                  // Up to 6 digits
    int timeactive;                   // represented up to 5 digits
    struct tm *start_datetime;
    time_t start_fulltime;
    // END
    struct tm *end_datetime; // TODO APAGAR ACHO Q N USO
    int end_sec_time;        // TODO APAGAR ACHO Q N USO
} AUCTIONINFO;

typedef struct AUCTIONLIST
{
    int no_aucs;
    int aucs[50]; // aids
} AUCTIONLIST;

typedef struct BIDINFO
{
    char UID[UID_SIZE];
    int bid_value;
    struct tm *bid_datetime;
    time_t bid_sec_time;
} BIDINFO;

typedef struct
{
    int no_bids;
    BIDINFO bids[50];
} BIDLIST;

// Verifications
int verify_user_credentials(char uid[], char pass[]);
int verify_UID(char uid[]);
int verify_AID(char aid[]);
int calculate_str_length(char buffer[]);
// UDP
int login_user(char uid[], char pass[]);
int logout_user(char uid[]);
int unregister_user(char uid[]);
int myauctions_user(char uid[], AUCTIONLIST *list);
int mybids_user(char uid[], AUCTIONLIST *list);
int list_all_auctions(AUCTIONLIST *list);
int show_record_user(int aid, AUCTIONINFO *auc, BIDLIST *list);
//  TCP
int open_auction(AUCTIONINFO info, FILEINFO file, char *file_data, int *aid);
int close_auction(USERINFO info, int aid);
int show_asset(int aid, FILEINFO *file, char **file_data);
int bid(USERINFO info, int aid, int bid_value);
// LOCAL
// int exit(char buffer[]);
// Server
//  Users
int create_user_folder(char uid[], char pass[]);
int create_pass_file(char uid[], char pass[]);
int create_login_file(char uid[]);
int delete_login_file(char uid[]);
int delete_pass_file(char uid[]);
int create_hosted_folder(char uid[]);
int create_bidded_folder(char uid[]);
int create_hosted_auction_file(char uid[], int aid);
int create_bidded_auction_file(char uid[], int aid);
//   Auctions
int create_auction_folder(int aid, AUCTIONINFO auc, FILEINFO file, char *file_data);
int create_start_file(int aid, AUCTIONINFO auc);
int create_asset_folder(int aid);
int create_asset(AUCTIONINFO auc);
int create_end_file(int aid);
int create_bids_folder(int aid);
int create_bid_file(char uid[], int aid, int bid_value);
// LookUps
int LookUpUser(char uid[]);
int LookUpUserPassword(char uid[], char pass[]);
int LookUpUserLogin(char uid[]);
int LookUpAssetFile(int aid, FILEINFO *file, char **file_data);
int LookUpAuction(int aid, AUCTIONINFO *auc, BIDLIST *list);
int GetHostedAuctionlist(char uid[], AUCTIONLIST *list);
int LoadAuction(const char *filepath, AUCTIONLIST *list);
int GetBidList(int AID, BIDLIST *list);
int GetBiddedAuctionlist(char uid[], AUCTIONLIST *list);
int GetBidList(int AID, BIDLIST *list);
int GetAuctionlist(AUCTIONLIST *list);
int LoadBid(const char *filepath, BIDLIST *list);
void DisplayAuctions(AUCTIONLIST *list, char *response);
char *DisplayRecord(AUCTIONINFO *auc, BIDLIST *list);
AUCTIONINFO getAuction(int aid);
// CHECKS NOT LOOKUPS
int check_auction_name(char auction_name[]);
int check_file_name(char file_name[]);
char *read_file_data(char file_name[]);
int get_file_size(char file_name[]);
int check_start_value(char start_value[]);
int check_time_active_input(char time_active[]);
int check_open_credentials(char auction_name[], char file_name[], char start_value[], char time_active[]);
int verify_file_name(char file_name[]);

/**/
#endif