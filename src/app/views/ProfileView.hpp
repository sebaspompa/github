#pragma once

#include <string>
#include <algorithm>

#include "src/engine/utils.hpp"

#include "src/engine/ui/Button.hpp"
#include "src/engine/ui/InputField.hpp"

#include "src/engine/rendering/Canvas.hpp"
#include "src/engine/navigation/NavigationStack.hpp"

#include "src/app/viewmodels/ProfileViewModel.hpp"

using std::string;
using std::min;

struct ProfileView
{
private:
    static constexpr int FIELD_COUNT = 4;

    ProfileViewModel profileVM;

    InputField nameField;
    InputField usernameField;
    InputField emailField;
    InputField passwordField;

    Button saveButton;
    Button backButton;

    ANSI::Style frameStyle;
    ANSI::Style titleStyle;
    ANSI::Style mutedStyle;
    ANSI::Style textStyle;
    ANSI::Style footerStyle;
    ANSI::Style errorStyle;
    ANSI::Style successStyle;

    int focusedField = -1;

    struct Layout
    {
        unsigned int width;
        unsigned int height;
        unsigned int contentX;
        unsigned int contentWidth;
        unsigned int titleY;
        unsigned int subtitleY;
        unsigned int infoY;
        unsigned int separatorY;
        unsigned int nameY;
        unsigned int usernameY;
        unsigned int emailY;
        unsigned int passwordY;
        unsigned int saveY;
        unsigned int messageY;
        unsigned int backY;
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
        const unsigned int contentHeight = 35;

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
            topY + 1,
            topY + 3,
            topY + 7,
            topY + 9,
            topY + 14,
            topY + 19,
            topY + 24,
            topY + 29,
            topY + 31,
            topY + 33,
            size.height > 2 ? size.height - 2 : 0
        };
    }

    void focusField(int index)
    {
        nameField.blur();
        usernameField.blur();
        emailField.blur();
        passwordField.blur();

        if (index == 0)
        {
            nameField.focus();
        }
        else if (index == 1)
        {
            usernameField.focus();
        }
        else if (index == 2)
        {
            emailField.focus();
        }
        else if (index == 3)
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

        canvas.drawUTF8("?" + _string(layout.width - 2, "?") + "?", { 0, 0 }, frameStyle);

        for (unsigned int y = 1; y < layout.height - 1; y++)
        {
            canvas.drawUTF8("?", { 0, y }, frameStyle);
            canvas.drawUTF8("?", { layout.width - 1, y }, frameStyle);
        }

        canvas.drawUTF8("?" + _string(layout.width - 2, "?") + "?", { 0, layout.height - 1 }, frameStyle);
    }

    void drawHeader(Canvas& canvas, const Layout& layout) const
    {
        if (layout.titleY >= layout.height)
        {
            return;
        }

        drawContentCentered(canvas, "Mi Perfil", layout.titleY, layout, titleStyle);

        if (layout.subtitleY < layout.height)
        {
            drawContentCentered(canvas, "Consulta y edita tu informacion", layout.subtitleY, layout, mutedStyle);
        }
    }

    void drawInfoLine(Canvas& canvas, const string& label, const string& value, unsigned int y, const Layout& layout) const
    {
        if (y >= layout.height)
        {
            return;
        }

        canvas.drawUTF8(label, { layout.contentX, y }, mutedStyle);
        canvas.drawUTF8(value.empty() ? "(sin definir)" : value, { layout.contentX + 10, y }, textStyle);
    }

    void drawInfo(Canvas& canvas, const Layout& layout) const
    {
        const User& profile = profileVM.getProfile();

        drawInfoLine(canvas, "Nombre", profile.name, layout.infoY, layout);
        drawInfoLine(canvas, "Usuario", profile.username.empty() ? "" : "@" + profile.username, layout.infoY + 1, layout);
        drawInfoLine(canvas, "Email", profile.email, layout.infoY + 2, layout);

        if (layout.separatorY < layout.height && layout.contentWidth > 2)
        {
            canvas.drawUTF8(_string(layout.contentWidth, "?"), { layout.contentX, layout.separatorY }, frameStyle);
        }
    }

    void drawInputs(Canvas& canvas, const Layout& layout)
    {
        if (layout.contentWidth < 5)
        {
            return;
        }

        nameField.updateWidth(layout.contentWidth);
        usernameField.updateWidth(layout.contentWidth);
        emailField.updateWidth(layout.contentWidth);
        passwordField.updateWidth(layout.contentWidth);

        nameField.render(canvas, { layout.contentX, layout.nameY });
        usernameField.render(canvas, { layout.contentX, layout.usernameY });
        emailField.render(canvas, { layout.contentX, layout.emailY });
        passwordField.render(canvas, { layout.contentX, layout.passwordY });
    }

    void drawButtons(Canvas& canvas, const Layout& layout)
    {
        const string saveText = "Guardar Cambios";
        const string backText = "Volver";

        saveButton.updatePosition({ layout.contentX + centerX(saveText, layout.contentWidth), layout.saveY });
        saveButton.render(canvas);

        backButton.updatePosition({ layout.contentX + centerX(backText, layout.contentWidth), layout.backY });
        backButton.render(canvas);
    }

    void drawMessage(Canvas& canvas, const Layout& layout) const
    {
        if (layout.messageY >= layout.height)
        {
            return;
        }

        if (!profileVM.getError().empty())
        {
            drawContentCentered(canvas, profileVM.getError(), layout.messageY, layout, errorStyle);
        }
        else if (!profileVM.getNotice().empty())
        {
            drawContentCentered(canvas, profileVM.getNotice(), layout.messageY, layout, successStyle);
        }
    }

    void drawFooter(Canvas& canvas, const Layout& layout) const
    {
        if (layout.height < 3)
        {
            return;
        }

        drawCentered(canvas, "Deja vacio un campo para conservar su valor", layout.footerY, footerStyle);
    }

public:
    ProfileView()
        : nameField("name", "Nuevo nombre (opcional)", 10, 30),
        usernameField("username", "Nuevo usuario (opcional)", 10, 20),
        emailField("email", "Nuevo email (opcional)", 10, 40),
        passwordField("password", "Nueva contrasena (opcional)", 10, 20, true)
    {
        frameStyle.foreground = ANSI::Color::BrightBlack;

        titleStyle.foreground = ANSI::Color::BrightWhite;
        titleStyle.bold = true;

        mutedStyle.foreground = ANSI::Color::BrightBlack;

        textStyle.foreground = ANSI::Color::White;

        footerStyle.foreground = ANSI::Color::BrightBlack;

        errorStyle.foreground = ANSI::Color::Red;
        errorStyle.bold = true;

        successStyle.foreground = ANSI::Color::Green;
        successStyle.bold = true;

        ANSI::Style buttonNormal;
        buttonNormal.foreground = ANSI::Color::BrightWhite;
        buttonNormal.background = ANSI::Background::Default;

        ANSI::Style buttonHover;
        buttonHover.foreground = ANSI::Color::BrightWhite;
        buttonHover.background = ANSI::Background::Magenta;
        buttonHover.bold = true;

        ANSI::Style backNormal;
        backNormal.foreground = ANSI::Color::BrightBlack;
        backNormal.background = ANSI::Background::Default;

        saveButton = Button("Guardar Cambios", { 0, 0 }, buttonNormal, buttonHover);
        backButton = Button("Volver", { 0, 0 }, backNormal, buttonHover);

        focusField(0);
    }

    void render(Canvas& canvas)
    {
        const Layout layout = getLayout(canvas);

        drawFrame(canvas, layout);
        drawHeader(canvas, layout);
        drawInfo(canvas, layout);
        drawInputs(canvas, layout);
        drawButtons(canvas, layout);
        drawMessage(canvas, layout);
        drawFooter(canvas, layout);
    }

    void handle(const Event& event, NavigationStack& ns)
    {
        if (event.type == EventType::MouseDown)
        {
            const Point point(event.mouseX, event.mouseY);

            if (nameField.contains(point))
            {
                focusField(0);
                return;
            }

            if (usernameField.contains(point))
            {
                focusField(1);
                return;
            }

            if (emailField.contains(point))
            {
                focusField(2);
                return;
            }

            if (passwordField.contains(point))
            {
                focusField(3);
                return;
            }

            focusField(-1);
        }

        if (event.type == EventType::Key && event.key == Key::Tab)
        {
            focusField((focusedField + 1) % FIELD_COUNT);
            return;
        }

        nameField.handle(event);
        usernameField.handle(event);
        emailField.handle(event);
        passwordField.handle(event);

        // Los botones solo marcan banderas: la accion se ejecuta fuera del
        // callback y la navegacion es siempre lo ultimo que toca `this`.
        bool save = false;
        bool back = false;

        saveButton.onClick(event, [&save]() { save = true; });
        backButton.onClick(event, [&back]() { back = true; });

        if (back)
        {
            ns.pop();
            return;
        }

        if (save)
        {
            profileVM.save(
                nameField.getValue(),
                usernameField.getValue(),
                emailField.getValue(),
                passwordField.getValue()
            );
        }
    }
};