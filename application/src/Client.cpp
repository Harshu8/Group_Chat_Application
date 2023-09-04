#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include "ClientDetail.h"
#include "Constant.h"
#include "InvalidIpAddress.h"
#include "Client.h"

ClientSocket::ClientSocket(int portNumber, struct hostent *ipAddress)
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (ipAddress == NULL)
    {
        throw InvalidIpAddress();
    }
    bzero((char *)&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    bcopy((char *)ipAddress->h_addr, (char *)&serverAddress.sin_addr.s_addr, ipAddress->h_length);
    serverAddress.sin_port = htons(portNumber);
}
int ClientSocket::getSocketFD()
{
    return sockfd;
}
int ClientSocket::connectToSocket()
{
    int isConnect = connect(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    return isConnect;
}
void ClientSocket::closeSocket()
{
    close(sockfd);
}
