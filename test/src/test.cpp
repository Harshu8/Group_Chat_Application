#include <gtest/gtest.h>
#include "client.h"
#include "server.h"
#include "clientDetail.h"

TEST(ServerSocketTest, whenWeTryToConnectServerWithSocket)
{
    ServerSocket serverSocketObject(8998);
    EXPECT_GE(serverSocketObject.getSocketFD(), 0);
}

TEST(ServerSocketTest, whenWeBindToTheSocket)
{
    ServerSocket serverSocketObject(8998);
    EXPECT_GE(serverSocketObject.bindSocket(), 0);
}
TEST(ServerSocketTest, whenWeListenToOtherSocket)
{
    ServerSocket serverSocketObject(8998);
    EXPECT_GE(serverSocketObject.listenSocket(), 0);
}

TEST(ClientServerTest, whenWeTryToConnectClientWithSocket)
{
    struct hostent *ipAddress = gethostbyname("127.0.0.1");
    int portNo = 9898;
    ClientSocket clientSocketObject(portNo, ipAddress);
    EXPECT_GE(clientSocketObject.getSocketFD(), 0);
}

TEST(ClientServerTest, whenWeGiveIpAddressThenOurProgramWorkCorrectly)
{
    struct hostent *ipAddress = gethostbyname("127.0.0.1");
    int portNo = 9898;
    EXPECT_THROW(ClientSocket clientSocketObject(portNo, NULL), InvalidIpAddress);
    EXPECT_NO_THROW(ClientSocket clientSocketObject(portNo, ipAddress));
}

TEST(ClientServerTest, whenWeProvideCorrectIpAddress)
{
    struct hostent *ipAddress = gethostbyname("127.0.0.1");
    int portNo = 9897;
    ClientSocket clientSocketObject(portNo, ipAddress);
    EXPECT_EQ(clientSocketObject.connectToSocket(), 0);
}

TEST(ClientServerTest, whenWeProvideIncorrectPortNumber)
{
    struct hostent *ipAddress = gethostbyname("127.0.0.1");
    int portNo = 9898;
    ClientSocket clientSocketObject(portNo, ipAddress);
    EXPECT_NE(clientSocketObject.connectToSocket(), 0);
}