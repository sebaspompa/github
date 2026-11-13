#pragma once
#include <string>

#include "src/app/models/Branch.hpp"

using std::string;

struct PullRequest
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
    Branch targetBranch;
    Branch sourceBranch;
    Priority priority;
    State state;
};