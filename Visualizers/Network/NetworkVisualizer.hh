#pragma once

#include "Entities/MVC.hh"
#include "Visualizers/Network/NetworkView.hh"
#include "Visualizers/Network/NetworkController.hh"

class NetworkVisualizer : public EntityVisualizer
{
public:

    NetworkVisualizer()
    {
    }

    virtual bool bind(const Viewport& viewport, Entity* entity)
    {
        if (entity->type() != Entity::GRAPH)
        {
            LOG("[MESH] Couldn't bind entity to view : Wrong entity type!\n");
            return false;
        }

        auto ts = static_cast<GraphEntity*>(entity);

        auto view = new NetworkView();
        view->setViewport(viewport);
        view->bind(ts);

        entity->context()->messages().addListener(view);

        auto controller = new NetworkController();
        controller->bind(static_cast<GraphContext*>(ts->context()), view);

        entity->controllers().push_back(controller);
        entity->context()->messages().addListener(controller);

        set(view);
        set(controller);

        return true;
    }
};
