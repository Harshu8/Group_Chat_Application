#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <cstring>
#include "Server.h"
#include "ClientDetail.h"
#include "Constant.h"
#include "ReadingError.h"
#include "WritingError.h"
#include "PortNotProvided.h"

pthread_mutex_t clientsMutex = PTHREAD_MUTEX_INITIALIZER;
ClientDetailClass *clients[maxClients];

void signal_callback_handler(int signum) 
{
   int errorCode=404;
   for (int index = 0; index < maxClients; ++index)
    {
        if(clients[index])
        {
            write(clients[index]->sockfd, &errorCode, sizeof(errorCode));
        }      
    }
    exit(signum);
}

void addClientsToQueue(ClientDetailClass *clientDetails)
{
    pthread_mutex_lock(&clientsMutex);
    for (int index = 0; index < maxClients; ++index)
    {
        if (!clients[index])
        {
            clients[index] = clientDetails;
            break;
        }
    }
    pthread_mutex_unlock(&clientsMutex);
}

void removeClientFromQueue(int uid)
{
    pthread_mutex_lock(&clientsMutex);
    for (int index = 0; index < maxClients; ++index)
    {
        if (clients[index])
        {
            if (clients[index]->uid == uid)
            {
                clients[index] = NULL;
                break;
            }
        }
    }
    pthread_mutex_unlock(&clientsMutex);
}

void sendMessageToOtherClients(char *message, int uid)
{
    int isFirstWrite, isSecondWrite;
    pthread_mutex_lock(&clientsMutex);
    for (int index = 0; index < maxClients; ++index)
    {
        if (clients[index] && clients[index]->uid != uid)
        {
            int sizeOfMsg = strlen(message);
            isFirstWrite = write(clients[index]->sockfd, &sizeOfMsg, sizeof(sizeOfMsg));
            isSecondWrite = write(clients[index]->sockfd, message, strlen(message));
            if (isFirstWrite < 0 || isSecondWrite < 0)
            {
                throw WritingError();
            }
        }
    }
    pthread_mutex_unlock(&clientsMutex);
}

void *handleClient(void *clientDetailArgs)
{
    int isRead, isWrite;
    clientCount++;
    ClientDetailClass *clientDetails = (ClientDetailClass *)clientDetailArgs;

    int lengthOfName;
    read(clientDetails->sockfd, &lengthOfName, sizeof(lengthOfName));
    if (isRead < 0)
    {
        throw ReadingError();
    }
    char* name = new char;
    isRead = read(clientDetails->sockfd, name, lengthOfName);
    if (isRead < 0)
    {
        throw ReadingError();
    }

    char* bufferForJoiningMessage = new char;
    strcpy(clientDetails->name, name);
    sprintf(bufferForJoiningMessage, "%s has joined", clientDetails->name);
    std::cout << bufferForJoiningMessage << std::endl;

    sendMessageToOtherClients(bufferForJoiningMessage, clientDetails->uid);
    bzero(bufferForJoiningMessage, strlen(bufferForJoiningMessage));

    while (1)
    {
        int sizeOfBuf=0;
        isRead = read(clientDetails->sockfd, &sizeOfBuf, sizeof(sizeOfBuf));
        if (isRead < 0)
        {
            throw ReadingError();
        }
        char* buffer = new char;
        bzero(buffer, sizeOfBuf);

        isRead = read(clientDetails->sockfd, buffer, sizeOfBuf);
        if (isRead < 0)
        {
            throw ReadingError();
            pthread_exit(NULL);
        }
        int isExit = strncmp("exit", buffer, 4);
        if (isExit != 0 && strlen(buffer) > 0)
        {
            if( clientCount == 1 )
            {
                std::string msg ="Wait, there is no one to chat.\n";
                int sizeOfMSg = msg.size();
                isWrite = write(clientDetails->sockfd, &sizeOfMSg, sizeof(sizeOfMSg));                
                if(isWrite<0)
                {
                    throw WritingError();
                }
                isWrite = write(clientDetails->sockfd, msg.c_str(), sizeOfMSg);
                if(isWrite<0)
                {
                    throw WritingError();
                }                
            }
            sendMessageToOtherClients(buffer, clientDetails->uid);
        }
        else if (isExit == 0)
        {
            sprintf(buffer, "%s has left", clientDetails->name);
            std::cout << buffer << std::endl;
            sendMessageToOtherClients(buffer, clientDetails->uid);
            break;
        }
    }
    close(clientDetails->sockfd);
    removeClientFromQueue(clientDetails->uid);
    free(clientDetails);
    clientCount--;
    pthread_detach(pthread_self());
    return NULL;
}

int main(int argc, char *argv[])
{
    try
    {
        signal(SIGINT, signal_callback_handler);
        int portNumber;
        struct sockaddr_in serverAddress;
        pthread_t threadId;
        if (argc < 2)
        {
            throw PortNotProvided();
        }

        portNumber = atoi(argv[1]);
        ServerSocket newSocketObject(portNumber);
        newSocketObject.bindSocket();
        newSocketObject.listenSocket();

        int sockfd = newSocketObject.getSocketFD();
        std::cout << "\nChat Begins: \n\n";
        while (1)
        {
            ClientDetailClass *clientDetails = newSocketObject.acceptConnection();
            addClientsToQueue(clientDetails);
            pthread_create(&threadId, NULL, &handleClient, (void *)clientDetails);
            if ((clientCount + 1) == 10)
            {
                std::cout << "Max clients reached. Rejected: ";
                close(clientDetails->sockfd);
                continue;
            }
        }
    }
    catch(ReadingError &error)
    {
        std::cout << error.what() << '\n';
    }
    catch(WritingError &error)
    {
        std::cout << error.what() << '\n';
    }
    catch(PortNotProvided &error)
    {
        std::cout << error.what() << '\n';
    }
    return 0;
}