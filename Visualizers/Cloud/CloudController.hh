#pragma once

#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/Controller.hh>
#include <raindance/Core/Context.hh>

#include <graphiti/Entities/MVC.hh>

#include <graphiti/Visualizers/Cloud/CloudWidgets.hh>

class CloudController : public GraphController
{
public:
	CloudController()
	{
		m_Font = new rd::Font();

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
        m_SliderWidget1->setValue(0.0f);

        m_WidgetGroup->add(m_SliderWidget2 = new SliderWidget("slider2", NULL, tl, glm::vec2(m_WidgetDimension.x * 5, m_WidgetDimension.y / 2)));
		tl.y -= m_WidgetSpacing + m_WidgetDimension.y / 2;
		m_SliderWidget2->setValue(0.0f);
	}

	virtual ~CloudController()
	{
		delete m_WidgetGroup;
	}

	void bind(Context* context, CloudView* view)
	{
		m_Context = context;
		m_CloudView = view;

		m_WindowWidth = (int) m_CloudView->getViewport().getDimension()[0];
		m_WindowHeight = (int) m_CloudView->getViewport().getDimension()[1];
		m_Camera.setOrthographicProjection(0.0f, (float)m_WindowWidth, 0.0f, (float)m_WindowHeight, 0.001f, 100.f);
		m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_SphericalCameraController.bind(context, view->getCamera3D());
		m_FirstPersonCameraController.bind(context, view->getCamera3D());
	}

	void onWindowSize(int width, int height) override
	{
		m_Camera.resize(width, height);
		m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_WidgetGroup->resize(width, height);

		switch(m_CloudView->getCamera3D()->mode())
		{
		case Camera::PERSPECTIVE:
			m_SphericalCameraController.onWindowSize(width, height);
			break;
		case Camera::ORTHOGRAPHIC:
			m_FirstPersonCameraController.onWindowSize(width, height);
			break;
		}
	}

	void idle(Context* context) override
	{
		(void) context;

		switch(m_CloudView->getCamera3D()->mode())
		{
		case Camera::PERSPECTIVE:
			m_SphericalCameraController.update();
			break;
		case Camera::ORTHOGRAPHIC:
			m_FirstPersonCameraController.update();
			break;
		}
	}

	void draw(Context* context) override
	{
		m_WidgetGroup->draw(context, glm::mat4(), m_Camera.getViewMatrix(), m_Camera.getProjectionMatrix());
	}

	void onKey(int key, int scancode, int action, int mods) override
	{
		switch(m_CloudView->getCamera3D()->mode())
		{
		case Camera::PERSPECTIVE:
			m_SphericalCameraController.onKey(key, scancode, action, mods);
			break;
		case Camera::ORTHOGRAPHIC:
			m_FirstPersonCameraController.onKey(key, scancode, action, mods);
			break;
		}
	}

	void onMouseDown(const glm::vec2& pos) override
	{
		switch(m_CloudView->getCamera3D()->mode())
		{
		case Camera::PERSPECTIVE:
			m_SphericalCameraController.onMouseDown(pos);
			break;
		case Camera::ORTHOGRAPHIC:
			m_FirstPersonCameraController.onMouseDown(pos);
			break;
		}
	}

	void onMouseClick(const glm::vec2& pos) override
	{
		IWidget* pickWidget = m_WidgetGroup->pickWidget(pos);
		if (pickWidget != NULL)
			pickWidget->onMouseClick(m_Context->messages(), pos);
		else
		{
			switch(m_CloudView->getCamera3D()->mode())
			{
			case Camera::PERSPECTIVE:
				m_SphericalCameraController.onMouseClick(pos);
				break;
			case Camera::ORTHOGRAPHIC:
				m_FirstPersonCameraController.onMouseClick(pos);
				break;
			}
		}
	}

	void onMouseDoubleClick(const glm::vec2& pos) override
	{
		switch(m_CloudView->getCamera3D()->mode())
		{
		case Camera::PERSPECTIVE:
			m_SphericalCameraController.onMouseDoubleClick(pos);
			break;
		case Camera::ORTHOGRAPHIC:
			m_FirstPersonCameraController.onMouseDoubleClick(pos);
			break;
		}
	}

	void onMouseTripleClick(const glm::vec2& pos) override
	{
		switch(m_CloudView->getCamera3D()->mode())
		{
		case Camera::PERSPECTIVE:
			m_SphericalCameraController.onMouseTripleClick(pos);
			break;
		case Camera::ORTHOGRAPHIC:
			m_FirstPersonCameraController.onMouseTripleClick(pos);
			break;
		}
	}

	void onMouseMove(const glm::vec2& pos, const glm::vec2& dpos) override
	{
		switch(m_CloudView->getCamera3D()->mode())
		{
		case Camera::PERSPECTIVE:
			m_SphericalCameraController.onMouseMove(pos, dpos);
			break;
		case Camera::ORTHOGRAPHIC:
			m_FirstPersonCameraController.onMouseMove(pos, dpos);
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
				m_CloudView->getPointCloud().m_FilterZ.Value = m_SliderWidget1->getValue();
			}
			else if (msg->Name == "slider2")
			{
				m_CloudView->getPointCloud().m_FilterZ.Active = true;
				m_CloudView->getPointCloud().m_FilterZ.Threshold = m_SliderWidget2->getValue();
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

private:
	Context* m_Context;
	CloudView* m_CloudView;

	int m_WindowHeight;
	int m_WindowWidth;

	Camera m_Camera;

	rd::Font* m_Font;

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

