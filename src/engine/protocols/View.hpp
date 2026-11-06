#pragma once
#include <concepts>

// 'forward declaration' para evitar dependencia circular.
struct NavigationStack;
struct Event;
struct Canvas;

template <typename T>
concept Renderable = requires(T &obj, Canvas &canvas) {
    obj.render(canvas);
};


template <typename T>
concept Handable = requires(T &obj, const Event &event, NavigationStack &ns) {
    obj.handle(event, ns);
};

template <typename T>
concept View = Renderable<T> && Handable<T>;