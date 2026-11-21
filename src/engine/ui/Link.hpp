#pragma once
#include <string>
#include <functional>

#include "src/engine/rendering/ansi.hpp"
#include "src/engine/core/Rect.hpp"
#include "src/engine/input/Event.hpp"
#include "src/engine/rendering/Canvas.hpp"

using std::string;
using std::function;

struct Link
{
private:
    string label;
    string value;
    Rect rect;
    ANSI::Style normalStyle;
    ANSI::Style hoverStyle;
    bool isHovered = false;

    const ANSI::Style &currentStyle() const
    {
        if (isHovered)
        {
            return hoverStyle;
        }

        return normalStyle;
    }

public:
    Link() {}

    Link(const string &label, const string &value) : label(label), value(value)
    {
        rect.size = Size(label.length(), 1);

        hoverStyle.background = ANSI::Background::Magenta;
    }

    void render(Canvas &canvas, const Point &position)
    {
        rect.origin = position;

        canvas.drawUTF8(label, rect.origin, currentStyle());
    }

    void handle(const Event &event, function<void(const string &)> action)
    {
        // Hover
        if (event.type == EventType::MouseMove)
        {
            Point mousePosition((unsigned int)event.mouseX, (unsigned int)event.mouseY);
            isHovered = rect.containsInclusive(mousePosition);
        }

        // Click
        if (event.type == EventType::MouseDown && isHovered)
        {
            action(value);
        }
    }

    string getValue() const
    {
        return value;
    }
};