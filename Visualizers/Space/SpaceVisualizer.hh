#pragma once

#include <graphiti/Entities/MVC.hh>
#include <graphiti/Visualizers/Space/SpaceView.hh>
#include <graphiti/Visualizers/Space/SpaceController.hh>
#include <graphiti/Visualizers/Space/SpaceResources.hh>

class SpaceVisualizer : public EntityVisualizer
{
public:

    SpaceVisualizer()
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

        auto view = new SpaceView();
        view->setViewport(viewport);
        view->bind(graph);

        entity->context()->messages().addListener(view);

        auto controller = new SpaceController();
        controller->bind(static_cast<GraphContext*>(graph->context()), graph->model(), view);

        entity->controllers().push_back(controller);
        entity->listeners().push_back(controller);
        entity->context()->messages().addListener(controller);

        set(view);
        set(controller);

        return true;
    }
};
