#pragma once
#include <string>

struct Entry
{
    enum class Type
    {
        File,
        Directory
    };

    std::string name;
    Type type;
};