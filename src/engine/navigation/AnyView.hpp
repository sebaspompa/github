#pragma once
#include <memory>
#include <utility>

#include "src/engine/protocols/View.hpp"

class AnyView
{
public:
    template <View V>
    AnyView(V view)
        : self_(std::make_unique<Model<V>>(std::move(view))) {}

    void render(Canvas &canvas)
    {
        self_->render(canvas);
    }

    void handle(const Event &event, NavigationStack &ns)
    {
        self_->handle(event, ns);
    }

private:
    struct Concept
    {
        virtual ~Concept() = default;

        virtual void render(Canvas &) = 0;
        virtual void handle(const Event &, NavigationStack &) = 0;
    };

    template <View V>
    struct Model : Concept
    {
        V view;

        Model(V view)
            : view(std::move(view)) {}

        void render(Canvas &canvas) override
        {
            view.render(canvas);
        }

        void handle(const Event &event, NavigationStack &ns) override
        {
            view.handle(event, ns);
        }
    };

    std::unique_ptr<Concept> self_;
};
