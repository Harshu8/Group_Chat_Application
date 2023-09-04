#pragma once
#include<iostream>

class ReadingError : public std::exception
{
    public:
    std::string what();
};