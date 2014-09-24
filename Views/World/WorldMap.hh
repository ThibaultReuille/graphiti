#pragma once

#include <raindance/Core/Scene/Node.hh>

class WorldMapGeoPoint : public Scene::Node
{
public:
	WorldMapGeoPoint()
	{
		m_Color = glm::vec4(1.0, 1.0, 1.0, 1.0);
		m_Size = 2.0;
	}

	~WorldMapGeoPoint()
	{
	}

	void draw(Context* context, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model)
	{
	    float size = m_Size * g_WorldResources->WorldMapNodeIconSize;
		g_WorldResources->WorldMapNodeIcon->draw(context, projection * view * glm::scale(model * getModelMatrix(), glm::vec3(size, size, 1.0)), m_Color, 0);
	}

    bool isOverlap (const glm::vec3& min, const glm::vec3& max) const
    {
        (void) min;
        (void) max;

        // TODO : WorldMapGeoPoint::isOverlap
        return false;
    }

    inline void setColor(const glm::vec4& color) { m_Color = color; }

    inline void setSize(float size) { m_Size = size; }

private:
	glm::vec4 m_Color;
	float m_Size;
};

class WorldMap
{
public:

	WorldMap(unsigned int width, unsigned int height)
	{
		std::cout << "Loading world map geometry ..." << std::endl;

		m_Width = width;
		m_Height = height;

		float w = (float) width;
		float h = (float) height;
		//                Position                Texcoord
		m_VertexBuffer << glm::vec3(0, 0, 0) << glm::vec2(0, 1);
		m_VertexBuffer << glm::vec3(w, 0, 0) << glm::vec2(1, 1);
		m_VertexBuffer << glm::vec3(w, h, 0) << glm::vec2(1, 0);
		m_VertexBuffer << glm::vec3(0, h, 0) << glm::vec2(0, 0);
		m_VertexBuffer.describe("a_Position", 3, GL_FLOAT, 5 * sizeof(GLfloat), 0);
		m_VertexBuffer.describe("a_Texcoord", 2, GL_FLOAT, 5 * sizeof(GLfloat), 3 * sizeof(GLfloat));
		m_VertexBuffer.generate(Buffer::DYNAMIC);

		std::cout << "Loading world map textures ..." << std::endl;
		// Texture* texture = new Texture("world", Resources_Earth_black_and_blue_jpg, sizeof(Resources_Earth_black_and_blue_jpg));
		Texture* texture = ResourceManager::getInstance().loadTexture("miller", Resources_Earth_miller_2048x1502_jpg, sizeof(Resources_Earth_miller_2048x1502_jpg));
		m_Layers.push_back(texture);
		m_Layers.push_back(ResourceManager::getInstance().loadTexture("layer1", texture->width(), texture->height(), texture->components()));

		std::cout << "Loading world map shader ..." << std::endl;
		m_Shader = ResourceManager::getInstance().loadShader("world", Resources_world_vert, sizeof(Resources_world_vert), Resources_world_frag, sizeof(Resources_world_frag));
		// m_Shader->dump();
	}

	~WorldMap()
	{
		ResourceManager::getInstance().unload(m_Shader);

		std::vector<Texture*>::iterator it;
		for (it = m_Layers.begin(); it != m_Layers.end(); ++it)
		{
			ResourceManager::getInstance().unload(*it);
		}
	}

	// NOTE : http://en.wikipedia.org/wiki/Miller_projection
	glm::vec2 miller(float latitude, float longitude)
	{
		float pi = static_cast<float>(M_PI);

		float radLatitude = latitude * pi / 180.0;
		float radLongitude = longitude * pi / 180.0;

		glm::vec2 pos = glm::vec2
		(
			radLongitude,
			(5.0f / 4.0f) * log(tan(pi / 4.0f + 2.0f * radLatitude / 5.0f))
		);

		float range = (5.0f / 4.0f) * log(tan(9.0f * pi / 20.0f));
		// NOTE : Scale to screen
		return glm::vec2
		(
			(float) m_Width / 2 + m_Width * pos.x / (2 * pi),
			(float) m_Height / 2 + m_Height * pos.y / (2 * range)
		);
	}

	glm::vec2 customProjection(float latitude, float longitude)
	{
		float pi = static_cast<float>(M_PI);

		float radLatitude = latitude * pi / 180.0;
		float radLongitude = longitude * pi / 180.0;

		glm::vec2 pos = glm::vec2
		(
			radLongitude,
			(5.0f / 4.0f) * log(tan(pi / 4.0f + 2.0f * radLatitude / 5.0f))
		);

		float xfactor = (675.0f / 702.0f);
		float yfactor = (150.0f / 180.0f);

		pos.x = xfactor * pos.x;

		float range = (5.0f / 4.0f) * log(tan(9.0f * pi / 20.0f)) * yfactor;
		// NOTE : Scale to screen
		return glm::vec2
		(
			(float) m_Width / 2 - 10 + m_Width * pos.x / (2 * pi),
			(float) m_Height / 2 - 73 + m_Height * pos.y / (2 * range)
		);
	}

	inline unsigned int width() { return m_Width; }
	inline unsigned int height() { return m_Height; }
	inline Texture& layer(unsigned int n) { return *m_Layers[n]; }

	void onAddNode(Node::ID uid, const char* label)
    {
        (void) label;
        unsigned long vid = m_GeoNodes.add(new WorldMapGeoPoint());
        m_NodeMap.addRemoteID(uid, vid);
    }

    void onRemoveNode(Node::ID uid)
    {
        unsigned long vid = m_NodeMap.getLocalID(uid);

        // TODO : Remove node links
        // TODO : Remove node spheres

        m_GeoNodes.remove(vid);
        m_NodeMap.eraseRemoteID(uid, vid);
    }

    void onSetNodeAttribute(Node::ID uid, const std::string& name, VariableType type, const std::string& value)
    {
        unsigned long vid = m_NodeMap.getLocalID(uid);

        Vec2Variable vvec2;
        Vec3Variable vvec3;
        Vec4Variable vvec4;

        if (name == "world:geolocation" && type == RD_VEC2)
        {
           vvec2.set(value);
           // TODO : Update links when position changes
           m_GeoNodes[vid]->setPosition(glm::vec3(miller(vvec2.value()[0], vvec2.value()[1]), 0));
        }
        else if (name == "world:color" && type == RD_VEC3)
        {
            vvec3.set(value);
            static_cast<WorldMapGeoPoint*>(m_GeoNodes[vid])->setColor(glm::vec4(vvec3.value(), 1.0));
        }
        else if (name == "world:color" && type == RD_VEC4)
        {
            vvec4.set(value);
            static_cast<WorldMapGeoPoint*>(m_GeoNodes[vid])->setColor(vvec4.value());
        }
        else
        {
            LOG("[WORLDMAP] Attribute '%s' ignored.\n", name.c_str());
        }
    }

	void draw(GraphContext* context, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model)
	{
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);

		Transformation transformation;

		transformation.set(model);

		{
			static unsigned short int indices[] = { 0, 1, 2, 0, 2, 3 };

			m_Shader->use();
			m_Shader->uniform("u_ModelViewProjection").set(projection * view * transformation.state());

			std::vector<Texture*>::iterator it;
			for (it = m_Layers.begin(); it != m_Layers.end(); ++it)
			{
				m_Shader->uniform("u_Texture").set(**it);
				context->geometry().bind(m_VertexBuffer, *m_Shader);
				context->geometry().drawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned short int), GL_UNSIGNED_SHORT, indices);
				context->geometry().unbind(m_VertexBuffer);
			}
		}

		transformation.translate(glm::vec3(0.0, 0.0, -1.0));
		m_GeoNodes.draw(context, projection, view, transformation.state());

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}

private:

	unsigned int m_Width;
	unsigned int m_Height;
	std::vector<Texture*> m_Layers;

	Buffer m_VertexBuffer;
	Shader::Program* m_Shader;

	Scene::NodeVector m_GeoNodes;
    NodeTranslationMap m_NodeMap;
};