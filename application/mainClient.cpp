#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <cstring>
#include "Client.h"
#include "InvalidIpAddress.h"
#include "Constant.h"
#include "ServerClosed.h"
#include "ReadingError.h"
#include "WritingError.h"
#include "PortNotProvided.h"

pthread_t thread2;
char *name = new char;

void trimNewLineCharacterFromString(char *message)
{
    char *firstNewLineCharacter = strchr(message, '\n');
    if (firstNewLineCharacter)
    {
        *firstNewLineCharacter = '\0';
    }
}

void *sendMessage(void *args)
{
    int *sockFD = (int *)args;
    char *buffer = new char;
    std::string message;
    while (1)
    {
        bzero(strdup(message.c_str()), message.size());
        getline(std::cin, message);
        trimNewLineCharacterFromString(strdup(message.c_str()));
        if (strncmp("exit", message.c_str(), 4) == 0)
        {
            int sizeOfExit = 4;
            int a = write(*sockFD, &sizeOfExit, sizeof(sizeOfExit));
            if (a < 0)
            {
                throw WritingError();
            }
            int b = write(*sockFD, message.c_str(), sizeOfExit);
            if (b < 0)
            {
                throw WritingError();
            }
            pthread_cancel(thread2);
            pthread_exit(NULL);
            break;
        }

        sprintf(buffer, "%s : %s", name, message.c_str());

        int sizeOfBuffer = strlen(buffer);
        int m = write(*sockFD, &sizeOfBuffer, sizeof(sizeOfBuffer));
        if (m < 0)
        {
            throw WritingError();
        }

        int n = write(*sockFD, buffer, strlen(buffer));
        if (n < 0)
        {
            throw WritingError();
        }
        else
        {
            std::cout << "Message delivered successfully to all.\n";
        }
    }
}

void *receiveMessage(void *arg)
{
    int isRead;
    int *sockFD = (int *)arg;
    thread2 = pthread_self();
    while (1)
    {
        int sizeOfMsg;
        isRead = read(*sockFD, &sizeOfMsg, sizeof(sizeOfMsg));
        if( sizeOfMsg == 404 )
        {
            std::cout<<"Server is closed. Error Code:" <<sizeOfMsg<<"\n";
            close(*sockFD);
            return NULL;
        }
        if (isRead < 0)
        {
            throw ReadingError();
        }
        char *message = new char;
        isRead = read(*sockFD, message, sizeOfMsg);
        if (isRead < 0)
        {
            throw ReadingError();
        }
        std::cout << message << "\n";
    }
}

int main(int argc, char *argv[])
{
    try
    {
        int portno, n;
        struct hostent *server;
        if (argc < 3)
        {
            throw PortNotProvided();
        }
        portno = atoi(argv[2]);
        server = gethostbyname(argv[1]);
        if (server == NULL)
        {
            throw InvalidIpAddress();
        }

        ClientSocket clientSock(portno, server);

        if (clientSock.connectToSocket() < 0)
        {
            throw ServerClosed();
        }
        else
        {
            std::cout << "\nEntered Chat Room successfully\n";
        }

        std::cout << "Enter your username : ";
        fgets(name, nameSize, stdin);
        trimNewLineCharacterFromString(name);

        int clientSockfd = clientSock.getSocketFD();

        int sizeOfName = nameSize;
        int c = write(clientSockfd, &sizeOfName, sizeof(sizeOfName));
        if (c < 0)
        {
            throw WritingError();
        }
        n = write(clientSockfd, name, strlen(name));
        if (n < 0)
        {
            throw WritingError();
        }

        pthread_t sendMessageThread;
        pthread_t receiveMessageThread;

        pthread_create(&sendMessageThread, NULL, &sendMessage, (void *)&clientSockfd);
        pthread_create(&receiveMessageThread, NULL, &receiveMessage, (void *)&clientSockfd);

        pthread_join(receiveMessageThread, NULL);

        clientSock.closeSocket();
    }
    catch (ServerClosed &error)
    {
        std::cout << error.what() << '\n';
    }
    catch (InvalidIpAddress &error)
    {
        std::cout << error.what() << '\n';
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