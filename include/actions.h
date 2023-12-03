#ifndef ACTIONS_H
#define ACTIONS_H

// Verifications
int verify_user_credentials(char buffer[]);
int verify_UID(char buffer[]);
int verify_AID(char buffer[]);
// UDP
int login_user(char buffer[]);
int logout_user(char buffer[]);
//int unregister_user(char buffer[]);
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
// LOCAL
int exit(char buffer[]);
*/
#endif