#pragma once

using uint = unsigned int;

struct Point
{
    uint x, y;

    Point(uint x, uint y) : x(x), y(y) {}

    Point() {}
};