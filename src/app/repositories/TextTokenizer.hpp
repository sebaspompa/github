#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include <unordered_set>

using std::string;
using std::stringstream;
using std::unordered_set;
using std::vector;

struct TextTokenizer
{
private:
    static bool isStopWord(const string &word)
    {
        static const unordered_set<string> stopWords =
        {
            "el", "la", "los", "las", "un", "una", "unos", "unas",
            "de", "del", "en", "y", "o", "a", "con", "por", "para",
            "the", "and", "or", "in", "on", "at", "to", "for", "of", "a", "an"
        };

        return stopWords.find(word) != stopWords.end();
    }

    static string normalize(const string &text)
    {
        string clean;

        for (char c : text)
        {
            if (std::isalnum(static_cast<unsigned char>(c)))
            {
                clean += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            }
            else
            {
                clean += ' ';
            }
        }

        return clean;
    }

public:
    static vector<string> extractKeywords(const string &text)
    {
        vector<string> keywords;
        unordered_set<string> seen;

        stringstream ss(normalize(text));
        string word;

        while (ss >> word)
        {
            if (word.size() < 2 || isStopWord(word))
            {
                continue;
            }

            if (seen.insert(word).second)
            {
                keywords.push_back(word);
            }
        }

        return keywords;
    }
};
