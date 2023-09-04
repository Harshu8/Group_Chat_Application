#pragma once
#include<iostream>

class FailedToBind : public std::exception
{
    public:
    std::string what();
};