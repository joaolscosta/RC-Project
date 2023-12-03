#ifndef ACTIONS_H
#define ACTIONS_H

#define UID_SIZE 6
#define PASS_SIZE 8

typedef struct User
{
    char uid[UID_SIZE + 1];
    char password[PASS_SIZE + 1];
} User;

typedef struct Auction
{
    char name[11]; // Define 11
    // asset(image)
    float start_value;
    // Time duration
} Auction;

// Verifications
int verify_user_credentials(User user);
int verify_UID(char buffer[]);
int verify_AID(char buffer[]);
// UDP
int login_user(char buffer[]);
int logout_user(char buffer[]);
int unregister_user(char buffer[]);
int myactions_user(char buffer[]);
int mybids_user(char buffer[]);
int list_user(char buffer[]);
int show_record_user(char buffer[]);
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
void create_user_folder(User user);
void create_pass_file(User user);
void create_login_file(User user);
void create_hosted_folder(User user);
void create_bidded_folder(User user);
// void create_hosted_auction_file(User user, Auction auc);
// void create_bidded_auction_file(User user, Auction auc);
//   Auctions
/*
void create_auction_folder(Auction auc);
void create_start_file(Auction auc);
void create_asset(Auction auc);
void create_end_file(Auction auc);
void create_
 */
// LookUps

/**/
#endif