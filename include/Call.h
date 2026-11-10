#pragma once
#include <string>

struct Call {
    int id = 0;
    int operatorId = 0;

    std::string clientPhone;
    std::string callDate;
    int duration = 0;

    std::string result;
};