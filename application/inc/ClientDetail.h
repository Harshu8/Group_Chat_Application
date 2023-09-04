#pragma once
#include <netinet/in.h>

class ClientDetailClass
{
    public:
    struct sockaddr_in address;
    int sockfd;
    int uid;
    char name[30];
};