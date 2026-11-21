#pragma once
#include <cstdint>

namespace ANSI
{
    enum class Color : uint8_t
    {
        Default = 39,

        Black = 30,
        Red = 31,
        Green = 32,
        Yellow = 33,
        Blue = 34,
        Magenta = 35,
        Cyan = 36,
        White = 37,

        BrightBlack = 90,
        BrightRed = 91,
        BrightGreen = 92,
        BrightYellow = 93,
        BrightBlue = 94,
        BrightMagenta = 95,
        BrightCyan = 96,
        BrightWhite = 97
    };

    enum class Background : uint8_t
    {
        Default = 49,

        Black = 40,
        Red = 41,
        Green = 42,
        Yellow = 43,
        Blue = 44,
        Magenta = 45,
        Cyan = 46,
        White = 47,

        BrightBlack = 100,
        BrightRed = 101,
        BrightGreen = 102,
        BrightYellow = 103,
        BrightBlue = 104,
        BrightMagenta = 105,
        BrightCyan = 106,
        BrightWhite = 107
    };

    enum class Effect : uint8_t
    {
        None = 0,
        Bold = 1,
        Dim = 2,
        Italic = 3,
        Underline = 4,
        Blink = 5,
        RapidBlink = 6,
        Reverse = 7,
        Hidden = 8,
        Strikethrough = 9,

        DoubleUnderline = 21,
        NormalIntensity = 22,
        NoItalic = 23,
        NoUnderline = 24,
        NoBlink = 25,
        NoReverse = 27,
        NoHidden = 28,
        NoStrikethrough = 29
    };

    struct Style
    {
        Color foreground = Color::Default;
        Background background = Background::Default;

        bool bold = false;
        bool dim = false;
        bool italic = false;
        bool underline = false;
        bool blink = false;
        bool rapidBlink = false;
        bool reverse = false;
        bool hidden = false;
        bool strikethrough = false;
        bool doubleUnderline = false;

        bool operator==(const Style &other) const
        {
            return foreground == other.foreground &&
                   background == other.background &&
                   bold == other.bold &&
                   dim == other.dim &&
                   italic == other.italic &&
                   underline == other.underline &&
                   blink == other.blink &&
                   rapidBlink == other.rapidBlink &&
                   reverse == other.reverse &&
                   hidden == other.hidden &&
                   strikethrough == other.strikethrough &&
                   doubleUnderline == other.doubleUnderline;
        }

        bool operator!=(const Style &other) const
        {
            return !(*this == other);
        }
    };
}