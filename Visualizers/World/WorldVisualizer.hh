#pragma once

#include "Entities/MVC.hh"
#include "Visualizers/World/WorldView.hh"
#include "Visualizers/World/WorldController.hh"
#include "Visualizers/World/WorldResources.hh"

class WorldVisualizer : public EntityVisualizer
{
public:

    WorldVisualizer()
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

        auto view = new WorldView();
        view->setViewport(viewport);
        if (!view->bind(graph))
        {
            SAFE_DELETE(view);
            return false;
        }

        entity->context()->messages().addListener(view);

        auto controller = new WorldController();
        controller->bind(static_cast<GraphContext*>(graph->context()), view);

        entity->controllers().push_back(controller);
        entity->context()->messages().addListener(controller);

        set(view);
        set(controller);

        return true;
    }

private:
};
