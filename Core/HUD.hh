#pragma once

#include <raindance/Core/Headers.hh>
#include <raindance/Core/Transformation.hh>
#include <raindance/Core/Icon.hh>

#include <raindance/Core/GUI/Widgets/Widget.hh>
#include <raindance/Core/GUI/Widgets/TextWidget.hh>

#include <graphiti/Core/Shell.hh>

class GraphitiHUD : public Controller
{
public:
	GraphitiHUD(const Viewport& viewport)
	{
		onResize(viewport);

		m_Camera.setOrthographicProjection(0.0f, (float)m_WindowWidth, 0.0f, (float)m_WindowHeight, 0.001f, 100.f);
		m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_Logo = new Icon();
		m_Logo->load("logo", Assets_Textures_umbrella_logo_png, sizeof(Assets_Textures_umbrella_logo_png));

		m_Shell = new GraphitiShell();
		m_ShowShell = true;
		
		m_WidgetPick = NULL;
		m_Context = NULL;
	}

	virtual ~GraphitiHUD()
	{
		SAFE_DELETE(m_Shell);
		SAFE_DELETE(m_Logo);
	}

	void bind(Context* context)
	{
		m_Context = context;
	}

	void draw(Context* context)
	{
		(void) context;
		
		Transformation transformation;

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		
		// Draw Logo
		transformation.push();
			transformation.translate(glm::vec3(m_WindowWidth - 64 / 2 - 10, 10 + 64 / 2, 0));
			transformation.scale(glm::vec3(64, 64, 0));
			m_Logo->draw(m_Context, m_Camera.getViewProjectionMatrix() * transformation.state(), glm::vec4(1.0, 1.0, 1.0, 1.0), 0);
		transformation.pop();

		// Draw Shell
		if (m_ShowShell)
		{
			transformation.push();
				transformation.translate(glm::vec3(10, 10, 0));
				m_Shell->draw(m_Context, m_Camera, transformation);
			transformation.pop();
		}

		glEnable(GL_DEPTH_TEST);
	}

	void onResize(const Viewport& viewport)
	{
		auto dimension = viewport.getDimension();

		m_WindowWidth = dimension.x;
		m_WindowHeight = dimension.y;

		m_Camera.resize(dimension.x,  dimension.y);
		m_Camera.setOrthographicProjection(0.0f, dimension.x, 0.0f, dimension.y, 0.001f, 100.f);
        m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	}

	IWidget* getWidgetPick() { return NULL; }

	void onKey(int key, int scancode, int action, int mods) override
	{
		(void) scancode;
		(void) mods;
		
		if (action == GLFW_PRESS && key == GLFW_KEY_TAB)
			m_ShowShell = !m_ShowShell;

		if (m_ShowShell)
			m_Shell->onKey(key, scancode, action, mods);
	}
    
    void onChar(unsigned codepoint) override
    {
    	if (m_ShowShell)
        	m_Shell->onChar(codepoint);
    }

	inline GraphitiShell* getShell() { return m_Shell; }

private:
	Context* m_Context;

	GraphitiShell* m_Shell;
	bool m_ShowShell;

	int m_WindowWidth;
	int m_WindowHeight;

	Camera m_Camera;
	Icon* m_Logo;

	IWidget* m_WidgetPick;
};
