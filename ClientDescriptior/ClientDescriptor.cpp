#include "ClientDescriptor.h"

void send_status(int client_sockfd, int status_code, char *status_msg) {
    struct status_message status;
    status.status_code = status_code;
    strcpy(status.status_msg, status_msg);
    send(client_sockfd, &status, sizeof(status), 0);
}


ClientDescriptor::ClientDescriptor(int socket) {
    this->socket = socket;
    this->is_authorized = FALSE;
}

int ClientDescriptor::process_login() {
    FILE   *users_dat;
    struct user_description *user, *auth_cred;
    recv(this->socket, auth_cred, sizeof(*auth_cred), 0);
    users_dat = fopen(USERS_DAT_FILE, "rb");

    if(users_dat == NULL) {
        send_status(this->socket, 500, "NO user_description EXISTS!!");
        return FALSE;
    }

    while (!feof(users_dat)) {
        fread(&user, sizeof(struct user_description), 1, users_dat);

        if ( strcmp(user->email, auth_cred->email) == 0) {
            if ( strcmp(user->password, auth_cred->password) == 0) {
                send_status(this->socket, 200, "Logged In Successfully");
                fclose(users_dat);
                this->is_authorized = TRUE;
                return TRUE;
            } else {
                send_status(this->socket, 403, "Password Is Incorrect");
                fclose(users_dat);
                return FALSE;
            }
        }
    }

    send_status(this->socket, 403, "Invalid Email And Password");
    fclose(users_dat);
    return FALSE;
}

int ClientDescriptor::process_register() {
    FILE   *users_dat;
    struct user_description *user, *user_check;

    recv(this->socket, user, sizeof(user), 0);

    users_dat = fopen(USERS_DAT_FILE, "rb");
    if(users_dat == NULL) {
        send_status(this->socket, 500, "Internal Server Error");
        fclose(users_dat);
        return FALSE;
    }

    while (!feof(users_dat)) {
        fread(user_check, sizeof(struct user_description), 1, users_dat);

        if (strcmp(user->email, user_check->email) == 0) {
            send_status(this->socket, 403, "Email already exist");
            fclose(users_dat);
            return FALSE;
        }
    }
    fclose(users_dat);
    users_dat = fopen(USERS_DAT_FILE, "ab");
    fwrite(user, sizeof(struct user_description), 1, users_dat);
    send_status(this->socket, 200, "OK");
    fclose(users_dat);

    this->client_desc = *user;
    return TRUE;
}

void ClientDescriptor::disconnect() {
    close(this->socket);
}

