#pragma once

#include <string>
#include <algorithm>

#include "src/engine/terminal/ansi.hpp"
#include "src/engine/core/Point.hpp"
#include "src/engine/core/Rect.hpp"
#include "src/engine/utils.hpp"
#include "src/engine/input/Event.hpp"
#include "src/engine/rendering/Canvas.hpp"

using std::string;
using std::size_t;
using std::min;

struct InputField
{
private:
    using UInt = unsigned int;

    string key;
    string value;
    string placeholder;
    bool isSecret;

    Rect rect;

    ANSI::Style style;
    ANSI::Style focusStyle;
    ANSI::Style placeholderStyle;

    UInt maxLength;

    bool isFocused = false;

    UInt valueWidth() const
    {
        return value.length();
    }

    string displayText() const
    {
        return value.empty() ? placeholder : (isSecret ? _string(valueWidth(), "•") : value);
    }

    UInt displayWidth() const
    {
        return displayText().length();
    }

    UInt innerWidth() const
    {
        return rect.size.width <= 2 ? 0 : rect.size.width - 2;
    }

    void removeLastCharacter()
    {
        if (value.empty())
        {
            return;
        }

        value.pop_back();
    }


    // Rendering
    void drawBorder(Canvas &canvas, const ANSI::Style &currentStyle) const
    {
        if (rect.size.width < 2)
        {
            return;
        }

        const size_t inside = innerWidth();

        // top
        canvas.drawUTF8(
            "╭" + _string(inside, "─") + "╮",
            rect.origin,
            currentStyle
        );

        // middle
        canvas.drawUTF8(
            "│" + _string(inside, " ") + "│",
            {
                rect.origin.x,
                rect.origin.y + 1
            },
            currentStyle
        );

        //  bottom
        canvas.drawUTF8(
            "╰" + _string(inside, "─") + "╯",
            {
                rect.origin.x,
                rect.origin.y + 2
            },
            currentStyle
        );
    }

    void drawText(Canvas &canvas) const
    {
        UInt availableWidth = innerWidth();

        if (availableWidth == 0 || (isFocused && availableWidth - 1 <= 0))
        {
            return;
        }

        string text = displayText();
        unsigned int textX = rect.origin.x + 2;

        // textX += isFocused && value.empty() ? 0 : 0;

        // Reserva una posición para el cursor.
        availableWidth += isFocused ? -1 : 0;

        if (text.length() > availableWidth)
        {
            const size_t end = min(availableWidth, (UInt)text.length());
            text = text.substr(0, end);
        }

        // Placeholder / value style
        const ANSI::Style &textStyle = value.empty() ? placeholderStyle : style;

        canvas.drawUTF8(
            text,
            {
                textX,
                rect.origin.y + 1
            },
            textStyle);
    }

    void drawCursor(Canvas &canvas) const
    {
        if (!isFocused)
        {
            return;
        }

        const size_t inside = innerWidth();

        if (inside == 0)
        {
            return;
        }

        const UInt cursorX = value.empty() ? rect.origin.x + 1 : rect.origin.x + 2 + valueWidth();

        canvas.drawUTF8(
            "│",
            {
                cursorX,
                rect.origin.y + 1
            },
            focusStyle
        );
    }

public:
    InputField() {}

    InputField(const string &key, const string &placeholder, UInt maxLength, UInt width, bool isSecret = false) : key(key), placeholder(placeholder), maxLength(maxLength), isSecret(isSecret)
    {
        style.foreground = ANSI::Color::White;
        focusStyle.foreground = ANSI::Color::BrightMagenta;
        focusStyle.bold = true;
        placeholderStyle.foreground = ANSI::Color::BrightBlack;

        rect.size = Size(width, 3);
    }

    void render(Canvas &canvas, const Point &position)
    {
        this->rect.origin = position;

        const ANSI::Style &currentStyle = isFocused ? focusStyle : style;

        // box
        drawBorder(canvas, currentStyle);

        // text
        drawText(canvas);

        // cursor
        drawCursor(canvas);
    }

    void handle(const Event &event)
    {
        if (event.type != EventType::Key || !isFocused)
        {
            return;
        }

        if (event.key == Key::Backspace)
        {
            removeLastCharacter();
        }
        else if (event.key >= 32 && event.key <= 126)
        {
            if (valueWidth() < maxLength)
            {
                value += static_cast<char>(event.key);
            }
        }
    }

    bool contains(const Point &point) const
    {
        return rect.contains(point);
    }

    const Rect& getRect() const
    {
        return rect;
    }

    void focus()
    {
        isFocused = true;
    }

    void blur()
    {
        isFocused = false;
    }

    bool focused() const
    {
        return isFocused;
    }

    const string& getValue() const
    {
        return value;
    }

    void setValue(const string &newValue)
    {
        value = newValue;
    }

    const string& getKey() const
    {
        return key;
    }

    void updateWidth(UInt width)
    {
        rect.size.width = width;
    }
};