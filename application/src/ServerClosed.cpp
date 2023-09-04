#include "ServerClosed.h"

std::string ServerClosed::what()
{
    return "Error in connecting, Server is closed. \nFirst start the server for connecting.\n";
}
