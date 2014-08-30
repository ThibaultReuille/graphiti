#pragma once

#include "Entities/TimeSeries/TimeSeriesModel.hh"

class TimeSeriesContext : public EntityContext
{
};

class TimeSeriesListener : public EntityListener
{
};

class TimeSeriesView : public EntityView, public TimeSeriesListener
{
};

class TimeSeriesController : public EntityController, public TimeSeriesListener
{
};

class TimeSeriesEntity : public Entity
{
public:
    TimeSeriesEntity()
    : Entity(TIME_SERIES)
    {
        m_TimeSeriesContext = new TimeSeriesContext();
        m_TimeSeriesModel = new TimeSeriesModel();
    }

    virtual ~TimeSeriesEntity()
    {
        SAFE_DELETE(m_TimeSeriesModel);
        SAFE_DELETE(m_TimeSeriesContext);
    }

    virtual void send(const Variables& input, Variables& output)
    {
        (void) output;
        LOG("[TIMESERIES] Message :\n");
        input.dump();
    }

    virtual EntityModel* model() { return m_TimeSeriesModel; }
    virtual EntityContext* context() { return m_TimeSeriesContext; }
private:
    TimeSeriesContext* m_TimeSeriesContext;
    TimeSeriesModel* m_TimeSeriesModel;
};
