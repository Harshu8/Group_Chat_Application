#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "Constant.h"
#include "BindingFailed.h"
#include "Server.h"

ServerSocket::ServerSocket(int portNumber)
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    std::memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(portNumber);
}

int ServerSocket::getSocketFD()
{
    return sockfd;
}

struct sockaddr_in ServerSocket::getAddress()
{
    return serverAddress;
}

int ServerSocket::bindSocket()
{
    int isBind = bind(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    return isBind;
}

int ServerSocket::listenSocket()
{
    int isListen = listen(sockfd, 10);
    return isListen;
}

ClientDetailClass *ServerSocket::acceptConnection()
{
    struct sockaddr_in clientAddress;
    socklen_t clientSocketLength = sizeof(clientAddress);
    int newsockfd = accept(sockfd, (struct sockaddr *)&clientAddress, &clientSocketLength);
    ClientDetailClass *clientDetails = (ClientDetailClass *)malloc(sizeof(ClientDetailClass));
    clientDetails->address = clientAddress;
    clientDetails->sockfd = newsockfd;
    clientDetails->uid = uid++;
    return clientDetails;
}

void ServerSocket::closeSocket()
{
    close(sockfd);
}