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

	struct NodeParticle
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

    struct EdgeParticle
    {
        glm::vec3 Origin;
    };

	struct Edge
	{
		typedef unsigned int ID;

		glm::vec3 SourcePosition;
        glm::vec4 SourceColor;

        glm::vec3 TargetPosition;
        glm::vec4 TargetColor;

		float Width;
	};

	GPUGraph()
	{
		m_Icon = new Icon();
        m_Icon->load("network/shapes/disk", FS::BinaryFile("Assets/NetworkView/shape_disk.png"));

		FS::TextFile nodes_vert("Assets/NetworkView/nodes.vert");
		FS::TextFile nodes_geom("Assets/NetworkView/nodes.geom");
		FS::TextFile nodes_frag("Assets/NetworkView/nodes.frag");
        m_NodeShader = ResourceManager::getInstance().loadShader("NetworkView/nodes",
        	nodes_vert.content(),
        	nodes_frag.content(),
        	nodes_geom.content());
        // m_NodeShader->dump();

        FS::TextFile edges_vert("Assets/NetworkView/edges.vert");
		FS::TextFile edges_geom("Assets/NetworkView/edges.geom");
		FS::TextFile edges_frag("Assets/NetworkView/edges.frag");
        m_EdgeShader = ResourceManager::getInstance().loadShader("NetworkView/edges",
        	edges_vert.content(),
        	edges_frag.content(),
        	edges_geom.content());
        m_EdgeShader->dump();

        m_NeedsUpdate = true;
	}

	virtual ~GPUGraph()
	{
		SAFE_DELETE(m_Icon);
		ResourceManager::getInstance().unload(m_NodeShader);
	}

	void addNode(const Node& node)
	{
		m_NodeBuffer.push(&node, sizeof(Node));
        m_NeedsUpdate = true;
	}

	void addEdge(const Edge& edge)
	{
		m_EdgeBuffer.push(&edge, sizeof(Edge));
        m_NeedsUpdate = true;
	}

	void update()
	{
        if (!m_NeedsUpdate)
            return;

		// --- Define Node Geometry ---

        m_NodeParticleBuffer << glm::vec3(0, 0, 0);
        m_NodeParticleBuffer.describe("a_Origin", 3, GL_FLOAT, sizeof(NodeParticle), 0);
        m_NodeParticleBuffer.generate(Buffer::STATIC);

        m_NodeBuffer.describe("a_Position", 3, GL_FLOAT, sizeof(Node), 0);
        m_NodeBuffer.describe("a_Size", 1, GL_FLOAT, sizeof(Node), sizeof(glm::vec3));
        m_NodeBuffer.describe("a_Color", 4, GL_FLOAT, sizeof(Node), sizeof(glm::vec3) + sizeof(GL_FLOAT));
        m_NodeBuffer.generate(Buffer::DYNAMIC);

        // --- Define Edge Geometry ---
       
        m_EdgeParticleBuffer << glm::vec3(0, 0, 0);
        m_EdgeParticleBuffer.describe("a_Origin", 3, GL_FLOAT, sizeof(EdgeParticle), 0);
        m_EdgeParticleBuffer.generate(Buffer::STATIC);

        m_EdgeBuffer.describe("a_SourcePosition", 3, GL_FLOAT, sizeof(Edge), 0);
        m_EdgeBuffer.describe("a_SourceColor",    4, GL_FLOAT, sizeof(Edge), sizeof(glm::vec3));
        m_EdgeBuffer.describe("a_TargetPosition", 3, GL_FLOAT, sizeof(Edge), sizeof(glm::vec3) + sizeof(glm::vec4));
        m_EdgeBuffer.describe("a_TargetColor",    4, GL_FLOAT, sizeof(Edge), 2 * sizeof(glm::vec3) + sizeof(glm::vec4));
        m_EdgeBuffer.describe("a_Width",          1, GL_FLOAT, sizeof(Edge), 2 * sizeof(glm::vec3) + 2 * sizeof(glm::vec4));
        
        m_EdgeBuffer.generate(Buffer::DYNAMIC);  

        m_NeedsUpdate = false;     
	}

	virtual void drawEdges(Context* context, Camera& camera, Transformation& transformation)
	{
        if (!m_EdgeBuffer.isGenerated() || m_EdgeBuffer.size() == 0)
            return;

        m_EdgeShader->use();
        //m_EdgeShader->uniform("u_Texture").set(m_Icon->getTexture(0));

        m_EdgeShader->uniform("u_ModelViewMatrix").set(camera.getViewMatrix() * transformation.state());
        m_EdgeShader->uniform("u_ProjectionMatrix").set(camera.getProjectionMatrix());

        context->geometry().bind(m_EdgeParticleBuffer, *m_EdgeShader);        
        context->geometry().bind(m_EdgeBuffer, *m_EdgeShader);
        
        glVertexAttribDivisorARB(m_EdgeShader->attribute("a_Origin").location(), 0); // Same vertices per instance

        glVertexAttribDivisorARB(m_EdgeShader->attribute("a_SourcePosition").location(), 1);
        glVertexAttribDivisorARB(m_EdgeShader->attribute("a_SourceColor").location(), 1);

        glVertexAttribDivisorARB(m_EdgeShader->attribute("a_TargetPosition").location(), 1);
        glVertexAttribDivisorARB(m_EdgeShader->attribute("a_TargetColor").location(), 1);
        
        glVertexAttribDivisorARB(m_EdgeShader->attribute("a_Width").location(), 1);

        context->geometry().drawArraysInstanced(GL_POINTS, 0, m_EdgeParticleBuffer.size() / sizeof(EdgeParticle), m_EdgeBuffer.size() / sizeof(Edge));
        
        context->geometry().unbind(m_EdgeParticleBuffer);
        context->geometry().unbind(m_EdgeBuffer);
	}

    virtual void drawNodes(Context* context, Camera& camera, Transformation& transformation)
    {       
        if (!m_NodeBuffer.isGenerated() || m_NodeBuffer.size() == 0)
            return;

        m_NodeShader->use();
        m_NodeShader->uniform("u_Texture").set(m_Icon->getTexture(0));

        m_NodeShader->uniform("u_ModelViewMatrix").set(camera.getViewMatrix() * transformation.state());
        m_NodeShader->uniform("u_ProjectionMatrix").set(camera.getProjectionMatrix());

        context->geometry().bind(m_NodeParticleBuffer, *m_NodeShader);        
        context->geometry().bind(m_NodeBuffer, *m_NodeShader);
        
        glVertexAttribDivisorARB(m_NodeShader->attribute("a_Origin").location(), 0); // Same vertices per instance

        glVertexAttribDivisorARB(m_NodeShader->attribute("a_Position").location(), 1);
        glVertexAttribDivisorARB(m_NodeShader->attribute("a_Size").location(), 1);
        glVertexAttribDivisorARB(m_NodeShader->attribute("a_Color").location(), 1);

        context->geometry().drawArraysInstanced(GL_POINTS, 0, m_NodeParticleBuffer.size() / sizeof(NodeParticle), m_NodeBuffer.size() / sizeof(Node));
        
        context->geometry().unbind(m_NodeParticleBuffer);
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

    inline Buffer& getNodes() { return m_NodeBuffer; }
    inline Buffer& getEdges() { return m_EdgeBuffer; }

    inline Node getNode(Node::ID id)
    {
        Node node;
        m_NodeBuffer.get(id, &node, sizeof(Node));
        return node;
    }

    inline void setNode(Node::ID id, const Node& node)
    {
        m_NodeBuffer.set(id, &node, sizeof(Node));
        m_NeedsUpdate = true;
    }

    inline Edge getEdge(Edge::ID id)
    {
        Edge edge;
        m_EdgeBuffer.get(id, &edge, sizeof(Edge));
        return edge;
    }

    inline void setEdge(Edge::ID id, const Edge& edge)
    {
        m_EdgeBuffer.set(id, &edge, sizeof(Edge));
        m_NeedsUpdate = true;
    }

private:
	Icon* m_Icon;
    bool m_NeedsUpdate;

	Shader::Program* m_NodeShader;
    Buffer m_NodeParticleBuffer;
	Buffer m_NodeBuffer;

	Shader::Program* m_EdgeShader;
    Buffer m_EdgeParticleBuffer;
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
	}

	virtual ~NetworkView()
	{
		SAFE_DELETE(m_Font);

		SAFE_DELETE(m_Graph);
	}

	virtual const char* name() const { return "network"; }

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
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Transformation transformation;

		m_Graph->draw(context(), m_Camera3D, transformation);
	}

	virtual void idle()
	{
        m_Graph->update();
	}

	void notify(IMessage* message)
	{
		(void) message;
	}

    // ----- Helpers -----

    void checkNodeUID(GPUGraph::Node::ID uid)
    {
        if (!m_NodeMap.containsRemoteID(uid))
        {
            LOG("Node UID %u not found !\n", uid);
            throw;
        }
    }

    void checkEdgeUID(GPUGraph::Edge::ID uid)
    {
        if (!m_EdgeMap.containsRemoteID(uid))
        {
            LOG("Edge UID %u not found !\n", uid);
            throw;
        }
    }

	// ----- Graph Events -----


    void onSetAttribute(const std::string& name, VariableType type, const std::string& value) override
    {
        LOG("onSetAttribute(%s, %i, %s)\n", name.c_str(), (int)type, value.c_str());
    }

    void onAddNode(Node::ID uid, const char* label) override // TODO : Attributes in Variables object
    {
        LOG("onAddNode(%lu, %s)\n", uid, label);

        GPUGraph::Node node;
            
        // TODO : Emitter system? (Define where particles appear)

        node.Position = 10.0f * glm::vec3(
            RANDOM_FLOAT(-1.0, 1.0),
            RANDOM_FLOAT(-1.0, 1.0),
            RANDOM_FLOAT(-1.0, 1.0));
        
        node.Size = 1.0;

        node.Color = glm::vec4(1.0, 1.0, 1.0, 1.0);

        m_Graph->addNode(node);

        static unsigned int node_count = 0;

        GPUGraph::Node::ID nid = node_count;
        m_NodeMap.addRemoteID(uid, nid);

        node_count++;
    }

    void onRemoveNode(Node::ID uid) override
    {
        LOG("onRemoveNode(%lu)\n", uid);
    }

    void onSetNodeAttribute(Node::ID uid, const std::string& name, VariableType type, const std::string& value) override
    {
        LOG("onSetNodeAttribute(%lu, %s, %i, %s)\n", uid, name.c_str(), (int)type, value.c_str());

        BooleanVariable vbool;
        Vec3Variable vvec3;
        Vec4Variable vvec4;
        FloatVariable vfloat;

        checkNodeUID(uid);

        GPUGraph::Node::ID id = m_NodeMap.getLocalID(uid);

        if ((name == "space:position" || name == "particles:position") && type == RD_VEC3)
        {
            vvec3.set(value);

            GPUGraph::Node node = m_Graph->getNode(id);
            node.Position = vvec3.value();
            m_Graph->setNode(id, node);

        }
        else if (name == "space:color" && (type == RD_VEC3 || type == RD_VEC4))
        {
            glm::vec4 c;

            if (type == RD_VEC3)
            {
                vvec3.set(value);
                c = glm::vec4(vvec3.value(), 1.0);
            }
            else
            {
                vvec4.set(value);
                c = vvec4.value();
            }

            GPUGraph::Node node = m_Graph->getNode(id);
            node.Color = vvec4.value();
            m_Graph->setNode(id, node);
        }
        else if (name == "space:size" && type == RD_FLOAT)
        {
            vfloat.set(value);

            GPUGraph::Node node = m_Graph->getNode(id);
            node.Size = vfloat.value();
            m_Graph->setNode(id, node);        
        }
    }

    void onAddLink(Link::ID uid, Node::ID uid1, Node::ID uid2) override
    {
        LOG("onAddLink(%lu, %lu, %lu)\n", uid, uid1, uid2);

        GPUGraph::Node::ID nid1 = m_NodeMap.getLocalID(uid1);
        GPUGraph::Node::ID nid2 = m_NodeMap.getLocalID(uid2);

        GPUGraph::Edge edge;

        GPUGraph::Node n1 = m_Graph->getNode(nid1);
        GPUGraph::Node n2 = m_Graph->getNode(nid2);

        edge.SourcePosition = n1.Position;
        edge.SourceColor = n1.Color;

        edge.TargetPosition = n2.Position;
        edge.TargetColor = n2.Color;

        edge.Width = 0.5; // NOTE : Half of the node size

        m_Graph->addEdge(edge);

        static unsigned int edge_count = 0;

        GPUGraph::Edge::ID eid = edge_count;
        m_EdgeMap.addRemoteID(uid, eid);

        edge_count++;
    }

    void onRemoveLink(Link::ID uid) override
    {
        LOG("onRemoveLink(%lu)\n", uid);
    }

    void onSetLinkAttribute(Link::ID uid, const std::string& name, VariableType type, const std::string& value) override
    {
        LOG("onSetLinkAttribute(%lu %s, %i, %s)\n", uid, name.c_str(), (int)type, value.c_str());

        FloatVariable vfloat;
        Vec3Variable vvec3;
        Vec4Variable vvec4;
        StringVariable vstring;

        checkEdgeUID(uid);
        GPUGraph::Edge::ID id = m_EdgeMap.getLocalID(uid);

        if (name == "space:color1" && type == RD_VEC4)
        {
            vvec4.set(value);
            
            GPUGraph::Edge edge = m_Graph->getEdge(id);
            edge.SourceColor = vvec4.value();
            m_Graph->setEdge(id, edge);
        }
        else if (name == "space:color2" && type == RD_VEC4)
        {
            vvec4.set(value);
            
            GPUGraph::Edge edge = m_Graph->getEdge(id);
            edge.TargetColor = vvec4.value();
            m_Graph->setEdge(id, edge);
        }
        else if (name == "space:width" && type == RD_FLOAT)
        {
            vfloat.set(value);
            
            GPUGraph::Edge edge = m_Graph->getEdge(id);
            edge.Width = vfloat.value();
            m_Graph->setEdge(id, edge);
        }
    }

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
    TranslationMap<GPUGraph::Node::ID, unsigned int> m_NodeMap;
    TranslationMap<GPUGraph::Edge::ID, unsigned int> m_EdgeMap;
};

