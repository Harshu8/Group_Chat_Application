#pragma once
#include <netinet/in.h>
#include "ClientDetail.h"

class ServerSocket
{
private:
    int sockfd;
    struct sockaddr_in serverAddress;
public:
    ServerSocket(int portNumber);
    int getSocketFD();
    struct sockaddr_in getAddress();
    int bindSocket();
    int listenSocket();
    ClientDetailClass *acceptConnection();
    void closeSocket();
};