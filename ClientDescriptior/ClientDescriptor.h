#ifndef COURSE_NETS_CLIENTDESCRIPTOR_H
#define COURSE_NETS_CLIENTDESCRIPTOR_H
#include "../common_includes.h"
#include "../common.h"

class ClientDescriptor {
private:
    int socket;
    void disconnect();
public:
    ClientDescriptor(int socket);
    int process_login();
    int process_register();
    struct user_description client_desc;
    int is_authorized;
};

#endif //COURSE_NETS_CLIENTDESCRIPTOR_H
