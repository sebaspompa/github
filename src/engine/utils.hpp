#pragma once
#include <vector>
#include <string>

using std::string;
using std::vector;
using std::size_t;

vector<string> splitUtf8(const string &text)
{
    vector<string> result;

    for (size_t i = 0; i < text.size();)
    {
        unsigned char c = static_cast<unsigned char>(text[i]);

        size_t length = 1;

        if ((c & 0x80) == 0)
            length = 1;
        else if ((c & 0xE0) == 0xC0)
            length = 2;
        else if ((c & 0xF0) == 0xE0)
            length = 3;
        else if ((c & 0xF8) == 0xF0)
            length = 4;

        if (i + length > text.size())
            length = 1;

        result.push_back(text.substr(i, length));
        i += length;
    }

    return result;
}

string _string(const unsigned int &times, const string &value)
{
    string content;
    for (size_t i = 0; i < times; i++)
    {
        content += value;
    }
    return content;
}