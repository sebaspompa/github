#pragma once
#include <string>
#include <vector>

#include "src/app/models/Star.hpp"
#include "src/app/models/Fork.hpp"
#include "src/app/models/Language.hpp"

using std::string;
using std::vector;

struct Repository
{
    string id;
    string name;
    string description;
    Language language;
    vector<Star> stars;
    vector<Fork> forks;
    string visibility;
};