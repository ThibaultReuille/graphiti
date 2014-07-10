#pragma once

#include <raindance/Core/Headers.hh>
#include <raindance/Core/Clock.hh>
#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/Mesh.hh>
#include <raindance/Core/Primitives/Quad.hh>
#include <raindance/Core/Resources/Texture.hh>

#include "Entities/MVC.hh"

class MeshView : public GraphView
{
public:
	MeshView()
	{
		LOG("[MESHVIEW] Creating mesh view ...\n");

		m_GraphEntity = NULL;

		m_WindowWidth = glutGet(GLUT_WINDOW_WIDTH);
		m_WindowHeight = glutGet(GLUT_WINDOW_HEIGHT);

		m_Camera2D.setOrthographicProjection(0.0f, (float)m_WindowWidth, 0.0f, (float)m_WindowHeight, 0.001f, 100.f);
		m_Camera2D.lookAt(glm::vec3(0, 0, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_Camera3D.setPerspectiveProjection(60.0f, m_WindowWidth / (float)m_WindowHeight, 0.1f, 1024.0f);
		m_Camera3D.lookAt(glm::vec3(0, 0, 30), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_Font = new Font();

		m_PointIcon = new Icon();
		m_PointIcon->load("disk", Resources_Particle_ball_png, sizeof(Resources_Particle_ball_png));
	}

	virtual ~MeshView()
	{
		SAFE_DELETE(m_Font);
		SAFE_DELETE(m_PointIcon);
	}

	virtual const char* name() const { return "mesh"; }

    virtual bool bind(Entity* entity)
    {
        if (entity->type() != Entity::GRAPH)
        {
            LOG("[MESH] Couldn't bind entity to view : Wrong entity type!\n");
            return false;
        }

        m_GraphEntity = static_cast<GraphEntity*>(entity);
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
		glClearColor(0.2, 0.2, 0.2, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Transformation transformation;

		glm::vec3 position = glm::vec3(0.0, 0.0, 0.0);

		float iconSize = 5;
		glm::mat4 modelView = m_Camera3D.getViewMatrix() * glm::translate(transformation.state(), position);
		modelView = glm::scale(Geometry::billboard(modelView), glm::vec3(iconSize, iconSize, iconSize));
		m_PointIcon->draw(context(), m_Camera3D.getProjectionMatrix() * modelView, glm::vec4(1.0, 1.0, 1.0, 1.0), 0);
	}

	virtual void idle()
	{
	}

	virtual IVariable* getNodeAttribute(Node::ID id, std::string& name)
	{
		LOG("[MESHVIEW] getNodeAttribute(%lu, %s)\n", id, name.c_str());
		return NULL;
	}

	virtual IVariable* getLinkAttribute(Link::ID id, std::string& name)
	{
		LOG("[MESHVIEW] getLinkAttribute(%lu, %s)\n", id, name.c_str());
		return NULL;
	}

	void notify(IMessage* message)
	{
		(void) message;
	}

	// ----- Graph Events -----

	virtual void onSetAttribute(const std::string& name, VariableType type, const std::string& value)
	{
		(void) name;
		(void) type;
		(void) value;
	}

	void onAddNode(Node::ID id, const char* label)
	{
		(void) id;
		(void) label;
	}

	void onRemoveNode(Node::ID id) { (void) id; }

	void onSetNodeAttribute(Node::ID id, const std::string& name, VariableType type, const std::string& value)
	{
		(void) id;
		(void) name;
		(void) type;
		(void) value;
	}

	void onSetNodeLabel(Node::ID id, const char* label)
	{
		(void) id;
		(void) label;
	}

	void onTagNode(Node::ID node, Sphere::ID sphere)
	{
		(void) node;
		(void) sphere;
	}

	void onAddLink(Link::ID uid, Node::ID uid1, Node::ID uid2)
	{
		(void) uid;
		(void) uid1;
		(void) uid2;
	}

	void onRemoveLink(Link::ID id)
	{
		(void) id;
	}

	void onSetLinkAttribute(Link::ID id, const std::string& name, VariableType type, const std::string& value)
	{
		(void) id;
		(void) name;
		(void) type;
		(void) value;
	}

	void onAddSphere(Sphere::ID id, const char* label)
	{
		(void) id;
		(void) label;
	}

	void onAddNeighbor(const std::pair<Node::ID, Link::ID>& element, const char* label, Node::ID neighbor)
	{
		(void) element;
		(void) label;
		(void) neighbor;
	}

    inline GraphContext* context() { return static_cast<GraphContext*>(m_GraphEntity->context()); }
    inline GraphModel* model() { return static_cast<GraphModel*>(m_GraphEntity->model()); }

private:
	unsigned int m_WindowWidth;
	unsigned int m_WindowHeight;

	GraphEntity* m_GraphEntity;

	Camera m_Camera2D;
	Camera m_Camera3D;

	Icon* m_PointIcon;

	Font* m_Font;
};

