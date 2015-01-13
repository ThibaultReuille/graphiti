#pragma once

#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/Controller.hh>
#include <raindance/Core/Context.hh>

#include "Entities/TimeSeries/TimeSeriesEntity.hh"

class StreamController : public TimeSeriesController
{
public:
	StreamController()
	{
		m_Font = new Font();

        m_WidgetGroup = new WidgetGroup("Stream/Controller/Widgets");
        m_WidgetSpacing = 10;
		m_WidgetDimension = glm::vec2(16, 16);
	}

	virtual ~StreamController()
	{
		SAFE_DELETE(m_WidgetGroup);
	}

	void bind(Context* context, TimeSeriesView* view)
	{
		m_Camera.setOrthographicProjection(0.0f, view->getViewport().getDimension()[0], 0.0f, view->getViewport().getDimension()[1], 0.001f, 100.f);
		m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_Context = context;
		m_TimeSeriesView = view;
	}

	void onWindowSize(int width, int height) override
	{
		m_Camera.reshape(width, height);
		m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_WidgetGroup->reshape(width, height);
	}

	void draw() override
	{
		m_WidgetGroup->draw(m_Context, glm::mat4(), m_Camera.getViewMatrix(), m_Camera.getProjectionMatrix());
	}

	void onMouseClick(const glm::vec2& pos) override
	{
		IWidget* pickWidget = m_WidgetGroup->pickWidget(pos);
		if (pickWidget != NULL)
			pickWidget->onMouseClick(m_Context->messages(), pos);
	}

	void notify(IMessage* message)
	{ 
		(void) message;
	}

private:
	Context* m_Context;
	TimeSeriesView* m_TimeSeriesView;

	Camera m_Camera;

	Font* m_Font;

	glm::vec2 m_WidgetDimension;
	float m_WidgetSpacing;
	WidgetGroup* m_WidgetGroup;
};

