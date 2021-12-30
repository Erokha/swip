#include "ClientDescriptior/ClientDescriptor.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"


int login(int client_sockfd, struct user_description *, struct user_description *);
int create_user_and_login(int client_sockfd, struct user_description *, struct user_description *);
void send_status(int client_sockfd, int status_code, char *status_msg);
void save_mail_to_database(int client_sockfd);
void handle_user_request_to_view_mails(int client_sockfd, struct user_description *user);


int handle_client(int client_sockfd) {
    int option, is_logged_in;
    ClientDescriptor cli = ClientDescriptor(client_sockfd);

    while (TRUE) {
        if (recv(client_sockfd, &option, sizeof(option), 0) == 0) {
            return FALSE;
        }

        switch (option) {
            case 1:
                cli.process_login();
                break;

            case 2:
                cli.process_register();
                break;

            default:
                break;
        }

        if(cli.is_authorized == FALSE) {
            continue;
        }

        while (TRUE) {
            // receive option form client
            if (recv(client_sockfd, &option, sizeof(option), 0) == 0) {
                return FALSE;
            }

            switch (option) {
                case 1:
                    handle_user_request_to_view_mails(client_sockfd, &cli.client_desc);
                    break;

                case 2:
                    save_mail_to_database(client_sockfd);
                    break;

                case 3:
                    break;

                default:
                    close(client_sockfd);
                    return 1;
            }

            if(option == 3) break;
        }
        close(client_sockfd);
    }
    close(client_sockfd);
}


int main(int argc, char *argv[]) {
    struct  sockaddr_in server_addr, client_addr;
    int     sockfd, client_sockfd, port, client_len;

    if(argc != 3) {
        fprintf(stderr, "Invalid Arguments\n");
        fprintf(stderr, "./server -p <port number>\n");
        return -1;
    }

    port = atoi(argv[2]);
    client_len = sizeof(client_addr);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror("socket");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if ( bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0 ) {
        perror("bind");
        return -1;
    }

    if ( listen(sockfd, 5) < 0) {
        perror("listen");
        return -1;
    }

    while(TRUE) {

        if ((client_sockfd = accept(sockfd, (struct sockaddr *) &client_addr, reinterpret_cast<socklen_t *>(&client_len))) < 0) {
            perror("accept");
            continue;
        }

        std::thread thread1(handle_client, client_sockfd);
        thread1.detach();
    }
}

void save_mail_to_database(int client_sockfd)
{
    struct  mail mail;
    struct  user_description user;
    FILE    *mails_dat, *users_dat;
    int     found = FALSE;

    recv(client_sockfd, &mail, sizeof(mail), 0);

    users_dat = fopen(USERS_DAT_FILE, "rb");
    mails_dat = fopen(MAILS_DAT_FILE, "ab+");

    while (!feof(users_dat)) {
        fread(&user, sizeof(struct user_description), 1, users_dat);
        if ( strcmp(user.email, mail.to) == 0 ) {
            found = TRUE;
            break;
        }
    }

    if (!found) {
        send_status(client_sockfd, 404, "To Address is not exist");
        fclose(users_dat);
        fclose(mails_dat);
        return;
    }

    fwrite(&mail, sizeof(struct mail), 1, mails_dat);

    perror("fwrite");

    send_status(client_sockfd, 200, "Successfully Send");

    fclose(users_dat);
    fclose(mails_dat);
}


void handle_user_request_to_view_mails(int client_sockfd, struct user_description *user) {
    struct inbox inbox;
    struct mail temp;
    int    i = 0;
    FILE   *mails_dat;

    mails_dat = fopen(MAILS_DAT_FILE, "rb");

    while(!feof(mails_dat)){
        fread(&temp, sizeof(struct mail), 1, mails_dat);
        if( strcmp(temp.to, user->email) == 0 ) {
            inbox.mails[i++]= temp;
        }
    }

    inbox.count = i - 1;

    send(client_sockfd, &inbox, sizeof(struct inbox), 0);

    fclose(mails_dat);
}

#pragma clang diagnostic pop