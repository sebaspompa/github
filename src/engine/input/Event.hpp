#pragma once

enum class EventType
{
    None,
    Key,
    MouseMove,
    MouseDown
};

struct Event
{
    EventType type = EventType::None;

    int key = 0;

    int mouseX = 0;
    int mouseY = 0;

    bool leftButton = false;
};

namespace Key
{
    constexpr int LF = 10;
    constexpr int CR = 13;
    constexpr int Escape = 27;
    constexpr int Backspace = 127;
    constexpr int BackspaceOld = 8;
    constexpr int Tab = 9;

    constexpr int Up = 1001;
    constexpr int Down = 1002;
    constexpr int Left = 1003;
    constexpr int Right = 1004;
};