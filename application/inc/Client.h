#pragma once
#include <netdb.h>
#include <netinet/in.h>

class ClientSocket
{
 private:
    int sockfd;
    struct sockaddr_in serverAddress;
public:
    ClientSocket(int portNumber, struct hostent *ipAddress);
    int getSocketFD();
    int connectToSocket();
    void closeSocket();
};