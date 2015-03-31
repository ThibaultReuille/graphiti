#pragma once

#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/Controller.hh>
#include <raindance/Core/Context.hh>

#include <graphiti/Entities/MVC.hh>

#include <graphiti/Visualizers/World/WorldWidgets.hh>

class WorldController : public GraphController
{
public:
	WorldController()
	{
		m_WidgetDimension = glm::vec2(32, 32);
		m_WidgetSpacing = 10;

		// glm::vec3 center = glm::vec3(m_WidgetSpacing + m_WidgetDimension.x / 2, - m_WidgetSpacing - m_WidgetDimension.y / 2, 0);
		m_WidgetGroup = new WidgetGroup("world controller group");
		// m_WidgetGroup->add(m_EarthWidget =    new EarthWidget    (NULL, center, m_WidgetDimension)); center.y -= m_WidgetSpacing + m_WidgetDimension.y;
		// m_WidgetGroup->add(m_WorldMapWidget = new WorldMapWidget (NULL, center, m_WidgetDimension));
	}

	virtual ~WorldController()
	{
		delete m_WidgetGroup;
	}

	void bind(Context* context, WorldView* view)
	{
		m_Context = context;
		m_WorldView = view;

		m_Camera.setOrthographicProjection(0.0f, view->getViewport().getDimension()[0], 0.0f, view->getViewport().getDimension()[1], 0.001f, 100.f);
		m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_SphericalCameraController.bind(context, view->getCamera3D());
		m_SphericalCameraController.updateCamera();
	}

	void idle() override
	{
		m_SphericalCameraController.updateCamera();
	}

	virtual void draw()
	{
		m_WidgetGroup->draw(m_Context, glm::mat4(), m_Camera.getViewMatrix(), m_Camera.getProjectionMatrix());
	}

	void onWindowSize(int width, int height) override
	{
		m_Camera.resize(width, height);
		m_Camera.setOrthographicProjection(0.0f, (float)width, 0.0f, (float)height, 0.001f, 100.f);
		m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_WidgetGroup->reshape(width, height);

		m_SphericalCameraController.onWindowSize(width, height);
	}

	void onKey(int key, int scancode, int action, int mods) override
	{
		m_SphericalCameraController.onKey(key, scancode, action, mods);
	}

	void onScroll(double xoffset, double yoffset) override
	{
		m_SphericalCameraController.onScroll(xoffset, yoffset);
	}

	void onMouseDown(const glm::vec2& pos) override
	{
		m_SphericalCameraController.onMouseDown(pos);
	}

	void onMouseClick(const glm::vec2& pos) override
	{
		IWidget* pickWidget = m_WidgetGroup->pickWidget(pos);
		if (pickWidget != NULL)
			pickWidget->onMouseClick(m_Context->messages(), pos);
		else
			m_SphericalCameraController.onMouseClick(pos);
	}

	void onMouseDoubleClick(const glm::vec2& pos) override
	{
		m_SphericalCameraController.onMouseDoubleClick(pos);
	}

	void onMouseTripleClick(const glm::vec2& pos) override
	{
		m_SphericalCameraController.onMouseTripleClick(pos);
	}

	void onMouseMove(const glm::vec2& pos, const glm::vec2& dpos) override
	{
		m_SphericalCameraController.onMouseMove(pos, dpos);
	}

	void notify(IMessage* message)
	{
		(void) message;
	}

private:
	Context* m_Context;
	WorldView* m_WorldView;

	Camera m_Camera;

	glm::vec2 m_WidgetDimension;
	float m_WidgetSpacing;
	WidgetGroup* m_WidgetGroup;

	// EarthWidget* m_EarthWidget;
	// WorldMapWidget* m_WorldMapWidget;

	SphericalCameraController m_SphericalCameraController;
};

