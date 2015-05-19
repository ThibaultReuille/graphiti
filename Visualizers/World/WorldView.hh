#pragma once

#include <raindance/Core/Headers.hh>
#include <raindance/Core/Clock.hh>
#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/Bezier.hh>
#include <raindance/Core/PointCloud.hh>
#include <raindance/Core/Primitives/Line.hh>
#include <raindance/Core/Primitives/Cube.hh>
#include <raindance/Core/Material.hh>
#include <raindance/Core/Light.hh>
#include <raindance/Core/Resources/Texture.hh>

#include <graphiti/Entities/MVC.hh>

#include <graphiti/Visualizers/World/WorldResources.hh>
#include <graphiti/Visualizers/World/Earth.hh>
#include <graphiti/Visualizers/World/WorldMap.hh>

typedef TranslationMap<SpaceNode::ID, Node::ID> NodeTranslationMap;
typedef TranslationMap<SpaceEdge::ID, Edge::ID> EdgeTranslationMap;

class WorldView : public GraphView
{
public:

	WorldView()
	{
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
		m_WindowWidth = (unsigned int) getViewport().getDimension()[0];
		m_WindowHeight = (unsigned int) getViewport().getDimension()[1];

		m_Camera2D.setOrthographicProjection(0.0f, (float)m_WindowWidth, 0.0f, (float)m_WindowHeight, 0.001f, 100.f);
		m_Camera2D.lookAt(glm::vec3(0, 0, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_Camera3D.setPerspectiveProjection(60.0f, m_WindowWidth / (float)m_WindowHeight, 0.1f, 1024.0f);
		m_Camera3D.lookAt(glm::vec3(30, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_GraphEntity = NULL;

		g_WorldResources = new WorldResources();

		float texture_ratio = 2048.0f / 1502.0f;
		m_WorldMap = new WorldMap_Old(m_WindowHeight * texture_ratio, m_WindowHeight);

		m_Earth = new Earth();

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

	void draw(Context* ctx) override
	{
		(void) ctx;

		//glClearColor(0.0, 0.0, 0.0, 1.0);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

	virtual void onSetAttribute(const std::string& name, VariableType type, const std::string& value)
	{
		(void) type;
		LOG("[WORLDVIEW] setAttribute(%s, %s)\n", name.c_str(), value.c_str());
	}

    virtual IVariable* getAttribute(const std::string& name)
    {
        (void) name;
        return NULL;
    }

	virtual IVariable* getNodeAttribute(Node::ID id, std::string& name)
	{
		LOG("[WORLDVIEW] getNodeAttribute(%lu, %s)\n", id, name.c_str());
		return NULL;
	}

	virtual IVariable* getEdgeAttribute(Edge::ID id, std::string& name)
	{
		LOG("[WORLDVIEW] getEdgeAttribute(%lu, %s)\n", id, name.c_str());
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

	void onAddEdge(Edge::ID uid, Node::ID uid1, Node::ID uid2)
	{
	    m_Earth->onAddEdge(uid, uid1, uid2);
	    // m_WorldMap->onAddEdge(uid, uid1, uid2);
	}

	void onRemoveEdge(Edge::ID uid)
	{
	    m_Earth->onRemoveEdge(uid);
	    // m_WorldMap->onAddEdge(uid);
	}

	void onSetEdgeAttribute(Edge::ID uid, const std::string& name, VariableType type, const std::string& value)
	{
        m_Earth->onSetEdgeAttribute(uid, name, type, value);
        // m_WorldMap->onSetEdgeAttribute(uid, name, type, value);
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

	void onAddNeighbor(const std::pair<Node::ID, Edge::ID>& element, const char* label, Node::ID neighbor)
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

	WorldMap_Old* m_WorldMap;
	Earth* m_Earth;
};

