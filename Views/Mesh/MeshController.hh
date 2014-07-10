#pragma once

#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/Controller.hh>
#include <raindance/Core/Context.hh>

#include "Entities/MVC.hh"

class MeshController : public GraphController
{
public:
	MeshController()
	{
		m_WindowWidth = glutGet(GLUT_WINDOW_WIDTH);
		m_WindowHeight = glutGet(GLUT_WINDOW_HEIGHT);
		m_Camera.setOrthographicProjection(0.0f, (float)m_WindowWidth, 0.0f, (float)m_WindowHeight, 0.001f, 100.f);
		m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_Font = new Font();

        m_WidgetGroup = new WidgetGroup("mesh controller group");
        m_WidgetSpacing = 10;
		m_WidgetDimension = glm::vec2(16, 16);

		/*
        glm::vec2 sliderDimension = glm::vec2(160, 16);
		glm::vec3 center = glm::vec3(m_WidgetSpacing + m_WidgetDimension.x / 2, - m_WidgetSpacing - m_WidgetDimension.y / 2, 0);

		m_WidgetGroup->add(m_ViewWidget =     new ViewWidget(NULL, center, m_WidgetDimension));    center.x += m_WidgetSpacing + m_WidgetDimension.x / 2;
		m_WidgetGroup->add(m_ViewTextWidget = new TextWidget("view text", NULL, center, glm::vec2(1.0, 1.0)));  center -= glm::vec3(m_WidgetSpacing + m_WidgetDimension.x / 2, m_WidgetSpacing + m_WidgetDimension.y, 0);
		m_ViewTextWidget->text().set("Perspective", m_Font);

		center.y -= m_WidgetSpacing + m_WidgetDimension.y;
		center.x = m_WidgetSpacing + sliderDimension.x / 2;

		m_WidgetGroup->add(m_SliderWidget1 = new SliderWidget("slider1", NULL, center, sliderDimension)); center.y -= m_WidgetSpacing + sliderDimension.y;
		m_WidgetGroup->add(m_SliderWidget2 = new SliderWidget("slider2", NULL, center, sliderDimension)); center.y -= m_WidgetSpacing + sliderDimension.y;
		m_SliderWidget1->value(0.0f);
		m_SliderWidget2->value(0.0f);
		*/
	}

	virtual ~MeshController()
	{
		delete m_WidgetGroup;
	}

	void bind(Context* context, MeshView* view)
	{
		m_Context = context;
		m_MeshView = view;

		m_SphericalCameraController.bind(context, view->getCamera3D());
		m_FirstPersonCameraController.bind(context, view->getCamera3D());
	}

	virtual void reshape(int width, int height)
	{
		m_Camera.reshape(width, height);
		m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_WidgetGroup->reshape(width, height);

		switch(m_MeshView->getCamera3D()->mode())
		{
		case Camera::PERSPECTIVE:
			m_SphericalCameraController.reshape(width, height);
			break;
		case Camera::ORTHOGRAPHIC:
			m_FirstPersonCameraController.reshape(width, height);
			break;
		}
	}

	virtual void idle()
	{
		switch(m_MeshView->getCamera3D()->mode())
		{
		case Camera::PERSPECTIVE:
			m_SphericalCameraController.updateCamera();
			break;
		case Camera::ORTHOGRAPHIC:
			m_FirstPersonCameraController.updateCamera();
			break;
		}
	}

	virtual void draw()
	{
		m_WidgetGroup->draw(m_Context, glm::mat4(), m_Camera.getViewMatrix(), m_Camera.getProjectionMatrix());
	}

	virtual void onKeyboard(unsigned char key, Controller::KeyEvent event)
	{
		switch(m_MeshView->getCamera3D()->mode())
		{
		case Camera::PERSPECTIVE:
			m_SphericalCameraController.onKeyboard(key, event);
			break;
		case Camera::ORTHOGRAPHIC:
			m_FirstPersonCameraController.onKeyboard(key, event);
			break;
		}
	}

	virtual void onMouseDown(int x, int y)
	{
		switch(m_MeshView->getCamera3D()->mode())
		{
		case Camera::PERSPECTIVE:
			m_SphericalCameraController.onMouseDown(x, y);
			break;
		case Camera::ORTHOGRAPHIC:
			m_FirstPersonCameraController.onMouseDown(x, y);
			break;
		}
	}

	virtual void onMouseClick(int x, int y)
	{
		IWidget* pickWidget = m_WidgetGroup->pickWidget(x, y);
		if (pickWidget != NULL)
			pickWidget->onMouseClick(m_Context->messages(), x, y);
		else
		{
			switch(m_MeshView->getCamera3D()->mode())
			{
			case Camera::PERSPECTIVE:
				m_SphericalCameraController.onMouseClick(x, y);
				break;
			case Camera::ORTHOGRAPHIC:
				m_FirstPersonCameraController.onMouseClick(x, y);
				break;
			}
		}
	}

	virtual void onMouseDoubleClick(int x, int y)
	{
		switch(m_MeshView->getCamera3D()->mode())
		{
		case Camera::PERSPECTIVE:
			m_SphericalCameraController.onMouseDoubleClick(x, y);
			break;
		case Camera::ORTHOGRAPHIC:
			m_FirstPersonCameraController.onMouseDoubleClick(x, y);
			break;
		}
	}

	virtual void onMouseTripleClick(int x, int y)
	{
		switch(m_MeshView->getCamera3D()->mode())
		{
		case Camera::PERSPECTIVE:
			m_SphericalCameraController.onMouseTripleClick(x, y);
			break;
		case Camera::ORTHOGRAPHIC:
			m_FirstPersonCameraController.onMouseTripleClick(x, y);
			break;
		}
	}

	virtual void onMouseMove(int x, int y, int dx, int dy)
	{
		switch(m_MeshView->getCamera3D()->mode())
		{
		case Camera::PERSPECTIVE:
			m_SphericalCameraController.onMouseMove(x, y, dx, dy);
			break;
		case Camera::ORTHOGRAPHIC:
			m_FirstPersonCameraController.onMouseMove(x, y, dx, dy);
			break;
		}
	}

	virtual void onSpecial(int key, Controller::KeyEvent event)
	{
		switch(m_MeshView->getCamera3D()->mode())
		{
		case Camera::PERSPECTIVE:
			m_SphericalCameraController.onSpecial(key, event);
			break;
		case Camera::ORTHOGRAPHIC:
			m_FirstPersonCameraController.onSpecial(key, event);
			break;
		}
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

private:
	Context* m_Context;
	MeshView* m_MeshView;

	int m_WindowHeight;
	int m_WindowWidth;

	Camera m_Camera;

	Font* m_Font;

	glm::vec2 m_WidgetDimension;
	float m_WidgetSpacing;
	WidgetGroup* m_WidgetGroup;

	SphericalCameraController m_SphericalCameraController;
	FirstPersonCameraController m_FirstPersonCameraController;
};

