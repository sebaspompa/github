#pragma once

// Session
#include "src/app/services/SessionService.hpp"

// Engine
#include "src/engine/rendering/Canvas.hpp"
#include "src/engine/input/InputSystem.hpp"
#include "src/engine/navigation/NavigationStack.hpp"
#include "src/app/AppContext.hpp"

// Views
#include "src/app/views/MainView.hpp"
#include "src/app/views/HomeView.hpp"
#include "src/app/views/RepositoryView.hpp"

struct App
{
private:
    Terminal &terminal = AppContext::instance().terminal();
    Canvas canvas = Canvas(terminal.getSize());
    InputSystem is;

    NavigationStack ns = NavigationStack(MainView());

    // Cambia el flujo visual de la aplicacion en base al estado de la sesión.
    void onSessionStateChanged(bool isActive)
    {
        if (isActive)
        {
            ns = NavigationStack(HomeView{});
        }
        else
        {
            ns = NavigationStack(MainView());
        }
    }

    // Retorna una referencia a la vista actual en la pila.
    AnyView& currentView()
    {
        return ns.current();
    }

public:
    App()
    {
        // Permite observar los cambios de estado de la sesión(activo o inactivo).
        SessionService::instance().init
        (
            [this](bool isActiveSession)
            {
                onSessionStateChanged(isActiveSession);
            }
        );

        SessionService::instance().create("0");
    }

    void run()
    {
        terminal.clear();

        while (true)
        {
            currentView().render(canvas);
            canvas.show();

            const Event event = is.poll();

            if (event.type == EventType::None)
            {
                continue;
            }

            if (event.key == Key::Escape)
            {
                // ns.pop();
                return;
            }

            currentView().handle(event, ns);
            canvas.clear();
        }
    }
};