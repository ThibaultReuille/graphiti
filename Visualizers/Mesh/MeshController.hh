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

		m_Camera.setOrthographicProjection(0.0f, view->getViewport().getDimension()[0], 0.0f, view->getViewport().getDimension()[1], 0.001f, 100.f);
		m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		
		m_SphericalCameraController.bind(context, view->getCamera3D());
		m_FirstPersonCameraController.bind(context, view->getCamera3D());
	}

	void onWindowSize(int width, int height) override
	{
		m_Camera.reshape(width, height);
		m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_WidgetGroup->reshape(width, height);

		switch(m_MeshView->getCamera3D()->mode())
		{
		case Camera::PERSPECTIVE:
			m_SphericalCameraController.onWindowSize(width, height);
			break;
		case Camera::ORTHOGRAPHIC:
			m_FirstPersonCameraController.onWindowSize(width, height);
			break;
		}
	}

	void idle() override
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

	void draw() override
	{
		m_WidgetGroup->draw(m_Context, glm::mat4(), m_Camera.getViewMatrix(), m_Camera.getProjectionMatrix());
	}

	void onKey(int key, int scancode, int action, int mods) override
	{
		switch(m_MeshView->getCamera3D()->mode())
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
		switch(m_MeshView->getCamera3D()->mode())
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
			switch(m_MeshView->getCamera3D()->mode())
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
		switch(m_MeshView->getCamera3D()->mode())
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
		switch(m_MeshView->getCamera3D()->mode())
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
		switch(m_MeshView->getCamera3D()->mode())
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
		(void) message;
	}

private:
	Context* m_Context;
	MeshView* m_MeshView;

	Camera m_Camera;

	Font* m_Font;

	glm::vec2 m_WidgetDimension;
	float m_WidgetSpacing;
	WidgetGroup* m_WidgetGroup;

	SphericalCameraController m_SphericalCameraController;
	FirstPersonCameraController m_FirstPersonCameraController;
};

