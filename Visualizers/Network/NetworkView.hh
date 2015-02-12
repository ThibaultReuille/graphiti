#pragma once

#include <raindance/Core/Headers.hh>
#include <raindance/Core/Clock.hh>
#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/Primitives/Quad.hh>
#include <raindance/Core/Resources/Texture.hh>

#include "Entities/MVC.hh"

class GPUGraph 
{
public:

	struct Vertex
	{
		glm::vec3 Origin;
	};

	struct Node
	{
		typedef unsigned int ID;

        glm::vec3 Position;
        float Size;
        glm::vec4 Color;
	};

	struct Edge
	{
		typedef unsigned int ID;

		Node::ID Source;
		Node::ID Target;

		float Width;
	};

	GPUGraph()
	{
		m_Icon = new Icon();
        m_Icon->load("network/shapes/disk", FS::BinaryFile("Assets/NetworkView/shape_disk.png"));

		FS::TextFile vert("Assets/NetworkView/particles.vert");
		FS::TextFile geom("Assets/NetworkView/particles.geom");
		FS::TextFile frag("Assets/NetworkView/particles.frag");
        m_Shader = ResourceManager::getInstance().loadShader("NetworkView/particles", vert.content(), frag.content(), geom.content());
        m_Shader->dump();
	}

	virtual ~GPUGraph()
	{
		SAFE_DELETE(m_Icon);
		ResourceManager::getInstance().unload(m_Shader);
	}

	void addNode(const Node& node)
	{
		m_NodeBuffer.push(&node, sizeof(Node));
	}

	void addEdge(const Edge& edge)
	{
		m_EdgeBuffer.push(&edge, sizeof(Edge));
	}

	void update()
	{
		// --- Define Node Geometry ---
        m_VertexBuffer << glm::vec3(0, 0, 0);
        m_VertexBuffer.describe("a_Origin", 3, GL_FLOAT, sizeof(Vertex), 0);
        m_VertexBuffer.generate(Buffer::STATIC);

		// --- Define Node Buffer Structure ---
        m_NodeBuffer.describe("a_Position", 3, GL_FLOAT, sizeof(Node), 0);
        m_NodeBuffer.describe("a_Size", 1, GL_FLOAT, sizeof(Node), sizeof(glm::vec3));
        m_NodeBuffer.describe("a_Color", 4, GL_FLOAT, sizeof(Node), sizeof(glm::vec3) + sizeof(GL_FLOAT));
        m_NodeBuffer.generate(Buffer::DYNAMIC);
	}


	virtual void drawEdges(Context* context, Camera& camera, Transformation& transformation)
	{

	}

    virtual void drawNodes(Context* context, Camera& camera, Transformation& transformation)
    {        
        m_Shader->use();
        m_Shader->uniform("u_Texture").set(m_Icon->getTexture(0));

        m_Shader->uniform("u_ModelViewMatrix").set(camera.getViewMatrix() * transformation.state());
        m_Shader->uniform("u_ProjectionMatrix").set(camera.getProjectionMatrix());

        context->geometry().bind(m_VertexBuffer, *m_Shader);        
        context->geometry().bind(m_NodeBuffer, *m_Shader);
        
        glVertexAttribDivisorARB(m_Shader->attribute("a_Origin").location(), 0); // Same vertices per instance

        glVertexAttribDivisorARB(m_Shader->attribute("a_Position").location(), 1);
        glVertexAttribDivisorARB(m_Shader->attribute("a_Size").location(), 1);
        glVertexAttribDivisorARB(m_Shader->attribute("a_Color").location(), 1);

        context->geometry().drawArraysInstanced(GL_POINTS, 0, m_VertexBuffer.size() / sizeof(Vertex), m_NodeBuffer.size() / sizeof(Node));
        
        context->geometry().unbind(m_VertexBuffer);
        context->geometry().unbind(m_NodeBuffer);
    }

    virtual void draw(Context* context, Camera& camera, Transformation& transformation)
    {
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);

        drawEdges(context, camera, transformation);

        drawNodes(context, camera, transformation);
    }

private:
	Icon* m_Icon;

	Shader::Program* m_Shader;
	Buffer m_VertexBuffer;

	Buffer m_NodeBuffer;
	Buffer m_EdgeBuffer;
};


class NetworkView : public GraphView
{
public:

	NetworkView()
	{
		LOG("[NETWORK] Creating network view ...\n");

		m_GraphEntity = NULL;

		m_Font = new Font();

		m_Graph = new GPUGraph();

        int node_count = 100000;
        int edge_count = 100000;

        for (int i = 0; i < node_count; i++)
        {
            float d = 100;

            GPUGraph::Node node;
            
            node.Position = d * glm::vec3(
                RANDOM_FLOAT(-1.0, 1.0),
                RANDOM_FLOAT(-1.0, 1.0),
                RANDOM_FLOAT(-1.0, 1.0));
            
            node.Size = RANDOM_FLOAT(0.1, 0.5);

            node.Color = glm::vec4(
                RANDOM_FLOAT(0.0, 1.0),
                RANDOM_FLOAT(0.0, 1.0),
                RANDOM_FLOAT(0.0, 1.0),
                1.0);

            m_Graph->addNode(node);
        }

        for (int i = 0; i < edge_count; i++)
        {
        	GPUGraph::Edge edge;

        	edge.Source = RANDOM_INT(0, node_count);
        	edge.Target = RANDOM_INT(0, node_count);

        	m_Graph->addEdge(edge);
        }

        m_Graph->update();
	}

	virtual ~NetworkView()
	{
		SAFE_DELETE(m_Font);

		SAFE_DELETE(m_Graph);
	}

	virtual const char* name() const { return "mesh"; }

    virtual bool bind(Entity* entity)
    {
        if (entity->type() != Entity::GRAPH)
        {
            LOG("[NETWORK] Couldn't bind entity to view : Wrong entity type!\n");
            return false;
        }

		m_Camera2D.setOrthographicProjection(0.0f, getViewport().getDimension()[0], 0.0f, getViewport().getDimension()[1], 0.001f, 100.f);
		m_Camera2D.lookAt(glm::vec3(0, 0, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_Camera3D.setPerspectiveProjection(60.0f, getViewport().getDimension()[0] / getViewport().getDimension()[1], 0.1f, 1024.0f);
		m_Camera3D.lookAt(glm::vec3(0, 0, 30), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

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

		m_Graph->draw(context(), m_Camera3D, transformation);
	}

	virtual void idle()
	{
	}

	void notify(IMessage* message)
	{
		(void) message;
	}

	// ----- Graph Events -----


	virtual IVariable* getAttribute(const std::string& name)
	{
		(void) name;
		return NULL;
	}

    virtual IVariable* getNodeAttribute(Node::ID id, std::string& name)
    {
    	(void) id;
    	(void) name;
    	return NULL;
    }

    virtual IVariable* getLinkAttribute(Link::ID id, std::string& name)
    {
    	(void) id;
    	(void) name;
    	return NULL;
    }

    inline GraphContext* context() { return static_cast<GraphContext*>(m_GraphEntity->context()); }

    inline GraphModel* model() { return static_cast<GraphModel*>(m_GraphEntity->model()); }

private:
	GraphEntity* m_GraphEntity;

	Camera m_Camera2D;
	Camera m_Camera3D;

	Font* m_Font;

	GPUGraph* m_Graph;
};

