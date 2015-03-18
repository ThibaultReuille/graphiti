#pragma once

#include <raindance/Core/Headers.hh>
#include <raindance/Core/Clock.hh>
#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/Mesh.hh>
#include <raindance/Core/Primitives/Quad.hh>
#include <raindance/Core/Resources/Texture.hh>

#include <graphiti/Entities/TimeSeries/TimeSeriesEntity.hh>

class StreamView : public TimeSeriesView
{
public:
	StreamView()
	{
		LOG("[TIMESERIES] Creating view ...\n");

		m_TimeSeriesEntity = NULL;
		m_Font = new rd::Font();
	}

	virtual ~StreamView()
	{
		SAFE_DELETE(m_Font);
	}

	virtual const char* name() const { return "mesh"; }

    virtual bool bind(TimeSeriesEntity* entity)
    {
		m_Camera2D.setOrthographicProjection(0.0f, getViewport().getDimension()[0], 0.0f, getViewport().getDimension()[1], 0.001f, 100.f);
		m_Camera2D.lookAt(glm::vec3(0, 0, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        m_TimeSeriesEntity = entity;
        m_TimeSeriesEntity->views().push_back(this);
        m_TimeSeriesEntity->listeners().push_back(this);

        return true;
    }

	virtual void draw()
	{
		glClearColor(0.7, 0.2, 0.2, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	}

	virtual void idle()
	{
	}

	virtual void onSetAttribute(const std::string& name, VariableType type, const std::string& value)
	{
		(void) name;
		(void) type;
		(void) value;
	}

    virtual IVariable* getAttribute(const std::string& name)
    {
        (void) name;
        return NULL;
    }

	void notify(IMessage* message)
	{
		(void) message;
	}

    inline TimeSeriesContext* context() { return static_cast<TimeSeriesContext*>(m_TimeSeriesEntity->context()); }
    inline TimeSeriesModel* model() { return static_cast<TimeSeriesModel*>(m_TimeSeriesEntity->model()); }

private:
	TimeSeriesEntity* m_TimeSeriesEntity;

	Camera m_Camera2D;

	rd::Font* m_Font;
};

