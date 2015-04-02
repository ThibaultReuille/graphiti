#pragma once

#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/Controller.hh>
#include <raindance/Core/Context.hh>

#include <graphiti/Entities/MVC.hh>

class NetworkController : public GraphController
{
public:
	NetworkController()
	{
		m_Font = new rd::Font();
	}

	virtual ~NetworkController()
	{
	}

	void bind(Context* context, NetworkView* view)
	{
		m_Context = context;
		m_NetworkView = view;

		m_Camera.setOrthographicProjection(0.0f, view->getViewport().getDimension()[0], 0.0f, view->getViewport().getDimension()[1], 0.001f, 100.f);
		m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_SphericalCameraController.bind(context, view->getCamera3D());
		m_FirstPersonCameraController.bind(context, view->getCamera3D());
	}

	void onResize(const Viewport& viewport) override
	{
		auto dimension = viewport.getDimension();

        m_Camera.resize(dimension.x, dimension.y);
        m_Camera.setOrthographicProjection(0.0f, dimension.x, 0.0f, dimension.y, 0.001f, 100.f);
        m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		switch(m_NetworkView->getCamera3D()->mode())
		{
		case Camera::PERSPECTIVE:
			m_SphericalCameraController.onResize(viewport);
			break;
		case Camera::ORTHOGRAPHIC:
			m_FirstPersonCameraController.onResize(viewport);
			break;
		}
	}

	void idle() override
	{
		switch(m_NetworkView->getCamera3D()->mode())
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
	}

	void onKey(int key, int scancode, int action, int mods) override
	{
		switch(m_NetworkView->getCamera3D()->mode())
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
		switch(m_NetworkView->getCamera3D()->mode())
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
		//IWidget* pickWidget = m_WidgetGroup->pickWidget(pos);
		//if (pickWidget != NULL)
		//	pickWidget->onMouseClick(m_Context->messages(), pos);
		//else
		{
			switch(m_NetworkView->getCamera3D()->mode())
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
		switch(m_NetworkView->getCamera3D()->mode())
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
		switch(m_NetworkView->getCamera3D()->mode())
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
		switch(m_NetworkView->getCamera3D()->mode())
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
	NetworkView* m_NetworkView;

	Camera m_Camera;

	rd::Font* m_Font;

	SphericalCameraController m_SphericalCameraController;
	FirstPersonCameraController m_FirstPersonCameraController;
};

