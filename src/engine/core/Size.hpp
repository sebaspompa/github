#pragma once

using uint = unsigned int;

struct Size
{
    uint width, height;

    Size(uint width, uint height) : width(width), height(height) {}

    Size() {}
};