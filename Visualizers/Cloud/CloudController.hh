#pragma once

#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/Controller.hh>
#include <raindance/Core/Context.hh>

#include "Entities/MVC.hh"

#include "Visualizers/Cloud/CloudWidgets.hh"

class CloudController : public GraphController
{
public:
	CloudController()
	{
		m_WindowWidth = glutGet(GLUT_WINDOW_WIDTH);
		m_WindowHeight = glutGet(GLUT_WINDOW_HEIGHT);
		m_Camera.setOrthographicProjection(0.0f, (float)m_WindowWidth, 0.0f, (float)m_WindowHeight, 0.001f, 100.f);
		m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_Font = new Font();

		m_WidgetSpacing = 10;
		m_WidgetDimension = glm::vec2(24, 24);
		float textWidgetWidth = 120;

		m_WidgetGroup = new WidgetGroup("cloud controller group", WidgetGroup::TOP_LEFT);

		glm::vec3 tl = glm::vec3(m_WidgetSpacing, - m_WidgetSpacing, 0);

		m_WidgetGroup->add(m_ViewWidget = new ViewWidget(NULL, tl, m_WidgetDimension));
		tl.x += m_WidgetSpacing + m_WidgetDimension.x;
		m_WidgetGroup->add(m_ViewTextWidget = new TextWidget("view text", NULL, tl, glm::vec2(textWidgetWidth, m_WidgetDimension.y)));
		tl -= glm::vec3(m_WidgetSpacing + m_WidgetDimension.x, m_WidgetSpacing + m_WidgetDimension.y, 0);
		m_ViewTextWidget->text().set("Perspective", m_Font);

        tl.x = m_WidgetSpacing;

		m_WidgetGroup->add(m_SliderWidget1 = new SliderWidget("slider1", NULL, tl, glm::vec2(m_WidgetDimension.x * 5, m_WidgetDimension.y / 2)));
		tl.y -= m_WidgetSpacing + m_WidgetDimension.y / 2;
        m_SliderWidget1->value(0.0f);

        m_WidgetGroup->add(m_SliderWidget2 = new SliderWidget("slider2", NULL, tl, glm::vec2(m_WidgetDimension.x * 5, m_WidgetDimension.y / 2)));
		tl.y -= m_WidgetSpacing + m_WidgetDimension.y / 2;
		m_SliderWidget2->value(0.0f);
	}

	virtual ~CloudController()
	{
		delete m_WidgetGroup;
	}

	void bind(Context* context, CloudView* view)
	{
		m_Context = context;
		m_CloudView = view;

		m_SphericalCameraController.bind(context, view->getCamera3D());
		m_FirstPersonCameraController.bind(context, view->getCamera3D());
	}

	virtual void reshape(int width, int height)
	{
		m_Camera.reshape(width, height);
		m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_WidgetGroup->reshape(width, height);

		switch(m_CloudView->getCamera3D()->mode())
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
		switch(m_CloudView->getCamera3D()->mode())
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
		switch(m_CloudView->getCamera3D()->mode())
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
		switch(m_CloudView->getCamera3D()->mode())
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
			switch(m_CloudView->getCamera3D()->mode())
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
		switch(m_CloudView->getCamera3D()->mode())
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
		switch(m_CloudView->getCamera3D()->mode())
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
		switch(m_CloudView->getCamera3D()->mode())
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
		switch(m_CloudView->getCamera3D()->mode())
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
		IMessage::Type type = message->type();

		if (type == IMessage::WIDGET)
		{
			WidgetMessage* msg = static_cast<WidgetMessage*>(message);

			if (msg->Name == "slider1" && msg->Message == "update")
			{
				m_CloudView->getPointCloud().m_FilterZ.Active = true;
				m_CloudView->getPointCloud().m_FilterZ.Value = m_SliderWidget1->value();
			}
			else if (msg->Name == "slider2")
			{
				m_CloudView->getPointCloud().m_FilterZ.Active = true;
				m_CloudView->getPointCloud().m_FilterZ.Threshold = m_SliderWidget2->value();
			}
			else if (msg->Name == "view")
			{
				if (msg->Message == "perspective")
				{
					m_CloudView->getCamera3D()->setPerspectiveProjection(60.0f, m_WindowWidth / (float)m_WindowHeight, 0.1f, 1024.0f);
					m_ViewTextWidget->text().set("Perspective", m_Font);
				}
				else
				{
					glm::vec3 diff = m_CloudView->getPointCloud().box().max() - m_CloudView->getPointCloud().box().min();
					float max = glm::length(diff);
					m_CloudView->getCamera3D()->setOrthographicProjection(-(float)max / 3, (float)max / 3, -(float)max / 3, (float)max / 3, 0.001f, 1024.f);

					if (msg->Message == "top")
					{
						m_CloudView->getCamera3D()->lookAt(glm::vec3(0, m_CloudView->getPointCloud().box().max().y + 1.0f, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));
						m_ViewTextWidget->text().set("Top", m_Font);
					}
					else if (msg->Message == "bottom")
					{
						m_CloudView->getCamera3D()->lookAt(glm::vec3(0, m_CloudView->getPointCloud().box().min().y - 1.0f, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
						m_ViewTextWidget->text().set("Bottom", m_Font);
					}
					else if (msg->Message == "left")
					{
						m_CloudView->getCamera3D()->lookAt(glm::vec3(m_CloudView->getPointCloud().box().min().x - 1.0f, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
						m_ViewTextWidget->text().set("Left", m_Font);
					}
					else if (msg->Message == "right")
					{
						m_CloudView->getCamera3D()->lookAt(glm::vec3(m_CloudView->getPointCloud().box().max().x + 1.0f, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
						m_ViewTextWidget->text().set("Right", m_Font);
					}
					else if (msg->Message == "front")
					{
						m_CloudView->getCamera3D()->lookAt(glm::vec3(0, 0, m_CloudView->getPointCloud().box().max().z + 1.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
						m_ViewTextWidget->text().set("Front", m_Font);
					}
					else if (msg->Message == "back")
					{
						m_CloudView->getCamera3D()->lookAt(glm::vec3(0, 0, m_CloudView->getPointCloud().box().min().z - 1.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
						m_ViewTextWidget->text().set("Back", m_Font);
					}
				}
			}
		}
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
	CloudView* m_CloudView;

	int m_WindowHeight;
	int m_WindowWidth;

	Camera m_Camera;

	Font* m_Font;

	glm::vec2 m_WidgetDimension;
	float m_WidgetSpacing;
	WidgetGroup* m_WidgetGroup;

	SliderWidget* m_SliderWidget1;
	SliderWidget* m_SliderWidget2;
	ViewWidget* m_ViewWidget;
	TextWidget* m_ViewTextWidget;

	SphericalCameraController m_SphericalCameraController;
	FirstPersonCameraController m_FirstPersonCameraController;
};

