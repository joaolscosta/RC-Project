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

typedef struct User
{
    char uid[UID_SIZE];
    char password[PASS_SIZE];
} User;

typedef struct Auction
{
    int aid;
    char name[11];         // Define 11
    char asset_fname[255]; // Define 255 TODO MUDAR DPS
    int start_value;
    time_t duration;
    struct tm *start_datetime;
    time_t start_fulltime;
} Auction;

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
int logout_user(char uid[], char pass[]);
int unregister_user(char uid[], char pass[]);
// int myactions_user(char buffer[]);
// int mybids_user(char buffer[]);
// int list_all_auctions();
// int show_record_user(char buffer[]);
//  TCP
/*
int open(char buffer[]);
int close(char buffer[]);
int show_asset(char buffer[]);
int bid(char buffer[]);
*/
// LOCAL
// int exit(char buffer[]);
// Server
//  Users
int create_user_folder(char uid[], char pass[]);
int remove_user_folder(char uid[], char pass[]);
int create_pass_file(char uid[], char pass[]);
int create_login_file(char uid[]);
int delete_login_file(char uid[]);
int create_hosted_folder(char uid[]);
int create_bidded_folder(char uid[]);
int create_hosted_auction_file(User user, Auction auc);
int create_bidded_auction_file(User user, Auction auc);
//   Auctions
int create_auction_folder(int aid);
int create_start_file(Auction auc, User user);
int create_asset_folder(int aid);
int create_asset(Auction auc);
int create_end_file(Auction auc);
int create_bids_folder(int aid);
int create_bid_file(Auction auc, User user, int bid_value);
// LookUps
int check_user(char uid[]);
int check_asset_file(char *fname);
char get_auctions_list(char uid[]);
char get_all_auctions();
int GetBidList(int AID, BIDLIST *list);
int LoadBid(const char *filepath, BIDLIST *list);
void show_auctions_list(char buffer[]);
int check_auction_name(char auction_name[]);
int check_file_name(char file_name[]);
int check_start_value(int start_value);
int check_time_active_input(int time_active);
int check_open_credentials(char auction_name[], char file_name[], int start_value, int time_active);

/**/
#endif