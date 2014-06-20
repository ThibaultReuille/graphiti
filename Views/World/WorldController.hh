#pragma once

#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/Controller.hh>
#include <raindance/Core/Context.hh>

#include "Entities/MVC.hh"

#include "Views/World/WorldWidgets.hh"

class WorldController : public GraphController
{
public:
	WorldController()
	{
		m_WindowWidth = glutGet(GLUT_WINDOW_WIDTH);
		m_WindowHeight = glutGet(GLUT_WINDOW_HEIGHT);
		m_Camera.setOrthographicProjection(0.0f, (float)m_WindowWidth, 0.0f, (float)m_WindowHeight, 0.001f, 100.f);
		m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

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

		m_SphericalCameraController.bind(context, view->getCamera3D());
		m_SphericalCameraController.updateCamera();
	}

	virtual void reshape(int width, int height)
	{
		m_Camera.reshape(width, height);
		m_Camera.setOrthographicProjection(0.0f, (float)width, 0.0f, (float)height, 0.001f, 100.f);
		m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_WidgetGroup->reshape(width, height);

		m_SphericalCameraController.reshape(width, height);
	}

	virtual void idle()
	{
		m_SphericalCameraController.updateCamera();
	}

	virtual void draw()
	{
		m_WidgetGroup->draw(m_Context, glm::mat4(), m_Camera.getViewMatrix(), m_Camera.getProjectionMatrix());
	}

	virtual void onKeyboard(unsigned char key, Controller::KeyEvent event)
	{
		m_SphericalCameraController.onKeyboard(key, event);
	}

	virtual void onMouseDown(int x, int y)
	{
		m_SphericalCameraController.onMouseDown(x, y);
	}

	virtual void onMouseClick(int x, int y)
	{
		IWidget* pickWidget = m_WidgetGroup->pickWidget(x, y);
		if (pickWidget != NULL)
			pickWidget->onMouseClick(m_Context->messages(), x, y);
		else
			m_SphericalCameraController.onMouseClick(x, y);
	}

	virtual void onMouseDoubleClick(int x, int y)
	{
		m_SphericalCameraController.onMouseDoubleClick(x, y);
	}

	virtual void onMouseTripleClick(int x, int y)
	{
		m_SphericalCameraController.onMouseTripleClick(x, y);
	}

	virtual void onMouseMove(int x, int y, int dx, int dy)
	{
		m_SphericalCameraController.onMouseMove(x, y, dx, dy);
	}

	virtual void onSpecial(int key, Controller::KeyEvent event)
	{
		m_SphericalCameraController.onSpecial(key, event);
	}

	void notify(IMessage* message)
	{
		(void) message;
	}

    // GraphListener Interface

    virtual void onSetAttribute(const std::string& name, VariableType type, const std::string& value)
    {
        (void) name;
        (void) type;
        (void) value;
    }

    virtual void onAddNode(Node::ID id, const char* label)
    {
        (void) id;
        (void) label;
    }

    virtual void onRemoveNode(Node::ID id)
    {
        (void) id;
    }

    virtual void onSetNodeAttribute(Node::ID id, const std::string& name, VariableType type, const std::string& value)
    {
        (void) id;
        (void) name;
        (void) type;
        (void) value;
    }

    virtual void onSetNodeLabel(Node::ID id, const char* label)
    {
        (void) id;
        (void) label;
    }

    virtual void onSetNodeMark(Node::ID id, unsigned int mark)
    {
        (void) id;
        (void) mark;
    }

    virtual void onSetNodeWeight(Node::ID id, float weight)
    {
        (void) id;
        (void) weight;
    }

    virtual void onTagNode(Node::ID node, Sphere::ID sphere)
    {
        (void) node;
        (void) sphere;
    }

    virtual void onAddLink(Link::ID uid, Node::ID uid1, Node::ID uid2)
    {
        (void) uid;
        (void) uid1;
        (void) uid2;
    }

    virtual void onRemoveLink(Link::ID id)
    {
        (void) id;
    }

    virtual void onSetLinkAttribute(Link::ID id, const std::string& name, VariableType type, const std::string& value)
    {
        (void) id;
        (void) name;
        (void) type;
        (void) value;
    }

    virtual void onAddNeighbor(const std::pair<Node::ID, Link::ID>& element, const char* label, Node::ID neighbor)
    {
        (void) element;
        (void) label;
        (void) neighbor;
    }

    virtual void onAddSphere(Sphere::ID id, const char* label)
    {
        (void) id;
        (void) label;
    }

    virtual void onSetSphereMark(Sphere::ID id, unsigned int mark)
    {
        (void) id;
        (void) mark;
    }

private:
	Context* m_Context;
	WorldView* m_WorldView;

	int m_WindowHeight;
	int m_WindowWidth;

	Camera m_Camera;

	glm::vec2 m_WidgetDimension;
	float m_WidgetSpacing;
	WidgetGroup* m_WidgetGroup;

	// EarthWidget* m_EarthWidget;
	// WorldMapWidget* m_WorldMapWidget;

	SphericalCameraController m_SphericalCameraController;
};

