#pragma once

#include "Entities/MVC.hh"
#include "Visualizers/Stream/StreamView.hh"
#include "Visualizers/Stream/StreamController.hh"

class StreamVisualizer : public EntityVisualizer
{
public:

    StreamVisualizer()
    {
    }

    virtual bool bind(const Viewport& viewport, Entity* entity)
    {
        if (entity->type() != Entity::TIME_SERIES)
        {
            LOG("[TIMESERIES] Couldn't bind entity to view : Wrong entity type!\n");
            return false;
        }

        auto ts = static_cast<TimeSeriesEntity*>(entity);

        auto view = new StreamView();
        view->setViewport(viewport);
        view->bind(ts);

        entity->context()->messages().addListener(view);

        auto controller = new StreamController();
        controller->bind(static_cast<GraphContext*>(ts->context()), view);

        entity->controllers().push_back(controller);
        entity->context()->messages().addListener(controller);

        set(view);
        set(controller);

        return true;
    }

private:
};
