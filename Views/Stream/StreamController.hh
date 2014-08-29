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
		m_WindowWidth = glutGet(GLUT_WINDOW_WIDTH);
		m_WindowHeight = glutGet(GLUT_WINDOW_HEIGHT);
		m_Camera.setOrthographicProjection(0.0f, (float)m_WindowWidth, 0.0f, (float)m_WindowHeight, 0.001f, 100.f);
		m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_Font = new Font();

        m_WidgetGroup = new WidgetGroup("TimeSeries/Controller/Widgets");
        m_WidgetSpacing = 10;
		m_WidgetDimension = glm::vec2(16, 16);
	}

	virtual ~StreamController()
	{
		SAFE_DELETE(m_WidgetGroup);
	}

	void bind(Context* context, TimeSeriesView* view)
	{
		m_Context = context;
		m_TimeSeriesView = view;
	}

	virtual void reshape(int width, int height)
	{
		m_Camera.reshape(width, height);
		m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_WidgetGroup->reshape(width, height);
	}

	virtual void idle()
	{
	}

	virtual void draw()
	{
		m_WidgetGroup->draw(m_Context, glm::mat4(), m_Camera.getViewMatrix(), m_Camera.getProjectionMatrix());
	}

	virtual void onKeyboard(unsigned char key, Controller::KeyEvent event)
	{
		(void) key;
		(void) event;
	}

	virtual void onMouseDown(int x, int y)
	{
		(void) x;
		(void) y;
	}

	virtual void onMouseClick(int x, int y)
	{
		IWidget* pickWidget = m_WidgetGroup->pickWidget(x, y);
		if (pickWidget != NULL)
			pickWidget->onMouseClick(m_Context->messages(), x, y);
	}

	virtual void onMouseDoubleClick(int x, int y)
	{
		(void) x;
		(void) y;
	}

	virtual void onMouseTripleClick(int x, int y)
	{
		(void) x;
		(void) y;
	}

	virtual void onMouseMove(int x, int y, int dx, int dy)
	{
		(void) x;
		(void) y;
		(void) dx;
		(void) dy;
	}

	virtual void onSpecial(int key, Controller::KeyEvent event)
	{
		(void) key;
		(void) event;
	}

	void notify(IMessage* message)
	{
		(void) message;
	}

private:
	Context* m_Context;
	TimeSeriesView* m_TimeSeriesView;

	int m_WindowHeight;
	int m_WindowWidth;

	Camera m_Camera;

	Font* m_Font;

	glm::vec2 m_WidgetDimension;
	float m_WidgetSpacing;
	WidgetGroup* m_WidgetGroup;
};

