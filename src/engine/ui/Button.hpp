#pragma once
#include <string>

#include "src/engine/core/Rect.hpp"
#include "src/engine/core/Size.hpp"
#include "src/engine/input/Event.hpp"
#include "src/engine/rendering/ansi.hpp"
#include "src/engine/utils.hpp"
#include "src/engine/rendering/Canvas.hpp"

using std::string;
using std::size_t;

struct Button
{
private:
    // Content
    string text;

    // State
    bool hovered = false;

    // Box
    Rect rect;

    // Style
    ANSI::Style normalStyle;
    ANSI::Style hoverStyle;

    const ANSI::Style &currentStyle() const
    {
        return hovered ? hoverStyle : normalStyle;
    }

public:
    Button(const string &text, const Point &position, const ANSI::Style &normalStyle, const ANSI::Style &hoverStyle) : text(text), normalStyle(normalStyle), hoverStyle(hoverStyle)
    {
        size_t width = splitUtf8(text).size();

        rect = {position, Size(width + 2, 3)};
    }

    Button() {}

    void render(Canvas &canvas) const
    {
        const ANSI::Style &style = currentStyle();

        // Bg
        canvas.paint(rect, style.background);

        // Content
        canvas.drawUTF8(
            text,
            Point(
                rect.origin.x + 1,
                rect.origin.y + 1),
            style);
    }

    void onClick(const Event &event, function<void()> action)
    {
        if (event.type == EventType::MouseMove)
        {
            Point mousePosition(event.mouseX, event.mouseY);

            hovered = rect.containsInclusive(mousePosition);
        }

        if (event.type == EventType::MouseDown)
        {
            if (hovered)
            {
                action();
            }
        }
    }

    void updatePosition(Point position)
    {
        rect.origin = position;
    }

    Size getSize() const
    {
        return rect.size;
    }

    void setStyles(const ANSI::Style &normalStyle, const ANSI::Style &hoverStyle)
    {
        this->normalStyle = normalStyle;
        this->hoverStyle = hoverStyle;
    }
};