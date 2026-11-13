#pragma once
#include <string>

using std::string;

struct Issue
{
    enum class Priority
    {
        High = 3,
        Medium = 2,
        Low = 1
    };

    enum class State
    {
        Open,
        Closed
    };

    string id;
    string title;
    string description;
    Priority priority;
    State state;
};