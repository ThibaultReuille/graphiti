#pragma once

#include <raindance/Core/Headers.hh>
#include <raindance/Core/Transformation.hh>
#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/Primitives/Rectangle.hh>

class WorldMap : public Document::Node
{
public:
	WorldMap(Document::Node* parent = NULL)
	: Document::Node(parent)
	{       
		auto image = FS::BinaryFile("Assets/Earth/miller-bw-cropped.png");
		m_Texture = ResourceManager::getInstance().loadTexture("WorldMap/miller", (unsigned char*)image.content().data(), image.content().size());

		m_Rectangle.setPosition(glm::vec3(0.0, 0.0, 0.0));
		m_Rectangle.setDimension(glm::vec2(1.0, 1.0));
		m_Rectangle.setColor(glm::vec4(1.0, 1.0, 1.0, 0.75));
		m_Rectangle.setTexture(m_Texture);
	}

	virtual ~WorldMap()
	{
		ResourceManager::getInstance().unload(m_Texture);
	}

	void draw(Context* context) override
	{
		m_Camera.setOrthographicProjection(0, 1, 0, 1, 0, 1);
		m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		glDisable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		
		Transformation transformation;

		m_Rectangle.draw(context, m_Camera, transformation);
	}

	void idle(Context* context) override
	{
		(void) context;
	}

private:
	Camera m_Camera;
	Rectangle m_Rectangle;
	Texture* m_Texture;
};