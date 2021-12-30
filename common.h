#ifndef COURSE_NETS_COMMON_H
#define COURSE_NETS_COMMON_H

struct user_description {
    char email[50];
    char password[50];
};

struct status_message {
    int status_code;
    char status_msg[50];
};

struct mail {
    char from[50];
    char to[50];
    char subject[50];
    char body[100];
};

struct inbox {
    int count;
    struct mail mails[100];
};

#define TRUE 1
#define FALSE 0

#define USERS_DAT_FILE "/Users/erokha/swip/users.dat"
#define MAILS_DAT_FILE "/Users/erokha/swip/mails.dat"

#endif //COURSE_NETS_COMMON_H
