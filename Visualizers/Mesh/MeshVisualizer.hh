#pragma once

#include "Entities/MVC.hh"
#include "Visualizers/Mesh/MeshView.hh"
#include "Visualizers/Mesh/MeshController.hh"

class MeshVisualizer : public EntityVisualizer
{
public:

    MeshVisualizer()
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

        auto view = new MeshView();
        view->setViewport(viewport);
        view->bind(ts);

        entity->context()->messages().addListener(view);

        auto controller = new MeshController();
        controller->bind(static_cast<GraphContext*>(ts->context()), view);

        entity->controllers().push_back(controller);
        entity->context()->messages().addListener(controller);

        set(view);
        set(controller);

        return true;
    }
};
