#pragma once
#include<iostream>

class PortNotProvided : public std::exception
{
    public:
    std::string what();
};