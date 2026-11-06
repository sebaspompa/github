#pragma once
#include <vector>

#include "src/engine/navigation/AnyView.hpp"

using std::vector;

struct NavigationStack
{
private:
    vector<AnyView> views;

public:
    template <View V>
    NavigationStack(V rootView)
    {
        views.push_back(std::move(rootView));
    }

    // Agrega una vista a la pila.
    template <View V>
    void push(V view)
    {
        views.push_back(std::move(view));
    }

    // Elimina una vista de la pila.
    // Asegura que la pila siempre contenga una vista.
    void pop()
    {
        if (views.size() > 1)
        {
            views.pop_back();
        }
    }

    // Retorna una referencia a la vista actual.
    // Permite que la vista mute/cambie su estado.
    AnyView &current()
    {
        return views.back();
    }
};