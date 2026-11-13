#pragma once

#include <string>
#include <algorithm>

#include "src/engine/utils.hpp"

#include "src/engine/ui/Button.hpp"
#include "src/engine/ui/InputField.hpp"

#include "src/engine/rendering/Canvas.hpp"
#include "src/engine/navigation/NavigationStack.hpp"

#include "src/app/viewmodels/CreateRepositoryViewModel.hpp"

using std::string;
using std::min;

struct CreateRepositoryView
{
private:
    static constexpr int FIELD_COUNT = 3;

    CreateRepositoryViewModel createVM;

    InputField nameField;
    InputField descriptionField;
    InputField languageField;

    Button visibilityButton;
    Button createButton;
    Button cancelButton;

    ANSI::Style frameStyle;
    ANSI::Style titleStyle;
    ANSI::Style mutedStyle;
    ANSI::Style footerStyle;
    ANSI::Style errorStyle;
    ANSI::Style buttonNormal;
    ANSI::Style buttonHover;

    int focusedField = -1;

    bool isPublic = true;

    struct Layout
    {
        unsigned int width;
        unsigned int height;
        unsigned int contentX;
        unsigned int contentWidth;
        unsigned int titleY;
        unsigned int subtitleY;
        unsigned int nameY;
        unsigned int descriptionY;
        unsigned int languageY;
        unsigned int visibilityY;
        unsigned int createY;
        unsigned int errorY;
        unsigned int cancelY;
        unsigned int footerY;
    };

    string visibilityText() const
    {
        return isPublic ? "Visibilidad: PUBLICO" : "Visibilidad: PRIVADO";
    }

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
        const unsigned int contentHeight = 28;

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
            topY + 4,
            topY + 9,
            topY + 14,
            topY + 19,
            topY + 21,
            topY + 23,
            topY + 25,
            size.height > 2 ? size.height - 2 : 0
        };
    }

    void focusField(int index)
    {
        nameField.blur();
        descriptionField.blur();
        languageField.blur();

        if (index == 0)
        {
            nameField.focus();
        }
        else if (index == 1)
        {
            descriptionField.focus();
        }
        else if (index == 2)
        {
            languageField.focus();
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

        drawContentCentered(canvas, "Nuevo Repositorio", layout.titleY, layout, titleStyle);

        if (layout.subtitleY < layout.height)
        {
            drawContentCentered(canvas, "Crea un repositorio en tu espacio", layout.subtitleY, layout, mutedStyle);
        }
    }

    void drawInputs(Canvas& canvas, const Layout& layout)
    {
        if (layout.contentWidth < 5)
        {
            return;
        }

        nameField.updateWidth(layout.contentWidth);
        descriptionField.updateWidth(layout.contentWidth);
        languageField.updateWidth(layout.contentWidth);

        nameField.render(canvas, { layout.contentX, layout.nameY });
        descriptionField.render(canvas, { layout.contentX, layout.descriptionY });
        languageField.render(canvas, { layout.contentX, layout.languageY });
    }

    void drawButtons(Canvas& canvas, const Layout& layout)
    {
        const string visibility = visibilityText();
        const string createText = "Crear Repositorio";
        const string cancelText = "Cancelar";

        visibilityButton.updatePosition({ layout.contentX + centerX(visibility, layout.contentWidth), layout.visibilityY });
        visibilityButton.render(canvas);

        createButton.updatePosition({ layout.contentX + centerX(createText, layout.contentWidth), layout.createY });
        createButton.render(canvas);

        cancelButton.updatePosition({ layout.contentX + centerX(cancelText, layout.contentWidth), layout.cancelY });
        cancelButton.render(canvas);
    }

    void drawError(Canvas& canvas, const Layout& layout) const
    {
        if (createVM.getError().empty() || layout.errorY >= layout.height)
        {
            return;
        }

        drawContentCentered(canvas, createVM.getError(), layout.errorY, layout, errorStyle);
    }

    void drawFooter(Canvas& canvas, const Layout& layout) const
    {
        if (layout.height < 3)
        {
            return;
        }

        drawCentered(canvas, "Haz clic en la visibilidad para cambiarla", layout.footerY, footerStyle);
    }

public:
    CreateRepositoryView()
        : nameField("name", "Nombre del repositorio", 10, 30),
        descriptionField("description", "Descripcion corta", 10, 60),
        languageField("language", "Lenguaje (ej: cpp)", 10, 20)
    {
        frameStyle.foreground = ANSI::Color::BrightBlack;

        titleStyle.foreground = ANSI::Color::BrightWhite;
        titleStyle.bold = true;

        mutedStyle.foreground = ANSI::Color::BrightBlack;

        footerStyle.foreground = ANSI::Color::BrightBlack;

        errorStyle.foreground = ANSI::Color::Red;
        errorStyle.bold = true;

        buttonNormal.foreground = ANSI::Color::BrightWhite;
        buttonNormal.background = ANSI::Background::Default;

        buttonHover.foreground = ANSI::Color::BrightWhite;
        buttonHover.background = ANSI::Background::Magenta;
        buttonHover.bold = true;

        ANSI::Style cancelNormal;
        cancelNormal.foreground = ANSI::Color::BrightBlack;
        cancelNormal.background = ANSI::Background::Default;

        visibilityButton = Button(visibilityText(), { 0, 0 }, buttonNormal, buttonHover);
        createButton = Button("Crear Repositorio", { 0, 0 }, buttonNormal, buttonHover);
        cancelButton = Button("Cancelar", { 0, 0 }, cancelNormal, buttonHover);

        focusField(0);
    }

    void render(Canvas& canvas)
    {
        const Layout layout = getLayout(canvas);

        drawFrame(canvas, layout);
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

            if (nameField.contains(point))
            {
                focusField(0);
                return;
            }

            if (descriptionField.contains(point))
            {
                focusField(1);
                return;
            }

            if (languageField.contains(point))
            {
                focusField(2);
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
        descriptionField.handle(event);
        languageField.handle(event);

        // Los botones solo marcan banderas; la accion ocurre fuera del callback.
        bool toggle = false;
        bool create = false;
        bool cancel = false;

        visibilityButton.onClick(event, [&toggle]() { toggle = true; });
        createButton.onClick(event, [&create]() { create = true; });
        cancelButton.onClick(event, [&cancel]() { cancel = true; });

        if (cancel)
        {
            ns.pop();
            return;
        }

        if (create)
        {
            const bool created = createVM.create(
                nameField.getValue(),
                descriptionField.getValue(),
                languageField.getValue(),
                isPublic
            );

            if (created)
            {
                // Despues de esto la vista deja de existir: no tocar `this`
                ns.pop();
            }

            return;
        }

        if (toggle)
        {
            isPublic = !isPublic;
            visibilityButton = Button(visibilityText(), { 0, 0 }, buttonNormal, buttonHover);
        }
    }
};