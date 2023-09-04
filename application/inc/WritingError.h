#pragma once
#include<iostream>

class WritingError : public std::exception
{
    public:
    std::string what();
};