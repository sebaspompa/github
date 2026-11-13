#pragma once
#include <string>
#include <vector>

#include "src/app/models/Author.hpp"

using std::string;
using std::vector;

struct Commit
{
    string id;
    string title;
    string description;
    string parentId;
};