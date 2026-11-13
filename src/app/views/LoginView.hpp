#pragma once

#include <string>
#include <algorithm>

#include "src/engine/utils.hpp"

#include "src/engine/ui/Button.hpp"
#include "src/engine/ui/InputField.hpp"

#include "src/engine/rendering/Canvas.hpp"
#include "src/engine/navigation/NavigationStack.hpp"

#include "src/app/viewmodels/AuthViewModel.hpp"

// Views
#include "src/app/views/RegisterView.hpp"

using std::string;
using std::min;

struct LoginView
{
private:
    AuthViewModel authVM;

    InputField usernameField;
    InputField passwordField;

    Button loginButton;
    Button registerButton;

    ANSI::Style frameStyle;
    ANSI::Style logoStyle;
    ANSI::Style titleStyle;
    ANSI::Style mutedStyle;
    ANSI::Style footerStyle;
    ANSI::Style errorStyle;

    int focusedField = -1;

    struct Layout
    {
        unsigned int width;
        unsigned int height;
        unsigned int contentX;
        unsigned int contentWidth;
        unsigned int logoY;
        unsigned int titleY;
        unsigned int subtitleY;
        unsigned int usernameY;
        unsigned int passwordY;
        unsigned int buttonsY;
        unsigned int errorY;
        unsigned int registerY;
        unsigned int footerY;
    };

    unsigned int centerX(const string& text, unsigned int width) const
    {
        const unsigned int textWidth = splitUtf8(text).size();

        if (width <= textWidth)
        {
            return 0;
        }

        return (width - textWidth) / 2;
    }

    void drawCentered(Canvas& canvas, const string& text, unsigned int y, const ANSI::Style& style) const
    {
        canvas.drawUTF8(text, { centerX(text, canvas.getSize().width), y }, style);
    }

    void drawContentCentered(Canvas& canvas, const string& text, unsigned int y, const Layout& layout, const ANSI::Style& style) const
    {
        canvas.drawUTF8(text, { layout.contentX + centerX(text, layout.contentWidth), y }, style);
    }

    Layout getLayout(const Canvas& canvas) const
    {
        const Size size = canvas.getSize();

        unsigned int contentWidth = size.width * 40 / 100;
        contentWidth = min(contentWidth, 52u);

        if (contentWidth < 30 && size.width >= 32)
        {
            contentWidth = 30;
        }

        if (contentWidth >= size.width)
        {
            contentWidth = size.width > 2 ? size.width - 2 : size.width;
        }

        const unsigned int contentX = size.width > contentWidth ? (size.width - contentWidth) / 2 : 0;
        const unsigned int contentHeight = 27;

        unsigned int topY = size.height > contentHeight ? (size.height - contentHeight) / 2 : 1;

        if (topY == 0)
        {
            topY = 1;
        }

        return {
            size.width,
            size.height,
            contentX,
            contentWidth,
            topY,
            topY + 4,
            topY + 6,
            topY + 9,
            topY + 14,
            topY + 19,
            topY + 21,
            topY + 23,
            size.height > 2 ? size.height - 2 : 0
        };
    }

    void focusField(int index)
    {
        usernameField.blur();
        passwordField.blur();

        if (index == 0)
        {
            usernameField.focus();
        }
        else if (index == 1)
        {
            passwordField.focus();
        }

        focusedField = index;
    }

    void drawFrame(Canvas& canvas, const Layout& layout) const
    {
        if (layout.width < 2 || layout.height < 2)
        {
            return;
        }

        canvas.drawUTF8("╭" + _string(layout.width - 2, "─") + "╮", { 0, 0 }, frameStyle);

        for (unsigned int y = 1; y < layout.height - 1; y++)
        {
            canvas.drawUTF8("│", { 0, y }, frameStyle);
            canvas.drawUTF8("│", { layout.width - 1, y }, frameStyle);
        }

        canvas.drawUTF8("╰" + _string(layout.width - 2, "─") + "╯", { 0, layout.height - 1 }, frameStyle);
    }

    void drawLogo(Canvas& canvas, const Layout& layout) const
    {
        if (layout.height < 4)
        {
            return;
        }

        drawContentCentered(canvas, "◉  ◉", layout.logoY, layout, logoStyle);
        drawContentCentered(canvas, "◠◡◠◠◡◠", layout.logoY + 1, layout, logoStyle);
    }

    void drawHeader(Canvas& canvas, const Layout& layout) const
    {
        if (layout.titleY >= layout.height)
        {
            return;
        }

        drawContentCentered(canvas, "Iniciar Sesion", layout.titleY, layout, titleStyle);

        if (layout.subtitleY < layout.height)
        {
            drawContentCentered(canvas, "Accede a tu espacio de desarrollo", layout.subtitleY, layout, mutedStyle);
        }
    }

    void drawInputs(Canvas& canvas, const Layout& layout)
    {
        if (layout.contentWidth < 5)
        {
            return;
        }

        usernameField.updateWidth(layout.contentWidth);
        passwordField.updateWidth(layout.contentWidth);

        usernameField.render(canvas, { layout.contentX, layout.usernameY });
        passwordField.render(canvas, { layout.contentX, layout.passwordY });
    }

    void drawButtons(Canvas& canvas, const Layout& layout)
    {
        const string loginText = "→ Iniciar Sesion";
        const string registerText = "No estas registrado?";

        loginButton.updatePosition({ layout.contentX + centerX(loginText, layout.contentWidth), layout.buttonsY });
        loginButton.render(canvas);

        registerButton.updatePosition({ layout.contentX + centerX(registerText, layout.contentWidth), layout.registerY });
        registerButton.render(canvas);
    }

    void drawError(Canvas& canvas, const Layout& layout) const
    {
        if (authVM.getError().empty() || layout.errorY >= layout.height)
        {
            return;
        }

        drawContentCentered(canvas, authVM.getError(), layout.errorY, layout, errorStyle);
    }

    void drawFooter(Canvas& canvas, const Layout& layout) const
    {
        if (layout.height < 3)
        {
            return;
        }

        drawCentered(canvas, "GitHub Terminal  ·  v0.1-beta", layout.footerY, footerStyle);
    }

public:
    LoginView()
        : usernameField("username", "Escribe tu nombre de usuario", 10, 20),
        passwordField("password", "Escribe tu contrasena", 10, 20, true)
    {
        frameStyle.foreground = ANSI::Color::BrightBlack;

        logoStyle.foreground = ANSI::Color::BrightMagenta;
        logoStyle.bold = true;

        titleStyle.foreground = ANSI::Color::BrightWhite;
        titleStyle.bold = true;

        mutedStyle.foreground = ANSI::Color::BrightBlack;

        footerStyle.foreground = ANSI::Color::BrightBlack;

        errorStyle.foreground = ANSI::Color::Red;
        errorStyle.bold = true;

        ANSI::Style buttonNormal;
        buttonNormal.foreground = ANSI::Color::BrightWhite;
        buttonNormal.background = ANSI::Background::Default;

        ANSI::Style buttonHover;
        buttonHover.foreground = ANSI::Color::BrightWhite;
        buttonHover.background = ANSI::Background::Magenta;
        buttonHover.bold = true;

        ANSI::Style linkNormal;
        linkNormal.foreground = ANSI::Color::BrightBlack;
        linkNormal.background = ANSI::Background::Default;

        loginButton = Button("→ Iniciar Sesion", { 0, 0 }, buttonNormal, buttonHover);
        registerButton = Button("No estas registrado?", { 0, 0 }, linkNormal, buttonHover);

        focusField(0);
    }

    void render(Canvas& canvas)
    {
        const Layout layout = getLayout(canvas);

        drawFrame(canvas, layout);
        drawLogo(canvas, layout);
        drawHeader(canvas, layout);
        drawInputs(canvas, layout);
        drawButtons(canvas, layout);
        drawError(canvas, layout);
        drawFooter(canvas, layout);
    }

    void handle(const Event& event, NavigationStack& ns)
    {
        if (event.type == EventType::MouseDown)
        {
            const Point point(event.mouseX, event.mouseY);

            if (usernameField.contains(point))
            {
                focusField(0);
                return;
            }

            if (passwordField.contains(point))
            {
                focusField(1);
                return;
            }

            focusField(-1);
        }

        if (event.type == EventType::Key && event.key == Key::Tab)
        {
            if (focusedField == -1)
            {
                focusField(0);
            }
            else if (focusedField == 0)
            {
                focusField(1);
            }
            else
            {
                focusField(0);
            }

            return;
        }

        usernameField.handle(event);
        passwordField.handle(event);

        bool submit = false;

        loginButton.onClick(event, [&submit]() { submit = true; });

        if (submit)
        {
            // Si el login funciona, esta vista puede ser reemplazada:
            // no se debe tocar nada de `this` despues de esta llamada.
            authVM.login(usernameField.getValue(), passwordField.getValue());
            return;
        }

        registerButton.onClick(event, [&ns]()
            {
                ns.push(RegisterView());
            });
    }
};