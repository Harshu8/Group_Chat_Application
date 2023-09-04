#pragma once
#include<iostream>

class InvalidIpAddress : public std::exception
{
    public:
    std::string what();
};