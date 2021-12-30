#include "common.h"
#include "common_includes.h"



void login_or_create(int *option);
void get_credintials(struct user_description *);
void send_cred_to_server(int sockfd, struct user_description *);
void get_user_info(struct user_description *);
void send_user_info(int sockfd, struct user_description *);
int  status(int sockfd);
void show_main_menu(int *option);
void compose_mail(int sockfd, char *from);
void show_inbox(int sockfd);

int main(int argc, char *argv[])
{
    struct  sockaddr_in server_addr;
    struct  user_description auth_cred;
    struct  user_description user;
    int     sockfd, port, option, is_logged_in, is_socket_connected = FALSE;

    if(argc != 3) {
        fprintf(stderr, "Invalid Arguments\n");
        fprintf(stderr, "./server -p <port number>\n");
        return -1;
    }

    port = atoi(argv[2]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror("socket");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(port);

    if ( connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0 ) {
        perror("connect");
        return -1;
    } else {
        is_socket_connected = TRUE;
    }


    while(is_socket_connected) {
        login_or_create(&option);
        send(sockfd, &option, sizeof(option), 0);
        switch (option) {
            case 1:
                get_credintials(&auth_cred);
                send_cred_to_server(sockfd, &auth_cred);
                is_logged_in = status(sockfd);
                break;

            case 2:
                get_user_info(&user);
                send_user_info(sockfd, &user);
                is_logged_in = status(sockfd);
                strcpy(auth_cred.email, user.email);
                strcpy(auth_cred.password, user.password);
                break;

            default:
                printf("Please choose an option !!\n");
        }

        while (is_logged_in == TRUE) {
            show_main_menu(&option);

            // send option to server
            send(sockfd, &option, sizeof(option), 0);

            switch (option)
            {
                case 1:
                    show_inbox(sockfd);
                    break;

                case 2:
                    compose_mail(sockfd, auth_cred.email);
                    break;

                case 3:
                    is_logged_in = 0;
                    is_socket_connected = FALSE;
                    break;

                default:
                    printf("Please choose an option !!\n");
            }
        }
    }

    close(sockfd);
    return 1;
}

void login_or_create(int *option) {
    printf("\n\n_______________________START MENU_____________________\n\n");
    printf("1. LOGIN \n");
    printf("2. CREATE ACCOUNT \n");
    printf("Choose an option => ");
    scanf("%d", option);
}

void get_credintials(struct user_description *auth_cred) {
    printf("EMAIL => ");
    scanf("%s", auth_cred->email);
    printf("PASSWORD => ");
    scanf("%s", auth_cred->password);
}

void send_cred_to_server(int sockfd, struct user_description *auth_cred) {
    send(sockfd, auth_cred, sizeof(*auth_cred), 0);
}

void get_user_info(struct user_description *user) {
    printf("ENTER EMAIL ADDRESS => ");
    scanf("%s", user->email);
    printf("ENTER PASSWORD => ");
    scanf("%s", user->password);
}

void send_user_info(int sockfd, struct user_description *user) {
    send(sockfd, user, sizeof(*user), 0);
}

int status(int sockfd) {
    struct status_message status;
    recv(sockfd, &status, sizeof(status), 0);

    printf("%s\n", status.status_msg);

    if(status.status_code == 500 || status.status_code == 403) {
        return FALSE;
    }

    return TRUE;
}

void show_main_menu(int *option) {
    printf("\n\n____________________MAIN MENU_________________\n\n");
    printf("1. inbox\n");
    printf("2. COMPOSE\n");
    printf("3. LOGOUT\n");
    printf("CHOOSE OPTION => ");
    scanf("%d", option);
}

void compose_mail(int sockfd, char *from) {
    struct mail mail;
    strcpy(mail.from, from);

    printf("\n\n____________________COMPOSE MAIL_________________\n\n");
    printf("FROM : %s\n", from);
    printf("TO => ");
    scanf("%s", mail.to);

    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    printf("SUBJECT => ");
    fgets(mail.subject, sizeof(mail.subject), stdin);

    printf("BODY => ");
    fgets(mail.body, sizeof(mail.body), stdin);

    // send mail to server
    send(sockfd, &mail, sizeof(mail), 0);

    // waiting for server to respond with a status
    status(sockfd);

}


void show_inbox(int sockfd) {
    struct inbox inbox;
    int    i = 0;

    recv(sockfd, &inbox, sizeof(struct inbox), 0);

    for(i = 0; i < inbox.count; i++) {
        printf("\n___________________________________________________________\n\n");
        printf("From : %s\n", inbox.mails[i].from);
        printf("To : %s\n", inbox.mails[i].to);
        printf("Subject : %s\n", inbox.mails[i].subject);
        printf("%s\n", inbox.mails[i].body);
        printf("\n___________________________________________________________\n");
    }

}