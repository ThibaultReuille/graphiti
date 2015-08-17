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
		m_HasPick = false;
	}

	virtual ~NetworkController()
	{
	}

	void bind(Context* context, NetworkView* view)
	{
		m_Context = context;
		m_NetworkView = view;

		m_CameraController.bind(context, view->cameras());
		#ifdef RD_OCULUS_RIFT
			m_CameraController.select(CameraController::OCULUS_RIFT);
		#else
			m_CameraController.select(CameraController::FIRST_PERSON);
		#endif
	}

	void onResize(const Viewport& viewport) override
	{
		m_Viewport = viewport;
		m_CameraController.onResize(viewport);
	}

	void idle(Context* context) override
	{
		(void) context;

		m_CameraController.update();
	}

    glm::vec2 convertToWindowCoords(const glm::vec2& pos)
    {
        // NOTE: Convert viewport to window coordinates
        // Eventually, Controllers will be documents and
        // everything will use viewport coordinates,
        // This is temporary hack. Remove when possible.

        auto framebuffer = m_Viewport.getFramebuffer();

        auto ratio = glm::vec2(
            m_Viewport.getDimension().x / framebuffer.Width,
            m_Viewport.getDimension().y / framebuffer.Height
        );

        return glm::vec2(pos.x * ratio.x, (framebuffer.Height - pos.y) * ratio.y);
    }

	void onKey(int key, int scancode, int action, int mods) override
	{
		m_CameraController.onKey(key, scancode, action, mods);
	}

	void onMouseDown(const glm::vec2& viewport_pos) override
	{
		auto pos = convertToWindowCoords(viewport_pos); // TODO: Hack! Remove when possible.

		m_CameraController.onMouseDown(pos);
	}

	void onMouseClick(const glm::vec2& viewport_pos) override
	{
		auto pos = convertToWindowCoords(viewport_pos); // TODO: Hack! Remove when possible.
		
		m_CameraController.onMouseClick(pos);
		
		m_SelectedNodes.clear();

		GPUGraph::NodeInstance::ID nid;
		m_HasPick = m_NetworkView->pick(pos, &nid);
		if (m_HasPick)
		{
			m_SelectedNodes.push_back(nid);
		}
		/*
		else
		{
			#ifdef RD_OCULUS_RIFT
				m_CameraController.select(CameraController::OCULUS_RIFT);
			#else 
				m_CameraController.select(CameraController::FIRST_PERSON);
			#endif
		}
		*/
	}

	void onMouseDoubleClick(const glm::vec2& viewport_pos) override
	{
		auto pos = convertToWindowCoords(viewport_pos); // TODO: Hack! Remove when possible.
		
		/*
		if (m_HasPick)
		{
			m_CameraController.zoom(m_SelectNodes[0])

			#ifdef RD_OCULUS_RIFT
				m_CameraController.select(CameraController::OCULUS_RIFT);
			#else 
				m_CameraController.select(CameraController::SPHERICAL);
			#endif
		}
		*/

		m_CameraController.onMouseDoubleClick(pos);
	}

	void onMouseTripleClick(const glm::vec2& viewport_pos) override
	{
		auto pos = convertToWindowCoords(viewport_pos); // TODO: Hack! Remove when possible.
		
		m_CameraController.onMouseTripleClick(pos);
	}

	void onMouseMove(const glm::vec2& viewport_pos, const glm::vec2& viewport_dpos) override
	{
		auto pos = convertToWindowCoords(viewport_pos); // TODO: Hack! Remove when possible.
		auto dpos = convertToWindowCoords(viewport_dpos); // TODO: Hack! Remove when possible.
		
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
	Viewport m_Viewport;

	CameraController m_CameraController;

	rd::Font* m_Font;

	bool m_HasPick;
	std::vector<GPUGraph::NodeInstance::ID> m_SelectedNodes;
};

