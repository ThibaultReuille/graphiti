#pragma once

#include "Entities/MVC.hh"
#include "Visualizers/Cloud/CloudView.hh"
#include "Visualizers/Cloud/CloudController.hh"

class CloudVisualizer : public EntityVisualizer
{
public:

    CloudVisualizer()
    {
    }

    virtual bool bind(const Viewport& viewport, Entity* entity)
    {
        if (entity->type() != Entity::GRAPH)
        {
            LOG("[CLOUD] Couldn't bind entity to view : Wrong entity type!\n");
            return false;
        }

        auto ts = static_cast<GraphEntity*>(entity);

        auto view = new CloudView();
        view->setViewport(viewport);
        view->bind(ts);

        entity->context()->messages().addListener(view);

        auto controller = new CloudController();
        controller->bind(static_cast<GraphContext*>(ts->context()), view);

        entity->controllers().push_back(controller);
        entity->context()->messages().addListener(controller);

        set(view);
        set(controller);

        return true;
    }
};
