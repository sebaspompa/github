#pragma once

// Engine
#include "src/engine/utils.hpp"
#include "src/engine/ui/Button.hpp"
#include "src/engine/rendering/Canvas.hpp"
#include "src/engine/navigation/NavigationStack.hpp"

// Views
#include "src/app/views/LoginView.hpp"
#include "src/app/views/RegisterView.hpp"

using std::string;

struct MainView
{
private:
    Button loginButton;
    Button registerButton;

    ANSI::Style frameStyle;
    ANSI::Style logoStyle;

    struct Layout
    {
        unsigned int width;
        unsigned int height;
        unsigned int logoY;
        unsigned int buttonsY;
        unsigned int registerY;
    };

    Layout getLayout(const Canvas& canvas) const
    {
        const Size size = canvas.getSize();
        const unsigned int logoY = size.height >= 12 ? size.height / 2 - 6 : 1;
        const unsigned int buttonsY = size.height >= 12 ? size.height / 2 : 4;
        const unsigned int registerY = buttonsY + 5;

        return { size.width, size.height, logoY, buttonsY, registerY };
    }

    unsigned int centerX(const string& text, const unsigned int width) const
    {
        const unsigned int textWidth = static_cast<unsigned int>(splitUtf8(text).size());

        if (width <= textWidth)
        {
            return 0;
        }

        return (width - textWidth) / 2;
    }

    void drawCentered(Canvas& canvas, const string& text, const unsigned int y, const ANSI::Style& style) const
    {
        canvas.drawUTF8(text, { centerX(text, canvas.getSize().width), y }, style);
    }

    string repeat(const unsigned int times, const string& value) const
    {
        string content;

        for (unsigned int i = 0; i < times; i++)
        {
            content += value;
        }

        return content;
    }

    void drawFrame(Canvas& canvas, const Layout& layout) const
    {
        if (layout.width < 2 || layout.height < 2)
        {
            return;
        }

        canvas.drawUTF8("╭" + repeat(layout.width - 2, "─") + "╮", { 0, 0 }, frameStyle);

        for (unsigned int y = 1; y < layout.height - 1; y++)
        {
            canvas.drawUTF8("│", { 0, y }, frameStyle);
            canvas.drawUTF8("│", { layout.width - 1, y }, frameStyle);
        }

        canvas.drawUTF8("╰" + repeat(layout.width - 2, "─") + "╯", { 0, layout.height - 1 }, frameStyle);
    }

    void drawLogo(Canvas& canvas, const Layout& layout) const
    {
        if (layout.height < 4)
        {
            return;
        }

        drawCentered(canvas, "◉  ◉", layout.logoY, logoStyle);
        drawCentered(canvas, "◠◡◠◠◡◠", layout.logoY + 1, logoStyle);
    }

    void drawButtons(Canvas& canvas, const Layout& layout)
    {
        const string loginText = "→ Iniciar Sesion";
        const string registerText = "→ Registrarse";

        loginButton.updatePosition({ centerX(loginText, layout.width), layout.buttonsY });
        loginButton.render(canvas);

        registerButton.updatePosition({ centerX(registerText, layout.width), layout.registerY });
        registerButton.render(canvas);
    }

public:
    MainView()
    {
        frameStyle.foreground = ANSI::Color::BrightBlack;

        logoStyle.foreground = ANSI::Color::BrightMagenta;
        logoStyle.bold = true;

        ANSI::Style buttonNormal;
        buttonNormal.foreground = ANSI::Color::BrightWhite;
        buttonNormal.background = ANSI::Background::Default;

        ANSI::Style buttonHover;
        buttonHover.foreground = ANSI::Color::BrightWhite;
        buttonHover.background = ANSI::Background::Magenta;
        buttonHover.bold = true;

        loginButton = Button("→ Iniciar Sesion", { 0, 0 }, buttonNormal, buttonHover);
        registerButton = Button("→ Registrarse", { 0, 0 }, buttonNormal, buttonHover);
    }

    void render(Canvas& canvas)
    {
        const Layout layout = getLayout(canvas);

        drawFrame(canvas, layout);
        drawLogo(canvas, layout);
        drawButtons(canvas, layout);
    }

    void handle(const Event& event, NavigationStack& ns)
    {
        loginButton.onClick(event, [&ns]() { ns.push(LoginView()); });
        registerButton.onClick(event, [&ns]() { ns.push(RegisterView()); });
    }
};