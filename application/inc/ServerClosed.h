#pragma once
#include<iostream>

class ServerClosed : public std::exception
{
    public:
    std::string what();
};