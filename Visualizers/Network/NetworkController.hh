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

		m_CameraController.bind(context, view->cameras());
		#ifdef RD_OCULUS_RIFT
			m_CameraController.select(CameraController::OCULUS_RIFT);
		#else
			m_CameraController.select(CameraController::SPHERICAL);
		#endif
	}

	void onResize(const Viewport& viewport) override
	{
		auto dimension = viewport.getDimension();

        m_Camera.resize(dimension.x, dimension.y);
        m_Camera.setOrthographicProjection(0.0f, dimension.x, 0.0f, dimension.y, 0.001f, 100.f);
        m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_CameraController.onResize(viewport);
	}

	void idle(Context* context) override
	{
		(void) context;

		m_CameraController.update();
	}

	void onKey(int key, int scancode, int action, int mods) override
	{
		m_CameraController.onKey(key, scancode, action, mods);
	}

	void onMouseDown(const glm::vec2& pos) override
	{
		m_CameraController.onMouseDown(pos);
	}

	void onMouseClick(const glm::vec2& pos) override
	{
		m_CameraController.onMouseClick(pos);
	}

	void onMouseDoubleClick(const glm::vec2& pos) override
	{
		m_CameraController.onMouseDoubleClick(pos);
	}

	void onMouseTripleClick(const glm::vec2& pos) override
	{
		m_CameraController.onMouseTripleClick(pos);
	}

	void onMouseMove(const glm::vec2& pos, const glm::vec2& dpos) override
	{
		m_CameraController.onMouseMove(pos, dpos);
	}

	void onScroll(double xoffset, double yoffset) override
	{
		m_CameraController.onScroll(xoffset, yoffset);
	}

	void notify(IMessage* message)
	{
		(void) message;
	}

private:
	Context* m_Context;
	NetworkView* m_NetworkView;

	Camera m_Camera;
	CameraController m_CameraController;

	rd::Font* m_Font;
};

