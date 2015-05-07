#pragma once

#include <raindance/Core/Headers.hh>
#include <raindance/Core/Clock.hh>
#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/Primitives/Axis.hh>
#include <raindance/Core/Primitives/Quad.hh>
#include <raindance/Core/Resources/Texture.hh>

#include <graphiti/Entities/MVC.hh>

#include <graphiti/Visualizers/Network/GPUGraph.hh>

class NetworkView : public GraphView
{
public:

	NetworkView()
	{
		LOG("[NETWORK] Creating network view ...\n");

        m_Axis = new Axis();

		m_GraphEntity = NULL;
		m_Font = new rd::Font();
		m_Graph = new GPUGraph();
	}

	virtual ~NetworkView()
	{
		SAFE_DELETE(m_Font);
        SAFE_DELETE(m_Axis);
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

	void draw(Context* context) override
	{
		glClearColor(0.2, 0.2, 0.2, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
		Transformation transformation;

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);

        transformation.push();
        transformation.scale(glm::vec3(10, 10, 10));
        m_Axis->draw(context, m_Camera3D, transformation);
        transformation.pop();

    	m_Graph->draw(context, m_Camera3D, transformation);
	}

	void idle(Context* context) override
	{
        static bool m_Initialized = false;
        if (!m_Initialized)
        {
            m_Graph->initialize(context);
            m_Initialized = true;
        }

        m_Graph->idle(context);
	}

	void notify(IMessage* message)
	{
		(void) message;
	}

    // ----- Helpers -----

    void checkNodeUID(GPUGraph::NodeInstance::ID uid)
    {
        if (!m_NodeMap.containsRemoteID(uid))
        {
            LOG("Node UID %u not found !\n", uid);
            throw;
        }
    }

    void checkEdgeUID(GPUGraph::EdgeInstance::ID uid)
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
        BooleanVariable vbool;

        if ((name == "space:physics" || name == "network:physics") && type == RD_BOOLEAN)
        {
            vbool.set(value);
            m_Graph->setPhysics(vbool.value());
        }
    }

    virtual IVariable* getAttribute(const std::string& name)
    {
        LOG("[NETWORK] getAttribute(%s)\n", name.c_str());

        return NULL;
    }

    void onAddNode(Node::ID uid, const char* label) override // TODO : Attributes in Variables object
    {
        (void) label; // TODO

        GPUGraph::NodeInstance node;
            
        // TODO : Emitter system? (Define where particles appear)

        node.Position = 10.0f * glm::vec4
        (
            RANDOM_FLOAT(-1.0, 1.0),
            RANDOM_FLOAT(-1.0, 1.0),
            RANDOM_FLOAT(-1.0, 1.0),
            0.0
        );

        node.Color = glm::vec4(1.0, 1.0, 1.0, 1.0);

        node.Size = 1.0;
        
        m_Graph->addNode(node);

        static unsigned int node_count = 0;

        GPUGraph::NodeInstance::ID nid = node_count;
        m_NodeMap.addRemoteID(uid, nid);

        node_count++;
    }

    void onRemoveNode(Node::ID uid) override
    {
        LOG("onRemoveNode(%lu)\n", uid);
    }

    void onSetNodeAttribute(Node::ID uid, const std::string& name, VariableType type, const std::string& value) override
    {
        BooleanVariable vbool;
        Vec3Variable vvec3;
        Vec4Variable vvec4;
        FloatVariable vfloat;

        checkNodeUID(uid);

        GPUGraph::NodeInstance::ID id = m_NodeMap.getLocalID(uid);

        if ((name == "space:position" || name == "network:position") && type == RD_VEC3)
        {
            vvec3.set(value);

            GPUGraph::NodeInstance node = m_Graph->getNode(id);
            node.Position = glm::vec4(vvec3.value(), 0.0);
            m_Graph->setNode(id, node);

        }
        else if ((name == "space:color" || name == "network:color") && (type == RD_VEC3 || type == RD_VEC4))
        {
            glm::vec4 c;

            if (type == RD_VEC3)
            {
                vvec3.set(value);
                c = glm::vec4(vvec3.value(), 0.0);
            }
            else
            {
                vvec4.set(value);
                c = vvec4.value();
            }

            GPUGraph::NodeInstance node = m_Graph->getNode(id);
            node.Color = vvec4.value();
            m_Graph->setNode(id, node);
        }
        else if ((name == "space:size" || name == "network:size") && type == RD_FLOAT)
        {
            vfloat.set(value);

            GPUGraph::NodeInstance node = m_Graph->getNode(id);
            node.Size = vfloat.value();
            m_Graph->setNode(id, node);        
        }
    }

    virtual IVariable* getNodeAttribute(Node::ID uid, std::string& name)
    {
        GPUGraph::NodeInstance::ID id = m_NodeMap.getLocalID(uid);
 
        if (name == "position")
        {
            Vec3Variable* variable = new Vec3Variable();
            variable->set(m_Graph->getNode(id).Position.xyz());
            return variable;
        }
        else if (name == "color")
        {
            Vec4Variable* variable = new Vec4Variable();
            variable->set(m_Graph->getNode(id).Color);
            return variable;
        }
        else if (name == "size")
        {
            FloatVariable* variable = new FloatVariable();
            variable->set(m_Graph->getNode(id).Size);
            return variable;
        }
        /* TODO
        else if (name == "locked")
        {
            BooleanVariable* variable = new BooleanVariable();
            variable->set(m_SpaceNodes[id]->isPositionLocked());
            return variable;
        }
        else if (name == "activity")
        {
            FloatVariable* variable = new FloatVariable();
            variable->set(static_cast<SpaceNode*>(m_SpaceNodes[id])->getActivity());
            return variable;
        }
        else if (name == "mark")
        {
            IntVariable* variable = new IntVariable();
            variable->set(static_cast<SpaceNode*>(m_SpaceNodes[id])->getMark());
            return variable;
        }
        */

        return NULL;
    }

    void onAddEdge(Edge::ID uid, Node::ID uid1, Node::ID uid2) override
    {
        // LOG("onAddEdge(%lu, %lu, %lu)\n", uid, uid1, uid2);

        GPUGraph::NodeInstance::ID nid1 = m_NodeMap.getLocalID(uid1);
        GPUGraph::NodeInstance::ID nid2 = m_NodeMap.getLocalID(uid2);

        GPUGraph::NodeInstance n1 = m_Graph->getNode(nid1);
        GPUGraph::NodeInstance n2 = m_Graph->getNode(nid2);

        GPUGraph::EdgeInstance edge;

        edge.SourceID = nid1;
        edge.SourcePosition = n1.Position;
        edge.SourceColor = n1.Color;

        edge.TargetID = nid2;
        edge.TargetPosition = n2.Position;
        edge.TargetColor =  n2.Color;

        edge.Width = 0.5; // NOTE : Half of the node size

        m_Graph->addEdge(edge);

        static unsigned int edge_count = 0;

        GPUGraph::EdgeInstance::ID eid = edge_count;
        m_EdgeMap.addRemoteID(uid, eid);

        edge_count++;
    }

    void onRemoveEdge(Edge::ID uid) override
    {
        LOG("onRemoveEdge(%lu)\n", uid);
    }

    void onSetEdgeAttribute(Edge::ID uid, const std::string& name, VariableType type, const std::string& value) override
    {
        FloatVariable vfloat;
        Vec3Variable vvec3;
        Vec4Variable vvec4;
        StringVariable vstring;

        checkEdgeUID(uid);
        GPUGraph::EdgeInstance::ID id = m_EdgeMap.getLocalID(uid);

        if ((name == "space:color" || name == "network:color") && type == RD_VEC4)
        {
            vvec4.set(value);
            
            GPUGraph::EdgeInstance edge = m_Graph->getEdge(id);
            edge.SourceColor = vvec4.value();
            edge.TargetColor = vvec4.value();
            m_Graph->setEdge(id, edge);
        }
        else if ((name == "space:color1" || name == "network:color1") && type == RD_VEC4)
        {
            vvec4.set(value);
            
            GPUGraph::EdgeInstance edge = m_Graph->getEdge(id);
            edge.SourceColor = vvec4.value();
            m_Graph->setEdge(id, edge);
        }
        else if ((name == "space:color2" || name == "network:color2") && type == RD_VEC4)
        {
            vvec4.set(value);
            
            GPUGraph::EdgeInstance edge = m_Graph->getEdge(id);
            edge.TargetColor = vvec4.value();
            m_Graph->setEdge(id, edge);
        }
        else if ((name == "space:width" || name == "network:color") && type == RD_FLOAT)
        {
            vfloat.set(value);
            
            GPUGraph::EdgeInstance edge = m_Graph->getEdge(id);
            edge.Width = vfloat.value();
            m_Graph->setEdge(id, edge);
        }
    }

    virtual IVariable* getEdgeAttribute(Edge::ID uid, std::string& name)
    {
        GPUGraph::EdgeInstance::ID id = m_EdgeMap.getLocalID(uid);
 
        if (name == "color1")
        {
            Vec4Variable* variable = new Vec4Variable();
            variable->set(m_Graph->getEdge(id).SourceColor);
            return variable;
        }
        else if (name == "color2")
        {
            Vec4Variable* variable = new Vec4Variable();
            variable->set(m_Graph->getEdge(id).TargetColor);
            return variable;
        }
        else if (name == "width")
        {
            FloatVariable* variable = new FloatVariable();
            variable->set(m_Graph->getEdge(id).Width);
            return variable;
        }
        /*
        else if (name == "activity")
        {
            FloatVariable* variable = new FloatVariable();
            variable->set();
            return variable;
        }
        */

        return NULL;
    }

    inline GraphContext* context() { return static_cast<GraphContext*>(m_GraphEntity->context()); }

    inline GraphModel* model() { return static_cast<GraphModel*>(m_GraphEntity->model()); }

private:
	GraphEntity* m_GraphEntity;

	Camera m_Camera2D;
	Camera m_Camera3D;

	rd::Font* m_Font;

    Axis* m_Axis;

	GPUGraph* m_Graph;
    TranslationMap<GPUGraph::NodeInstance::ID, unsigned int> m_NodeMap;
    TranslationMap<GPUGraph::EdgeInstance::ID, unsigned int> m_EdgeMap;
};

