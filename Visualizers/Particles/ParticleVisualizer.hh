#pragma once

#include "Entities/MVC.hh"
#include "Visualizers/Particles/ParticleView.hh"
#include "Visualizers/Particles/ParticleController.hh"

class ParticleVisualizer : public EntityVisualizer
{
public:

    ParticleVisualizer()
    {
    }

    virtual bool bind(const Viewport& viewport, Entity* entity)
    {
        if (entity->type() != Entity::GRAPH)
        {
            LOG("[SPACE] Couldn't bind entity to visualizer : Wrong entity type!\n");
            return false;
        }

        auto graph = static_cast<GraphEntity*>(entity);

        auto view = new ParticleView();
        view->setViewport(viewport);
        view->bind(graph);

        graph->context()->messages().addListener(view);

        auto controller = new ParticleController();
        controller->bind(static_cast<GraphContext*>(graph->context()), graph->model(), view);
        entity->controllers().push_back(controller);
        entity->context()->messages().addListener(controller);

        set(view);
        set(controller);

        return true;
    }
};
