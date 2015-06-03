#pragma once

#include <raindance/Core/Scene/Node.hh>

class EarthGeoPoint : public Scene::Node
{
public:
    typedef unsigned long ID;

	EarthGeoPoint(const char* label)
		: m_ID(0), m_Color(glm::vec4(1.0, 1.0, 1.0, 0.5)), m_Size(1.0)
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
		transformation.set(model * getModelMatrix());
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

        context->geometry().bind(g_WorldResources->EarthGeoCube->getTriangleVertexBuffer(), *g_WorldResources->EarthGeoPointShader);        
        context->geometry().drawArrays(GL_TRIANGLES, 0, g_WorldResources->EarthGeoCube->getTriangleVertexBuffer().size() / sizeof(Cube::Vertex));
        context->geometry().unbind(g_WorldResources->EarthGeoCube->getTriangleVertexBuffer());
	}

	bool isOverlap (const glm::vec3& min, const glm::vec3& max) const
    {
        (void) min;
        (void) max;

        // TODO : EarthGeoPoint::isOverlap
        return false;
    }

	inline ID getID() { return m_ID; }
	inline void setID(ID id) { m_ID = id; }

	inline void setColor(const glm::vec4& color) { m_Color = color; }
	inline void setSize(float size) { m_Size = size; }

private:
    ID m_ID;
    glm::vec4 m_Color;
    float m_Size;
};

class EarthGeoEdge : public Scene::Node
{
public:
    typedef unsigned long ID;

    EarthGeoEdge(Scene::Node* node1, Scene::Node* node2)
    : m_Node1(node1), m_Node2(node2)
    {
        m_Activity = 0.0f;
        update();
    }

    ~EarthGeoEdge()
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

    inline EarthGeoPoint::ID getNode1() { return static_cast<EarthGeoPoint*>(m_Node1)->getID(); }
    inline EarthGeoPoint::ID getNode2() { return static_cast<EarthGeoPoint*>(m_Node2)->getID(); }

    void draw(Context* context, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model)
    {
        g_WorldResources->EarthGeoEdgeShader->use();
        g_WorldResources->EarthGeoEdgeShader->uniform("u_ModelViewProjection").set(projection * view * model);
        g_WorldResources->EarthGeoEdgeShader->uniform("u_Tint").set(glm::vec4(1.0, 1.0, 1.0, 1.0));

        Buffer& vertexBuffer = m_Curve.getVertexBuffer();
        context->geometry().bind(vertexBuffer, *g_WorldResources->EarthGeoEdgeShader);
        context->geometry().drawArrays(GL_LINE_STRIP, 0, vertexBuffer.size() / sizeof(BezierCurve::Vertex));
        context->geometry().unbind(vertexBuffer);
    }

    bool isOverlap (const glm::vec3& min, const glm::vec3& max) const
    {
        (void) min;
        (void) max;

        // TODO : EarthGeoEdge::isOverlap
        return false;
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

class Earth_Old
{
public:

	Earth_Old()
	{
		m_SphereTexture = ResourceManager::getInstance().loadTexture("earth", Assets_Earth_world_2048x1024_jpg, sizeof(Assets_Earth_world_2048x1024_jpg));
		m_SphereMesh = new SphereMesh(g_WorldResources->EarthRadius, 100, 100);

        FS::TextFile globe_vert("Assets/Earth/globe.vert");
        FS::TextFile globe_frag("Assets/Earth/globe.frag");
		m_SphereShader = ResourceManager::getInstance().loadShader("WorldVisualizer/globe", globe_vert.content(), globe_frag.content());

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

	virtual ~Earth_Old()
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
			//Buffer& indexBuffer = m_SphereMesh->getIndexBuffer();

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
            context->geometry().drawArrays(GL_TRIANGLES, 0, vertexBuffer.size() / sizeof(SphereMesh::Vertex));
			context->geometry().unbind(vertexBuffer);
		}
		transformation.pop();

        m_GeoNodes.draw(context, projection, view, transformation.state());

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
		m_GeoEdges.draw(context, projection, view, transformation.state());
		glDisable(GL_BLEND);
	}

    void onAddNode(Node::ID uid, const char* label)
    {
        EarthGeoPoint* geopoint = new EarthGeoPoint(label);
        EarthGeoPoint::ID vid = m_GeoNodes.add(geopoint);
        geopoint->setID(vid);

        m_NodeMap.addRemoteID(uid, vid);
    }

    void onRemoveNode(Node::ID uid)
    {
        EarthGeoPoint::ID vid = m_NodeMap.getLocalID(uid);

        for (unsigned int i = 0; i < m_GeoEdges.size(); i++)
        {
            if (m_GeoEdges[i] != NULL)
            {
                EarthGeoEdge* edge = static_cast<EarthGeoEdge*>(m_GeoEdges[i]);

                if (edge->getNode1() == vid || edge->getNode2() == vid)
                {
                    m_GeoEdges.remove(i);
                    m_EdgeMap.removeLocalID(i);

                    LOG("Edge %i removed with node\n", i);
                }
            }
        }

        // TODO : Remove node spheres

        m_GeoNodes.remove(vid);
        m_NodeMap.eraseRemoteID(uid, vid);
    }

    void onSetNodeAttribute(Node::ID uid, const std::string& name, VariableType type, const std::string& value)
    {
        EarthGeoPoint::ID vid = m_NodeMap.getLocalID(uid);

        FloatVariable vfloat;
        Vec2Variable vvec2;
        Vec3Variable vvec3;
        Vec4Variable vvec4;

        if (name == "world:geolocation" && type == RD_VEC2)
        {
           vvec2.set(value);
           // TODO : Update edges when position changes
           m_GeoNodes[vid]->setPosition(spherical(vvec2.value()[0], vvec2.value()[1], 0.0f));
           glm::quat rotation;
           rotation = glm::angleAxis(glm::radians(vvec2.value()[0] - 90), glm::vec3(0, 0, 1)) * rotation;
           rotation = glm::angleAxis(glm::radians(vvec2.value()[1]), glm::vec3(0, 1, 0)) * rotation;
           m_GeoNodes[vid]->setOrientation(rotation);
    	}
        else if (name == "world:color" && type == RD_VEC3)
        {
            vvec3.set(value);
            static_cast<EarthGeoPoint*>(m_GeoNodes[vid])->setColor(glm::vec4(vvec3.value(), 1.0));
        }
        else if (name == "world:color" && type == RD_VEC4)
        {
            vvec4.set(value);
            static_cast<EarthGeoPoint*>(m_GeoNodes[vid])->setColor(vvec4.value());
        }
        else if (name == "world:size" && type == RD_FLOAT)
        {
            vfloat.set(value);
            static_cast<EarthGeoPoint*>(m_GeoNodes[vid])->setSize(vfloat.value());
        }
        else
        {
            LOG("[EARTH] Attribute '%s' ignored.\n", name.c_str());
        }
    }

    void onAddEdge(Edge::ID uid, Node::ID uid1, Node::ID uid2)
    {
        EarthGeoPoint::ID vid1 = m_NodeMap.getLocalID(uid1);
        EarthGeoPoint::ID vid2 = m_NodeMap.getLocalID(uid2);

        EarthGeoEdge::ID lid = m_GeoEdges.add(new EarthGeoEdge(m_GeoNodes[vid1], m_GeoNodes[vid2]));

        m_EdgeMap.addRemoteID(uid, lid);
    }

    void onRemoveEdge(Edge::ID uid)
    {
        SpaceEdge::ID vid = m_EdgeMap.getLocalID(uid);
        m_GeoEdges.remove(vid);
        m_EdgeMap.eraseRemoteID(uid, vid);
    }

    void onSetEdgeAttribute(Edge::ID uid, const std::string& name, VariableType type, const std::string& value)
    {
        SpaceEdge::ID vid = m_EdgeMap.getLocalID(uid);

        Vec2Variable vvec2;
        Vec3Variable vvec3;
        Vec4Variable vvec4;

        if (name == "world:color" && type == RD_VEC3)
        {
            vvec3.set(value);
            static_cast<EarthGeoEdge*>(m_GeoEdges[vid])->setColor(glm::vec4(vvec3.value(), 1.0));
        }
        else if (name == "world:color" && type == RD_VEC4)
        {
            vvec4.set(value);
            static_cast<EarthGeoEdge*>(m_GeoEdges[vid])->setColor(vvec4.value());
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
	Scene::NodeVector m_GeoEdges;

    NodeTranslationMap m_NodeMap;
    EdgeTranslationMap m_EdgeMap;
};
