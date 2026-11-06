#pragma once
#include "src/engine/core/Point.hpp"
#include "src/engine/core/Size.hpp"

struct Rect
{
    Point origin;
    Size size;

    Rect(Point origin, Size size) : origin(origin), size(size) {}

    Rect() {}

    bool contains(const Point &point) const
    {
        const bool inX = origin.x < point.x && point.x < (origin.x + size.width - 1);
        const bool inY = origin.y < point.y && point.y < (origin.y + size.height - 1);
        return inX && inY;
    }

    bool containsInclusive(const Point &point) const
    {
        const bool inX = origin.x <= point.x && point.x <= (origin.x + size.width - 1);
        const bool inY = origin.y <= point.y && point.y <= (origin.y + size.height - 1);
        return inX && inY;
    }
};