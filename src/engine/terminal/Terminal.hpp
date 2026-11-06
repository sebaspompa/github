#pragma once

#include <iostream>
#include <cstdlib>

#ifdef _WIN32

#include <windows.h>

#else

#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

#endif

#include "src/engine/core/Size.hpp"


class Terminal
{
private:

#ifdef _WIN32

    HANDLE inputHandle_;
    HANDLE outputHandle_;

    DWORD oldInputMode_ = 0;
    DWORD oldOutputMode_ = 0;

    UINT oldInputCodePage_ = 0;
    UINT oldOutputCodePage_ = 0;

#else

    termios oldTerminal_{};

#endif

public:

    Terminal()
    {
        setup();
    }

    ~Terminal()
    {
        restore();
    }

    void setup()
    {
#ifdef _WIN32

        // ====================================================
        // WINDOWS
        // ====================================================

        inputHandle_ =
            GetStdHandle(STD_INPUT_HANDLE);

        outputHandle_ =
            GetStdHandle(STD_OUTPUT_HANDLE);

        GetConsoleMode(
            inputHandle_,
            &oldInputMode_);

        GetConsoleMode(
            outputHandle_,
            &oldOutputMode_);

        oldInputCodePage_ =
            GetConsoleCP();

        oldOutputCodePage_ =
            GetConsoleOutputCP();

        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);

        // ----------------------------------------------------
        // INPUT
        // ----------------------------------------------------

        DWORD inputMode =
            oldInputMode_;

        inputMode &= ~ENABLE_LINE_INPUT;
        inputMode &= ~ENABLE_ECHO_INPUT;
        inputMode &= ~ENABLE_PROCESSED_INPUT;

        inputMode |= ENABLE_WINDOW_INPUT;
        inputMode |= ENABLE_MOUSE_INPUT;

        SetConsoleMode(
            inputHandle_,
            inputMode);

        // ----------------------------------------------------
        // OUTPUT
        // ----------------------------------------------------

        DWORD outputMode =
            oldOutputMode_;

        outputMode |=
            ENABLE_VIRTUAL_TERMINAL_PROCESSING;

        outputMode |=
            ENABLE_PROCESSED_OUTPUT;

        SetConsoleMode(
            outputHandle_,
            outputMode);

#else

        // ====================================================
        // UNIX / macOS / Linux
        // ====================================================

        tcgetattr(
            STDIN_FILENO,
            &oldTerminal_);

        termios raw =
            oldTerminal_;

        raw.c_lflag &=
            ~(ICANON | ECHO);

        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 1;

        tcsetattr(
            STDIN_FILENO,
            TCSANOW,
            &raw);

#endif

        // ====================================================
        // CONFIGURACIÓN COMÚN
        // ====================================================

        hideCursor();

        std::cout << "\033[2J";
        std::cout << "\033[H";

        enableMouse();

        std::cout.flush();
    }

    void restore()
    {
        // ====================================================
        // CONFIGURACIÓN COMÚN
        // ====================================================

        disableMouse();

        showCursor();

        std::cout << "\033[0m";
        std::cout << "\033[2J";
        std::cout << "\033[H";

        std::cout.flush();

#ifdef _WIN32

        // ====================================================
        // WINDOWS
        // ====================================================

        SetConsoleMode(
            inputHandle_,
            oldInputMode_);

        SetConsoleMode(
            outputHandle_,
            oldOutputMode_);

        SetConsoleCP(
            oldInputCodePage_);

        SetConsoleOutputCP(
            oldOutputCodePage_);

#else

        // ====================================================
        // UNIX / macOS / Linux
        // ====================================================

        tcsetattr(
            STDIN_FILENO,
            TCSANOW,
            &oldTerminal_);

#endif
    }

    void enableMouse()
    {
        std::cout << "\033[?1003h";
        std::cout << "\033[?1006h";

        std::cout.flush();
    }

    void disableMouse()
    {
        std::cout << "\033[?1003l";
        std::cout << "\033[?1006l";

        std::cout.flush();
    }

    void hideCursor()
    {
        std::cout << "\033[?25l";
    }

    void showCursor()
    {
        std::cout << "\033[?25h";
    }

    Size getSize()
    {
#ifdef _WIN32

        // ====================================================
        // WINDOWS
        // ====================================================

        CONSOLE_SCREEN_BUFFER_INFO info{};

        GetConsoleScreenBufferInfo(
            outputHandle_,
            &info);

        Size size;

        size.width =
            info.srWindow.Right -
            info.srWindow.Left + 1;

        size.height =
            info.srWindow.Bottom -
            info.srWindow.Top + 1;

        return size;

#else

        // ====================================================
        // UNIX / macOS / Linux
        // ====================================================

        winsize sizeInfo{};

        ioctl(
            STDOUT_FILENO,
            TIOCGWINSZ,
            &sizeInfo);

        Size size;

        size.width =
            static_cast<int>(
                sizeInfo.ws_col);

        size.height =
            static_cast<int>(
                sizeInfo.ws_row);

        if (size.width <= 0)
            size.width = 120;

        if (size.height <= 0)
            size.height = 35;

        return size;

#endif
    }

    void clear() const
    {
#ifdef _WIN32

        system("cls");

#else

        system("clear");

#endif
    }
};