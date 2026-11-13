#pragma once

#include <functional>

#include "src/engine/utils.hpp"

// Session
#include "src/app/services/SessionService.hpp"

// Engine
#include "src/engine/ui/Button.hpp"
#include "src/engine/ui/Link.hpp"
#include "src/engine/rendering/Canvas.hpp"
#include "src/engine/input/Event.hpp"
#include "src/engine/navigation/NavigationStack.hpp"

// ViewModels
#include "src/app/viewmodels/HomeViewModel.hpp"

// Views
#include "src/app/views/RepositoryView.hpp"
#include "src/app/views/ProfileView.hpp"
#include "src/app/views/CreateRepositoryView.hpp"

using std::string;
using std::vector;
using std::function;
using std::to_string;

struct HomeView
{
private:
    HomeViewModel homeVM;

    Button profileButton;
    Button createRepositoryButton;
    Button logoutButton;

    vector<Link> repositoryLinks;
    unsigned long linksVersion = 0;

    ANSI::Style frameStyle;
    ANSI::Style sidebarStyle;
    ANSI::Style logoStyle;
    ANSI::Style titleStyle;
    ANSI::Style subtitleStyle;
    ANSI::Style textStyle;
    ANSI::Style mutedStyle;
    ANSI::Style accentStyle;
    ANSI::Style dimStyle;
    ANSI::Style statusStyle;
    ANSI::Style footerStyle;

    struct Layout
    {
        unsigned int width;
        unsigned int height;

        unsigned int sidebarWidth;

        unsigned int contentX;
        unsigned int contentWidth;

        unsigned int headerY;
        unsigned int separatorY;
        unsigned int repositoriesY;
        unsigned int repositoryY;

        unsigned int statusY;
        unsigned int footerY;

        unsigned int repositoryX;
        unsigned int repositoryWidth;
    };

    Layout getLayout(const Canvas& canvas) const
    {
        const Size size = canvas.getSize();

        const unsigned int width = size.width;
        const unsigned int height = size.height;

        unsigned int sidebarWidth = width / 5;

        if (sidebarWidth < 22 && width >= 30)
        {
            sidebarWidth = 22;
        }

        if (sidebarWidth > 34)
        {
            sidebarWidth = 34;
        }

        if (sidebarWidth >= width)
        {
            sidebarWidth = width > 1 ? width - 1 : 0;
        }

        const unsigned int contentX = sidebarWidth + 4;

        const unsigned int contentWidth = width > contentX + 2 ? width - contentX - 2 : 0;

        const unsigned int headerY = height >= 8 ? 4 : 1;

        const unsigned int separatorY = height >= 12 ? 9 : headerY + 2;

        const unsigned int repositoriesY = height >= 16 ? 12 : separatorY + 2;

        const unsigned int repositoryY = repositoriesY + 3;

        const unsigned int statusY = height >= 5 ? height - 5 : 0;

        const unsigned int footerY = height >= 3 ? height - 2 : 0;

        return { width, height, sidebarWidth, contentX, contentWidth, headerY, separatorY, repositoriesY, repositoryY, statusY, footerY, contentX, contentWidth };
    }

    unsigned int centerX(const string& text, const unsigned int areaX, const unsigned int areaWidth) const
    {
        const unsigned int textWidth = static_cast<unsigned int>(splitUtf8(text).size());

        if (areaWidth <= textWidth)
        {
            return areaX;
        }

        return areaX + (areaWidth - textWidth) / 2;
    }

    void drawCentered(Canvas& canvas, const string& text, const unsigned int y, const unsigned int areaX, const unsigned int areaWidth, const ANSI::Style& style) const
    {
        canvas.drawUTF8(text, { centerX(text, areaX, areaWidth), y }, style);
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

    string horizontalLine(const unsigned int width) const
    {
        return repeat(width, "─");
    }

    void drawFrame(Canvas& canvas, const Layout& layout) const
    {
        if (layout.width < 2 || layout.height < 2)
        {
            return;
        }

        canvas.drawUTF8("╭" + horizontalLine(layout.width - 2) + "╮", { 0, 0 }, frameStyle);

        for (unsigned int y = 1; y < layout.height - 1; y++)
        {
            canvas.drawUTF8("│", { 0, y }, frameStyle);
            canvas.drawUTF8("│", { layout.width - 1, y }, frameStyle);
        }

        canvas.drawUTF8("╰" + horizontalLine(layout.width - 2) + "╯", { 0, layout.height - 1 }, frameStyle);
    }

    void drawLogo(Canvas& canvas, const Layout& layout) const
    {
        if (layout.sidebarWidth < 10 || layout.height < 8)
        {
            return;
        }

        const string eyes = "◉  ◉";
        const string mouth = "◠◡◠◠◡◠";

        const unsigned int eyesWidth = static_cast<unsigned int>(splitUtf8(eyes).size());
        const unsigned int mouthWidth = static_cast<unsigned int>(splitUtf8(mouth).size());

        const unsigned int logoWidth = eyesWidth > mouthWidth ? eyesWidth : mouthWidth;

        const unsigned int logoX = layout.sidebarWidth > logoWidth ? (layout.sidebarWidth - logoWidth) / 2 : 0;

        const unsigned int eyesX = logoX + (logoWidth - eyesWidth) / 2;
        const unsigned int mouthX = logoX + (logoWidth - mouthWidth) / 2;

        const unsigned int logoY = 4;

        canvas.drawUTF8(eyes, { eyesX, logoY }, logoStyle);
        canvas.drawUTF8(mouth, { mouthX, logoY + 1 }, logoStyle);
    }

    void drawSidebar(Canvas& canvas, const Layout& layout)
    {
        if (layout.sidebarWidth == 0)
        {
            return;
        }

        if (layout.sidebarWidth < layout.width)
        {
            for (unsigned int y = 1; y + 1 < layout.height; y++)
            {
                canvas.drawUTF8("│", { layout.sidebarWidth, y }, sidebarStyle);
            }
        }

        drawLogo(canvas, layout);

        if (layout.sidebarWidth > 18 && layout.height > 10)
        {
            drawCentered(canvas, "GitHub Terminal", 8, 1, layout.sidebarWidth - 1, titleStyle);
        }

        if (layout.height > 10)
        {
            drawCentered(canvas, "@" + homeVM.getUsername(), 10, 1, layout.sidebarWidth - 1, mutedStyle);
        }

        if (layout.sidebarWidth > 10 && layout.height > 13)
        {
            canvas.drawUTF8(horizontalLine(layout.sidebarWidth - 10), { 5, 13 }, frameStyle);
        }

        if (layout.height > 15)
        {
            canvas.drawUTF8("ESPACIO PERSONAL", { 5, 15 }, dimStyle);
        }

        const unsigned int buttonX = 5;

        if (layout.height >= 21)
        {
            profileButton.updatePosition({ buttonX, 17 });
            profileButton.render(canvas);
        }

        if (layout.height >= 25)
        {
            createRepositoryButton.updatePosition({ buttonX, 21 });
            createRepositoryButton.render(canvas);
        }

        if (layout.height >= 29)
        {
            const unsigned int logoutY = layout.footerY > 29 ? layout.footerY - 4 : 25;

            logoutButton.updatePosition({ buttonX, logoutY });
            logoutButton.render(canvas);
        }

        if (layout.height >= 4)
        {
            drawCentered(canvas, "v0.1-beta", layout.footerY, 1, layout.sidebarWidth - 1, footerStyle);
        }
    }

    void drawHeader(Canvas& canvas, const Layout& layout) const
    {
        if (layout.contentWidth == 0)
        {
            return;
        }

        canvas.drawUTF8("INICIO", { layout.contentX, layout.headerY }, mutedStyle);

        if (layout.contentWidth >= 20)
        {
            canvas.drawUTF8("Tu espacio de desarrollo", { layout.contentX, layout.headerY + 2 }, titleStyle);
        }

        if (layout.contentWidth >= 40)
        {
            canvas.drawUTF8("Tus repositorios recientes y proyectos activos", { layout.contentX, layout.headerY + 4 }, subtitleStyle);
        }

        if (layout.contentWidth > 2)
        {
            canvas.drawUTF8(horizontalLine(layout.contentWidth), { layout.contentX, layout.separatorY }, frameStyle);
        }
    }

    unsigned int textWidth(const string& text) const
    {
        return static_cast<unsigned int>(splitUtf8(text).size());
    }

    void drawRepository(Canvas& canvas, const Repository& repository, const unsigned int y, const Layout& layout, Link& repositoryLink) const
    {
        const unsigned int x = layout.repositoryX;
        const unsigned int nameX = x + 4;

        canvas.drawUTF8("◆", { x, y }, accentStyle);

        repositoryLink.render(canvas, { nameX, y });

        const unsigned int nameWidth = textWidth(repository.name);

        if (repository.visibility == "1" || repository.visibility == "0")
        {
            const string visibility = repository.visibility == "1" ? "PUBLICO" : "PRIVADO";

            canvas.drawUTF8(visibility, { nameX + nameWidth + 3, y }, dimStyle);
        }

        canvas.drawUTF8(repository.description, { nameX, y + 2 }, textStyle);

        // Linea de metadatos: lenguaje, estrellas y forks
        unsigned int metaX = nameX;
        const unsigned int metaY = y + 4;

        if (!repository.language.name.empty())
        {
            canvas.drawUTF8("●", { metaX, metaY }, accentStyle);
            canvas.drawUTF8(repository.language.name, { metaX + 2, metaY }, mutedStyle);

            metaX += 2 + textWidth(repository.language.name) + 3;
        }

        const string stars = "★ " + to_string(repository.stars.size());
        canvas.drawUTF8(stars, { metaX, metaY }, mutedStyle);

        metaX += textWidth(stars) + 3;

        const string forks = "⑂ " + to_string(repository.forks.size());
        canvas.drawUTF8(forks, { metaX, metaY }, mutedStyle);

        if (layout.repositoryWidth > 2)
        {
            canvas.drawUTF8(horizontalLine(layout.repositoryWidth), { x, y + 5 }, frameStyle);
        }
    }

    void updateRepositoryLinks()
    {
        const vector<Repository>& repositories = homeVM.getRepositories();

        repositoryLinks.clear();

        for (const Repository& repository : repositories)
        {
            repositoryLinks.emplace_back(repository.name, repository.id);
        }

        linksVersion = homeVM.getVersion();
    }

    void drawRepositories(Canvas& canvas, const Layout& layout)
    {
        if (layout.contentWidth < 10)
        {
            return;
        }

        canvas.drawUTF8("REPOSITORIOS", { layout.contentX, layout.repositoriesY }, accentStyle);

        const vector<Repository>& repositories = homeVM.getRepositories();

        const string count = to_string(repositories.size()) + " repositorios";

        const unsigned int countWidth = static_cast<unsigned int>(splitUtf8(count).size());

        const unsigned int countX = layout.contentX + (layout.contentWidth > countWidth ? layout.contentWidth - countWidth : 0);

        canvas.drawUTF8(count, { countX, layout.repositoriesY }, mutedStyle);

        // Los enlaces se reconstruyen cuando los datos se recargaron
        // (por ejemplo, despues de crear un repositorio)
        if (repositoryLinks.size() != repositories.size() || linksVersion != homeVM.getVersion())
        {
            updateRepositoryLinks();
        }

        unsigned int y = layout.repositoryY;

        for (unsigned int i = 0; i < repositories.size() && i < repositoryLinks.size(); i++)
        {
            drawRepository(canvas, repositories[i], y, layout, repositoryLinks[i]);

            y += 8;

            if (y >= layout.footerY)
            {
                break;
            }
        }
    }

public:
    HomeView()
    {
        frameStyle.foreground = ANSI::Color::BrightBlack;

        sidebarStyle.foreground = ANSI::Color::BrightBlack;

        logoStyle.foreground = ANSI::Color::BrightMagenta;
        logoStyle.bold = true;

        titleStyle.foreground = ANSI::Color::BrightWhite;
        titleStyle.bold = true;

        subtitleStyle.foreground = ANSI::Color::White;

        textStyle.foreground = ANSI::Color::White;

        mutedStyle.foreground = ANSI::Color::BrightBlack;

        dimStyle.foreground = ANSI::Color::BrightBlack;
        dimStyle.dim = true;

        accentStyle.foreground = ANSI::Color::BrightMagenta;
        accentStyle.bold = true;

        statusStyle.foreground = ANSI::Color::Green;
        statusStyle.bold = true;

        footerStyle.foreground = ANSI::Color::BrightBlack;

        ANSI::Style navigationNormal;
        navigationNormal.foreground = ANSI::Color::White;
        navigationNormal.background = ANSI::Background::Default;

        ANSI::Style navigationHover;
        navigationHover.foreground = ANSI::Color::BrightWhite;
        navigationHover.background = ANSI::Background::Magenta;
        navigationHover.bold = true;

        profileButton = Button("◉  Perfil", { 5, 17 }, navigationNormal, navigationHover);
        createRepositoryButton = Button("+  Nuevo repo", { 5, 21 }, navigationNormal, navigationHover);
        logoutButton = Button("←  Cerrar sesion", { 5, 25 }, navigationNormal, navigationHover);
    }

    void render(Canvas& canvas)
    {
        const Layout layout = getLayout(canvas);

        drawFrame(canvas, layout);
        drawSidebar(canvas, layout);
        drawHeader(canvas, layout);
        drawRepositories(canvas, layout);
    }

    void handle(const Event& event, NavigationStack& ns)
    {
        // Los callbacks solo marcan banderas. La accion se ejecuta despues,
        // y es lo ultimo que toca `this` (la vista puede ser reemplazada).
        bool goProfile = false;
        bool goCreateRepository = false;
        bool logout = false;
        bool openRepository = false;
        string repositoryToOpen;

        profileButton.onClick(event, [&goProfile]() { goProfile = true; });
        createRepositoryButton.onClick(event, [&goCreateRepository]() { goCreateRepository = true; });
        logoutButton.onClick(event, [&logout]() { logout = true; });

        for (Link& repositoryLink : repositoryLinks)
        {
            repositoryLink.handle(event, [&openRepository, &repositoryToOpen](const string& repositoryId)
                {
                    openRepository = true;
                    repositoryToOpen = repositoryId;
                });
        }

        if (goProfile)
        {
            ns.push(ProfileView());
            return;
        }

        if (goCreateRepository)
        {
            ns.push(CreateRepositoryView());
            return;
        }

        if (openRepository)
        {
            ns.push(RepositoryView(repositoryToOpen));
            return;
        }

        if (logout)
        {
            SessionService::instance().revoke();
            return;
        }
    }
};