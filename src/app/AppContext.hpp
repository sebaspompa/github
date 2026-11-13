#pragma once

#include "src/engine/terminal/Terminal.hpp"

struct AppContext
{
private:
    Terminal _terminal;

    AppContext() = default;

    ~AppContext() = default;

public:
    AppContext(const AppContext &) = delete;
    AppContext &operator=(const AppContext &) = delete;

    static AppContext &instance()
    {
        static AppContext instance;
        return instance;
    }

    Terminal &terminal()
    {
        return _terminal;
    }
};