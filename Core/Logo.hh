#pragma once

#include <raindance/Core/Headers.hh>
#include <raindance/Core/Transformation.hh>
#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/Icon.hh>

class Logo : public Document::Node
{
public:
	Logo(Document::Node* parent = NULL)
	: Document::Node(parent)
	{       
		m_Logo = new Icon(glm::vec2(1.0, 1.0), glm::vec2(0.5, 0.5));
		m_Logo->load("logo", Assets_Textures_umbrella_logo_png, sizeof(Assets_Textures_umbrella_logo_png)); 
	}

	virtual ~Logo()
	{
		SAFE_DELETE(m_Logo);
	}

	void draw(Context* context) override
	{
		m_Camera.setOrthographicProjection(0, 1, 0, 1, 0, 1);
		m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		
		m_Logo->draw(context, m_Camera.getViewProjectionMatrix() , glm::vec4(1.0, 1.0, 1.0, 0.75), 0);

		glEnable(GL_DEPTH_TEST);
	}

	void idle(Context* context) override
	{
		(void) context;
	}

private:
	Camera m_Camera;
	Icon* m_Logo;
};