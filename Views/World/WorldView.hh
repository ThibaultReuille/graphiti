#pragma once

#include <raindance/Core/Headers.hh>
#include <raindance/Core/Clock.hh>
#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/Bezier.hh>
#include <raindance/Core/PointCloud.hh>
#include <raindance/Core/Scene.hh>
#include <raindance/Core/Primitives/Line.hh>
#include <raindance/Core/Primitives/Cube.hh>
#include <raindance/Core/Material.hh>
#include <raindance/Core/Light.hh>
#include <raindance/Core/Resources/Texture.hh>

#include "Entities/MVC.hh"

#include "Views/World/WorldResources.hh"

typedef TranslationMap<SpaceNode::ID, Node::ID> NodeTranslationMap;
typedef TranslationMap<SpaceEdge::ID, Link::ID> LinkTranslationMap;

class WorldMapGeoPoint : public Scene::IDrawable
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
		g_WorldResources->WorldMapNodeIcon->draw(context, projection * view * glm::scale(model, glm::vec3(size, size, 1.0)), m_Color, 0);
	}

    inline void setColor(const glm::vec4& color) { m_Color = color; }

    inline void setSize(float size) { m_Size = size; }

private:
	glm::vec4 m_Color;
	float m_Size;
};

class EarthGeoPoint : public Scene::IDrawable
{
public:
    typedef unsigned long ID;

	EarthGeoPoint(ID id, const char* label)
		: m_ID(id), m_Color(glm::vec4(1.0, 1.0, 1.0, 0.5)), m_Size(1.0)
	{
	    (void) label;
	}

	~EarthGeoPoint()
	{
	}

	void draw(Context* context, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model)
	{
		g_WorldResources->EarthGeoPointMaterial.setDiffuse(m_Color);
		g_WorldResources->EarthGeoPointMaterial.setShininess(100.0f);

		Transformation transformation;
		transformation.set(model);
		transformation.scale(glm::vec3(g_WorldResources->EarthNodeSize, m_Size, g_WorldResources->EarthNodeSize));

		g_WorldResources->EarthGeoPointShader->use();
		g_WorldResources->EarthGeoPointShader->uniform("u_ModelMatrix").set(transformation.state());
		g_WorldResources->EarthGeoPointShader->uniform("u_ViewMatrix").set(view);
		g_WorldResources->EarthGeoPointShader->uniform("u_ProjectionMatrix").set(projection);
		// NOTE : Normal Matrix is the transpose of the inverse of the model view matrix
		// Using the Model View Matrix doesn't work if using non-homogeneous scales
		g_WorldResources->EarthGeoPointShader->uniform("u_NormalMatrix").set(glm::transpose(glm::inverse(glm::mat3(view * transformation.state()))));

		g_WorldResources->EarthGeoPointShader->uniform("u_Light.Type").set(g_WorldResources->Sun.getType());
		g_WorldResources->EarthGeoPointShader->uniform("u_Light.Position").set(g_WorldResources->Sun.getPosition());
		g_WorldResources->EarthGeoPointShader->uniform("u_Light.Direction").set(g_WorldResources->Sun.getDirection());
		g_WorldResources->EarthGeoPointShader->uniform("u_Light.Color").set(g_WorldResources->Sun.getColor());

		g_WorldResources->EarthGeoPointShader->uniform("u_Material.Ambient").set(g_WorldResources->EarthGeoPointMaterial.getAmbient());
		g_WorldResources->EarthGeoPointShader->uniform("u_Material.Diffuse").set(g_WorldResources->EarthGeoPointMaterial.getDiffuse());
		g_WorldResources->EarthGeoPointShader->uniform("u_Material.Specular").set(g_WorldResources->EarthGeoPointMaterial.getSpecular());
		g_WorldResources->EarthGeoPointShader->uniform("u_Material.Shininess").set(g_WorldResources->EarthGeoPointMaterial.getShininess());

		context->geometry().bind(g_WorldResources->EarthGeoCube->getVertexBuffer(), *g_WorldResources->EarthGeoPointShader);
		context->geometry().drawElements(GL_TRIANGLES, g_WorldResources->EarthGeoCube->getTriangleBuffer().size() / sizeof(short int), GL_UNSIGNED_SHORT, g_WorldResources->EarthGeoCube->getTriangleBuffer().ptr());
		context->geometry().unbind(g_WorldResources->EarthGeoCube->getVertexBuffer());
	}

	inline ID id() { return m_ID; }

	inline void setColor(const glm::vec4& color) { m_Color = color; }
	inline void setSize(float size) { m_Size = size; }

private:
    ID m_ID;
    glm::vec4 m_Color;
    float m_Size;
};

class EarthGeoLink : public Scene::IDrawable
{
public:
    typedef unsigned long ID;

    EarthGeoLink(Scene::Node* node1, Scene::Node* node2)
    : m_Node1(node1), m_Node2(node2)
    {
        m_Activity = 0.0f;
        update();
    }

    ~EarthGeoLink()
    {
    }

    void update()
    {
        glm::vec3 start = m_Node1->getPosition();
        glm::vec3 end = m_Node2->getPosition();

        glm::vec3 origin = glm::vec3(0.0, 0.0, 0.0); // NOTE : Earth center is assumed in (0, 0, 0)

        glm::vec3 center = (start + end) / 2.0f;

        glm::vec3 div1 = start;
        glm::vec3 div2 = end;

        glm::vec3 dirc = glm::normalize(center - origin);
        glm::vec3 dir1 = glm::normalize(div1 - origin);
        glm::vec3 dir2 = glm::normalize(div2 - origin);

        glm::vec3 p1 = 1.5f * g_WorldResources->EarthRadius * dir1;
        glm::vec3 p2 = 1.5f * g_WorldResources->EarthRadius * dir2;

        glm::vec3 d = p2 - p1;
        glm::vec3 pc = (glm::length(d) / 10.0f) * 1.5f * g_WorldResources->EarthRadius * dirc;

        m_Curve.clearControlPoints();
        m_Curve.addControlPoint(BezierCurve::ControlPoint(start, glm::vec4(1.0, 1.0, 1.0, 0.5)));
        m_Curve.addControlPoint(BezierCurve::ControlPoint(p1,    glm::vec4(1.0, 1.0, 1.0, 0.5)));
        m_Curve.addControlPoint(BezierCurve::ControlPoint(pc,    glm::vec4(1.0, 1.0, 1.0, 0.5)));
        m_Curve.addControlPoint(BezierCurve::ControlPoint(p2,    glm::vec4(1.0, 1.0, 1.0, 0.5)));
        m_Curve.addControlPoint(BezierCurve::ControlPoint(end,   glm::vec4(1.0, 1.0, 1.0, 0.5)));
        m_Curve.setDivisions(50);
        m_Curve.update();
    }

    inline SpaceNode::ID getNode1() { return static_cast<SpaceNode*>(m_Node1->getDrawable())->getID(); }
    inline SpaceNode::ID getNode2() { return static_cast<SpaceNode*>(m_Node2->getDrawable())->getID(); }

    void draw(Context* context, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model)
    {
        g_WorldResources->EarthGeoLinkShader->use();
        g_WorldResources->EarthGeoLinkShader->uniform("u_ModelViewProjection").set(projection * view * model);
        g_WorldResources->EarthGeoLinkShader->uniform("u_Tint").set(glm::vec4(1.0, 1.0, 1.0, 1.0));

        Buffer& vertexBuffer = m_Curve.getVertexBuffer();
        context->geometry().bind(vertexBuffer, *g_WorldResources->EarthGeoLinkShader);
        context->geometry().drawArrays(GL_LINE_STRIP, 0, vertexBuffer.size() / sizeof(BezierCurve::Vertex));
        context->geometry().unbind(vertexBuffer);
    }

    inline void setCurve(const BezierCurve& curve) { m_Curve = curve; }

    inline void setColor(const glm::vec4& color)
    {
        glm::vec4 middleColor = glm::vec4(WHITE, color.a);

        for (unsigned long i = 0; i < m_Curve.size(); i++)
        {
            float t = (float) i / (m_Curve.size() - 1);
            t = 1 - 2 * fabs(t - 0.5);

            BezierCurve::ControlPoint cp = m_Curve.getControlPoint(i);

            cp.Color = BezierCurve::linear(color, middleColor, t);

            m_Curve.setControlPoint(i, cp);
        }
        m_Curve.update();
    }

private:

    Scene::Node* m_Node1;
    Scene::Node* m_Node2;

    BezierCurve m_Curve;
    float m_Activity;
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
		float radLatitude = latitude * M_PI / 180.0;
		float radLongitude = longitude * M_PI / 180.0;

		glm::vec2 pos = glm::vec2
		(
			radLongitude,
			(5.0f / 4.0f) * log(tan(M_PI / 4.0f + 2.0f * radLatitude / 5.0f))
		);

		float range = (5.0f / 4.0f) * log(tan(9.0f * M_PI / 20.0f));
		// NOTE : Scale to screen
		return glm::vec2
		(
			(float) m_Width / 2 + m_Width * pos.x / (2 * M_PI),
			(float) m_Height / 2 + m_Height * pos.y / (2 * range)
		);
	}

	glm::vec2 customProjection(float latitude, float longitude)
	{
		float radLatitude = latitude * M_PI / 180.0;
		float radLongitude = longitude * M_PI / 180.0;

		glm::vec2 pos = glm::vec2
		(
			radLongitude,
			(5.0f / 4.0f) * log(tan(M_PI / 4.0f + 2.0f * radLatitude / 5.0f))
		);

		float xfactor = (675.0f / 702.0f);
		float yfactor = (150.0f / 180.0f);

		pos.x = xfactor * pos.x;

		float range = (5.0f / 4.0f) * log(tan(9.0f * M_PI / 20.0f)) * yfactor;
		// NOTE : Scale to screen
		return glm::vec2
		(
			(float) m_Width / 2 - 10 + m_Width * pos.x / (2 * M_PI),
			(float) m_Height / 2 - 73 + m_Height * pos.y / (2 * range)
		);
	}

	inline unsigned int width() { return m_Width; }
	inline unsigned int height() { return m_Height; }
	inline Texture& layer(unsigned int n) { return *m_Layers[n]; }

	void onAddNode(Node::ID uid, const char* label)
    {
        (void) label;
        unsigned long vid = m_GeoNodes.add(new Scene::Node(new WorldMapGeoPoint(), true));
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

    void onSetNodeWeight(Node::ID uid, float weight)
    {
        (void) uid;
        (void) weight;

        // NOTE : Ignoring node size for clarity
        // unsigned int vid = m_NodeMap.getLocalID(uid);
        // m_GeoNodes[vid]->setScale(glm::vec3(weight, weight, weight));
    }

    void onSetNodeAttribute(Node::ID uid, const std::string& name, VariableType type, const std::string& value)
    {
        unsigned long vid = m_NodeMap.getLocalID(uid);

        Vec2Variable vvec2;
        Vec3Variable vvec3;
        Vec4Variable vvec4;

        if (name == "world:geolocation" && type == VEC2)
        {
           vvec2.set(value);
           // TODO : Update links when position changes
           m_GeoNodes[vid]->setPosition(glm::vec3(miller(vvec2.value()[0], vvec2.value()[1]), 0));
        }
        else if (name == "world:color" && type == VEC3)
        {
            vvec3.set(value);
            static_cast<WorldMapGeoPoint*>(m_GeoNodes[vid]->getDrawable())->setColor(glm::vec4(vvec3.value(), 1.0));
        }
        else if (name == "world:color" && type == VEC4)
        {
            vvec4.set(value);
            static_cast<WorldMapGeoPoint*>(m_GeoNodes[vid]->getDrawable())->setColor(vvec4.value());
        }
        else
        {
            LOG("[WORLD] Attribute '%s' ignored.\n", name.c_str());
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

class Earth
{
public:

	Earth()
	{
		m_SphereTexture = ResourceManager::getInstance().loadTexture("earth", Resources_Earth_world_2048x1024_jpg, sizeof(Resources_Earth_world_2048x1024_jpg));
		m_SphereMesh = new SphereMesh(g_WorldResources->EarthRadius, 100, 100);
		m_SphereShader = ResourceManager::getInstance().loadShader("globe", Resources_globe_vert, sizeof(Resources_globe_vert), Resources_globe_frag, sizeof(Resources_globe_frag));
		m_SphereMaterial.setDiffuse(glm::vec4(0.1, 0.1, 0.1, 1.0));
		m_SphereMaterial.setShininess(45.0f);

		g_WorldResources->Sun.setPosition(glm::vec3(100.0, 100.0, 100.0));

		/*
		glm::vec3 geo_Paris =        spherical( 48.8742,    2.3470, 0.0);
		glm::vec3 geo_SanFrancisco = spherical( 37.7750, -122.4183, 0.0);
		glm::vec3 geo_NewYorkCity =  spherical( 40.7142,  -74.0064, 0.0);
		glm::vec3 geo_Tokyo =        spherical( 35.6833,  139.7667, 0.0);
		glm::vec3 geo_RioDeJaneiro = spherical(-22.9083,  -43.2436, 0.0);
		glm::vec3 geo_Sidney =       spherical(-33.8683,  151.2086, 0.0);
		glm::vec3 geo_Vancouver =    spherical(49.2505,  -123.1119, 0.0);
		glm::vec3 geo_Johannesburg = spherical(-26.2000,   28.0667, 0.0);
		glm::vec3 geo_Beijing =      spherical(39.9100,   116.4000, 0.0);
	    */
	}

	~Earth()
	{
		ResourceManager::getInstance().unload(m_SphereTexture);
		ResourceManager::getInstance().unload(m_SphereShader);
		delete m_SphereMesh;
	}

	void draw(Context* context, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model)
	{
		glClear(GL_DEPTH_BUFFER_BIT);

#ifndef EMSCRIPTEN
        // NOTE : Not supported by WebGL
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
#endif
        glLineWidth(1.5);

		// static unsigned int count = 0;
		// count++;
		// float t = (float)count / 1000.0f;
		// g_WorldResources->Sun.setPosition(glm::vec3(100.0 * cos(t), 0, 100.0 * sin(t)));

		Transformation transformation;

		transformation.set(model);

		transformation.push();
		{
			transformation.rotate(-180.0f, glm::vec3(0, 1, 0));
			transformation.rotate(180.0f, glm::vec3(1, 0, 0));

			Buffer& vertexBuffer = m_SphereMesh->getVertexBuffer();
			Buffer& indexBuffer = m_SphereMesh->getIndexBuffer();

			m_SphereShader->use();
			m_SphereShader->uniform("u_ModelMatrix").set(transformation.state());
			m_SphereShader->uniform("u_ViewMatrix").set(view);
			m_SphereShader->uniform("u_ProjectionMatrix").set(projection);
			// NOTE : Normal Matrix is the transpose of the inverse of the model view matrix
			// Using the Model View Matrix doesn't work if using non-homogeneous scales
			m_SphereShader->uniform("u_NormalMatrix").set(glm::transpose(glm::inverse(glm::mat3(view * transformation.state()))));

			m_SphereShader->uniform("u_Texture").set(*m_SphereTexture);

			m_SphereShader->uniform("u_Light.Type").set(g_WorldResources->Sun.getType());
			m_SphereShader->uniform("u_Light.Position").set(g_WorldResources->Sun.getPosition());
			m_SphereShader->uniform("u_Light.Direction").set(g_WorldResources->Sun.getDirection());
			m_SphereShader->uniform("u_Light.Color").set(g_WorldResources->Sun.getColor());

			m_SphereShader->uniform("u_Material.Ambient").set(m_SphereMaterial.getAmbient());
			m_SphereShader->uniform("u_Material.Diffuse").set(m_SphereMaterial.getDiffuse());
			m_SphereShader->uniform("u_Material.Specular").set(m_SphereMaterial.getSpecular());
			m_SphereShader->uniform("u_Material.Shininess").set(m_SphereMaterial.getShininess());

			context->geometry().bind(vertexBuffer, *m_SphereShader);
			context->geometry().drawElements(GL_TRIANGLES, indexBuffer.size() / sizeof(unsigned short int), GL_UNSIGNED_SHORT, indexBuffer.ptr());
			context->geometry().unbind(vertexBuffer);
		}
		transformation.pop();

        m_GeoNodes.draw(context, projection, view, transformation.state());

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
		m_GeoLinks.draw(context, projection, view, transformation.state());
		glDisable(GL_BLEND);
	}

    void onAddNode(Node::ID uid, const char* label)
    {
        Scene::Node* node = new Scene::Node(NULL, false);
        EarthGeoPoint::ID vid = m_GeoNodes.add(node);

        EarthGeoPoint* geopoint = new EarthGeoPoint(vid, label);
        node->setDrawable(geopoint, true);

        m_NodeMap.addRemoteID(uid, vid);
    }

    void onRemoveNode(Node::ID uid)
    {
        EarthGeoPoint::ID vid = m_NodeMap.getLocalID(uid);

        for (unsigned int i = 0; i < m_GeoLinks.size(); i++)
        {
            if (m_GeoLinks[i] != NULL)
            {
                EarthGeoLink* link = static_cast<EarthGeoLink*>(m_GeoLinks[i]->getDrawable());

                if (link->getNode1() == vid || link->getNode2() == vid)
                {
                    m_GeoLinks.remove(i);
                    m_LinkMap.removeLocalID(i);

                    LOG("Link %i removed with node\n", i);
                }
            }
        }

        // TODO : Remove node spheres

        m_GeoNodes.remove(vid);
        m_NodeMap.eraseRemoteID(uid, vid);
    }

    void onSetNodeWeight(Node::ID uid, float weight)
    {
        EarthGeoPoint::ID vid = m_NodeMap.getLocalID(uid);

        m_GeoNodes[vid]->setScale(glm::vec3(1.0, weight, 1.0));
    }

    void onSetNodeAttribute(Node::ID uid, const std::string& name, VariableType type, const std::string& value)
    {
        EarthGeoPoint::ID vid = m_NodeMap.getLocalID(uid);

        FloatVariable vfloat;
        Vec2Variable vvec2;
        Vec3Variable vvec3;
        Vec4Variable vvec4;

        if (name == "world:geolocation" && type == VEC2)
        {
           vvec2.set(value);
           // TODO : Update links when position changes
           m_GeoNodes[vid]->setPosition(spherical(vvec2.value()[0], vvec2.value()[1], 0.0f));
           glm::quat rotation;
           rotation = glm::angleAxis(vvec2.value()[0] - 90, glm::vec3(0, 0, 1)) * rotation;
           rotation = glm::angleAxis(vvec2.value()[1], glm::vec3(0, 1, 0)) * rotation;
           m_GeoNodes[vid]->setOrientation(rotation);
        }
        else if (name == "world:color" && type == VEC3)
        {
            vvec3.set(value);
            static_cast<EarthGeoPoint*>(m_GeoNodes[vid]->getDrawable())->setColor(glm::vec4(vvec3.value(), 1.0));
        }
        else if (name == "world:color" && type == VEC4)
        {
            vvec4.set(value);
            static_cast<EarthGeoPoint*>(m_GeoNodes[vid]->getDrawable())->setColor(vvec4.value());
        }
        else if (name == "world:size" && type == FLOAT)
        {
            vfloat.set(value);
            static_cast<EarthGeoPoint*>(m_GeoNodes[vid]->getDrawable())->setSize(vfloat.value());
        }
        else
        {
            LOG("[WORLD] Attribute '%s' ignored.\n", name.c_str());
        }
    }

    void onAddLink(Link::ID uid, Node::ID uid1, Node::ID uid2)
    {
        EarthGeoPoint::ID vid1 = m_NodeMap.getLocalID(uid1);
        EarthGeoPoint::ID vid2 = m_NodeMap.getLocalID(uid2);

        EarthGeoLink::ID lid = m_GeoLinks.add(new Scene::Node(new EarthGeoLink(m_GeoNodes[vid1], m_GeoNodes[vid2]), true));

        m_LinkMap.addRemoteID(uid, lid);
    }

    void onRemoveLink(Link::ID uid)
    {
        SpaceEdge::ID vid = m_LinkMap.getLocalID(uid);
        m_GeoLinks.remove(vid);
        m_LinkMap.eraseRemoteID(uid, vid);
    }

    void onSetLinkAttribute(Link::ID uid, const std::string& name, VariableType type, const std::string& value)
    {
        SpaceEdge::ID vid = m_LinkMap.getLocalID(uid);

        Vec2Variable vvec2;
        Vec3Variable vvec3;
        Vec4Variable vvec4;

        if (name == "world:color" && type == VEC3)
        {
            vvec3.set(value);
            static_cast<EarthGeoLink*>(m_GeoLinks[vid]->getDrawable())->setColor(glm::vec4(vvec3.value(), 1.0));
        }
        else if (name == "world:color" && type == VEC4)
        {
            vvec4.set(value);
            static_cast<EarthGeoLink*>(m_GeoLinks[vid]->getDrawable())->setColor(vvec4.value());
        }
        else
        {
            LOG("[WORLD] Attribute '%s' ignored.\n", name.c_str());
        }
    }

	glm::vec3 spherical(float latitude, float longitude, float delta)
	{
		float radLatitude = latitude * M_PI / 180.0;
		float radLongitude = longitude * M_PI / 180.0;

		return glm::vec3
		(
			(g_WorldResources->EarthRadius + delta) * cos(radLatitude) * cos(radLongitude),
			(g_WorldResources->EarthRadius + delta) * sin(radLatitude),
			- (g_WorldResources->EarthRadius + delta) * cos(radLatitude) * sin(radLongitude)
		);
	}

private:
	SphereMesh* m_SphereMesh;
	Texture* m_SphereTexture;
	Shader::Program* m_SphereShader;
	Material m_SphereMaterial;

	Scene::NodeVector m_GeoNodes;
	Scene::NodeVector m_GeoLinks;

    NodeTranslationMap m_NodeMap;
    LinkTranslationMap m_LinkMap;
};

class WorldView : public GraphView
{
public:

	WorldView()
	{
		std::cout << "Creating world view ..." << std::endl;

		m_WindowWidth = glutGet(GLUT_WINDOW_WIDTH);
		m_WindowHeight = glutGet(GLUT_WINDOW_HEIGHT);

		m_Camera2D.setOrthographicProjection(0.0f, (float)m_WindowWidth, 0.0f, (float)m_WindowHeight, 0.001f, 100.f);
		m_Camera2D.lookAt(glm::vec3(0, 0, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_Camera3D.setPerspectiveProjection(60.0f, m_WindowWidth / (float)m_WindowHeight, 0.1f, 1024.0f);
		m_Camera3D.lookAt(glm::vec3(30, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_GraphEntity = NULL;

		g_WorldResources = new WorldResources();

		float texture_ratio = 2048.0f / 1502.0f;
		m_WorldMap = new WorldMap(m_WindowHeight * texture_ratio, m_WindowHeight);

		m_Earth = new Earth();
	}

	virtual ~WorldView()
	{
		delete g_WorldResources;
		delete m_WorldMap;
		delete m_Earth;
	}

	virtual const char* name() const { return "world"; }

	virtual bool bind(Entity* entity)
	{
        if (entity->type() != Entity::GRAPH)
        {
            LOG("[WORLD] Couldn't bind entity to view : Wrong entity type!\n");
            return false;
        }

        m_GraphEntity = static_cast<GraphEntity*>(entity);
        m_SphericalCameraController.bind(m_GraphEntity->context(), &m_Camera3D);

        m_GraphEntity->views().push_back(this);
        m_GraphEntity->listeners().push_back(this);

        return true;
	}

	inline Camera* getCamera3D()
	{
		return &m_Camera3D;
	}

	virtual void draw()
	{
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		Transformation transformation;

		g_WorldResources->Starfield->draw(context(), m_Camera3D);

		m_Earth->draw(context(), m_Camera3D.getProjectionMatrix(), m_Camera3D.getViewMatrix(), transformation.state());

		transformation.push();
		{
			transformation.translate(glm::vec3(10, 10, 0));
			transformation.scale(glm::vec3(0.25, 0.25, 0.25));
			m_WorldMap->draw(context(), m_Camera2D.getProjectionMatrix(), m_Camera2D.getViewMatrix(), transformation.state());
		}
		transformation.pop();
	}

	virtual void idle()
	{
	}

	virtual void onSetAttribute(const std::string& name, VariableType type, const std::string& value)
	{
		(void) type;
		LOG("[WORLDVIEW] setAttribute(%s, %s)\n", name.c_str(), value.c_str());
	}

	virtual IVariable* getNodeAttribute(Node::ID id, std::string& name)
	{
		LOG("[WORLDVIEW] getNodeAttribute(%lu, %s)\n", id, name.c_str());
		return NULL;
	}

	virtual IVariable* getLinkAttribute(Link::ID id, std::string& name)
	{
		LOG("[WORLDVIEW] getLinkAttribute(%lu, %s)\n", id, name.c_str());
		return NULL;
	}

	void notify(IMessage* message)
	{
		IMessage::Type type = message->type();
		if (type == IMessage::WIDGET)
		{
			WidgetMessage* msg = static_cast<WidgetMessage*>(message);

			(void) msg;
		}
	}

	// ----- Graph Events -----

	void onAddNode(Node::ID uid, const char* label)
	{
        m_Earth->onAddNode(uid, label);
        m_WorldMap->onAddNode(uid, label);
	}

	void onRemoveNode(Node::ID uid)
	{
	    m_Earth->onRemoveNode(uid);
	    m_WorldMap->onRemoveNode(uid);
	}

	void onSetNodeAttribute(Node::ID uid, const std::string& name, VariableType type, const std::string& value)
	{
        m_Earth->onSetNodeAttribute(uid, name, type, value);
        m_WorldMap->onSetNodeAttribute(uid, name, type, value);
	}

	void onSetNodeLabel(Node::ID uid, const char* label)
	{
		(void) uid;
		(void) label;
	}

	void onAddLink(Link::ID uid, Node::ID uid1, Node::ID uid2)
	{
	    m_Earth->onAddLink(uid, uid1, uid2);
	    // m_WorldMap->onAddLink(uid, uid1, uid2);
	}

	void onRemoveLink(Link::ID uid)
	{
	    m_Earth->onRemoveLink(uid);
	    // m_WorldMap->onAddLink(uid);
	}

	void onSetLinkAttribute(Link::ID uid, const std::string& name, VariableType type, const std::string& value)
	{
        m_Earth->onSetLinkAttribute(uid, name, type, value);
        // m_WorldMap->onSetLinkAttribute(uid, name, type, value);
	}

	void onAddSphere(Sphere::ID id, const char* label)
	{
		(void) id;
		(void) label;
	}

    void onTagNode(Node::ID node, Sphere::ID sphere)
    {
        (void) node;
        (void) sphere;
    }

	void onAddNeighbor(const std::pair<Node::ID, Link::ID>& element, const char* label, Node::ID neighbor)
	{
		(void) element;
		(void) label;
		(void) neighbor;
	}

	inline GraphContext* context() { return static_cast<GraphContext*>(m_GraphEntity->context()); }
	inline GraphModel* model() { return m_GraphEntity->model(); }

private:
	unsigned int m_WindowWidth;
	unsigned int m_WindowHeight;

	GraphEntity* m_GraphEntity;

	Clock m_Clock;
	Camera m_Camera2D;
	Camera m_Camera3D;
	SphericalCameraController m_SphericalCameraController;

	WorldMap* m_WorldMap;
	Earth* m_Earth;
};

