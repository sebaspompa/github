#pragma once

#include <string>
#include <cstdio>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "src/engine/input/Event.hpp"


class InputSystem
{
private:

#ifdef _WIN32

    HANDLE inputHandle_;

#endif

public:

    InputSystem()
    {
#ifdef _WIN32

        inputHandle_ =
            GetStdHandle(STD_INPUT_HANDLE);

#endif
    }

    Event poll()
    {
#ifdef _WIN32

        return pollWindows();

#else

        return pollUnix();

#endif
    }

private:

#ifdef _WIN32

    // ========================================================
    // WINDOWS
    // ========================================================

    Event pollWindows()
    {
        INPUT_RECORD record{};
        DWORD count = 0;

        if (!ReadConsoleInput(
                inputHandle_,
                &record,
                1,
                &count))
        {
            return {};
        }

        // ====================================================
        // TECLADO
        // ====================================================

        if (record.EventType == KEY_EVENT)
        {
            const KEY_EVENT_RECORD& key =
                record.Event.KeyEvent;

            // Ignorar cuando se suelta una tecla.
            if (!key.bKeyDown)
                return {};

            int value = 0;

            switch (key.wVirtualKeyCode)
            {
            case VK_UP:
                value = Key::Up;
                break;

            case VK_DOWN:
                value = Key::Down;
                break;

            case VK_LEFT:
                value = Key::Left;
                break;

            case VK_RIGHT:
                value = Key::Right;
                break;

            case VK_ESCAPE:
                value = Key::Escape;
                break;

            case VK_BACK:
                value = Key::Backspace;
                break;

            case VK_TAB:
                value = Key::Tab;
                break;

            default:
                // Caracteres normales:
                //
                // a, b, c, 1, 2, 3, etc.
                //
                // Se conserva exactamente la idea
                // utilizada en pollUnix().
                value =
                    static_cast<unsigned char>(
                        key.uChar.AsciiChar);

                break;
            }

            if (value == 0)
                return {};

            Event event;

            event.type = EventType::Key;
            event.key = value;

            return event;
        }

        // ====================================================
        // MOUSE
        // ====================================================

        if (record.EventType == MOUSE_EVENT)
        {
            const MOUSE_EVENT_RECORD& mouse =
                record.Event.MouseEvent;

            Event event;

            event.mouseX =
                mouse.dwMousePosition.X;

            event.mouseY =
                mouse.dwMousePosition.Y;

            // ------------------------------------------------
            // MOVIMIENTO DEL MOUSE
            // ------------------------------------------------

            if (mouse.dwEventFlags == MOUSE_MOVED)
            {
                event.type =
                    EventType::MouseMove;

                event.leftButton =
                    (mouse.dwButtonState &
                     FROM_LEFT_1ST_BUTTON_PRESSED) != 0;

                return event;
            }

            // ------------------------------------------------
            // CLICK IZQUIERDO
            // ------------------------------------------------

            if (mouse.dwEventFlags == 0 &&
                (mouse.dwButtonState &
                 FROM_LEFT_1ST_BUTTON_PRESSED))
            {
                event.type =
                    EventType::MouseDown;

                event.leftButton = true;

                return event;
            }
        }

        return {};
    }

#else

    // ========================================================
    // UNIX / macOS / Linux
    //
    // Esta parte se mantiene igual.
    // ========================================================

    Event pollUnix()
    {
        unsigned char first = 0;

        const int readResult =
            static_cast<int>(
                read(STDIN_FILENO, &first, 1));

        if (readResult <= 0)
            return {};

        // ----------------------------------------------------
        // TECLAS NORMALES
        // ----------------------------------------------------

        if (first != 27)
        {
            Event event;

            event.type = EventType::Key;
            event.key = first;

            return event;
        }

        // ----------------------------------------------------
        // ESCAPE SEQUENCE
        // ----------------------------------------------------

        unsigned char second = 0;

        if (read(STDIN_FILENO, &second, 1) <= 0)
        {
            Event event;

            event.type = EventType::Key;
            event.key = Key::Escape;

            return event;
        }

        // ----------------------------------------------------
        // FLECHAS
        // ----------------------------------------------------

        if (second == '[')
        {
            unsigned char third = 0;

            if (read(STDIN_FILENO, &third, 1) <= 0)
            {
                Event event;

                event.type = EventType::Key;
                event.key = Key::Escape;

                return event;
            }

            if (third == 'A')
            {
                Event event;

                event.type = EventType::Key;
                event.key = Key::Up;

                return event;
            }

            if (third == 'B')
            {
                Event event;

                event.type = EventType::Key;
                event.key = Key::Down;

                return event;
            }

            if (third == 'C')
            {
                Event event;

                event.type = EventType::Key;
                event.key = Key::Right;

                return event;
            }

            if (third == 'D')
            {
                Event event;

                event.type = EventType::Key;
                event.key = Key::Left;

                return event;
            }

            // ------------------------------------------------
            // MOUSE SGR
            //
            // Formato:
            //
            // ESC [ < button ; x ; y M
            // ESC [ < button ; x ; y m
            // ------------------------------------------------

            if (third == '<')
            {
                std::string sequence;

                char character = 0;

                while (read(
                           STDIN_FILENO,
                           &character,
                           1) > 0)
                {
                    sequence += character;

                    if (character == 'M' ||
                        character == 'm')
                    {
                        break;
                    }

                    if (sequence.size() > 40)
                        break;
                }

                int button = 0;
                int x = 0;
                int y = 0;
                char ending = 0;

                const std::string full =
                    "<" + sequence;

                if (sscanf(
                        full.c_str(),
                        "<%d;%d;%d%c",
                        &button,
                        &x,
                        &y,
                        &ending) == 4)
                {
                    Event event;

                    event.mouseX = x - 1;
                    event.mouseY = y - 1;

                    // Movimiento del mouse.
                    if ((button & 32) != 0)
                    {
                        event.type =
                            EventType::MouseMove;

                        event.leftButton =
                            (button & 3) == 0;

                        return event;
                    }

                    // Botón izquierdo.
                    if ((button & 3) == 0 &&
                        ending == 'M')
                    {
                        event.type =
                            EventType::MouseDown;

                        event.leftButton = true;

                        return event;
                    }
                }
            }
        }

        return {};
    }

#endif
};