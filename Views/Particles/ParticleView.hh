#pragma once

#include <raindance/Core/View.hh>
#include <raindance/Core/OpenCL.hh>
#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/Camera/Controllers.hh>
#include <raindance/Core/Primitives/Sphere.hh>

#include "Entities/Graph/GraphModel.hh"

class ParticleView : public GraphView
{
public:
    struct ParticleNode
    {
        typedef unsigned long ID;

        glm::vec4 Position;
    };

    struct ParticleEdge
    {
        typedef unsigned long ID;

        unsigned long Node1;
        unsigned long Node2;
    };

    struct Force
    {
        glm::vec4 Direction;
    };

    ParticleView()
    {
        m_GraphEntity = NULL;

        m_Sphere = new SphereMesh(3.0, 7, 10);
        m_Sphere->getVertexBuffer().mute("a_Texcoord", true);
        m_Shader = ResourceManager::getInstance().loadShader("ParticleView/node",
                Resources_ParticleView_node_vert, sizeof(Resources_ParticleView_node_vert),
                Resources_ParticleView_node_frag, sizeof(Resources_ParticleView_node_frag));
        // m_Shader->dump();
   }

    virtual ~ParticleView()
    {
        SAFE_DELETE(m_Sphere);
        ResourceManager::getInstance().unload(m_Shader);
    }

    virtual const char* name() const { return "particles"; }

    virtual bool bind(Entity* entity)
    {
        if (entity->type() != Entity::GRAPH)
        {
            LOG("[PARTICLES] Couldn't bind entity to view : Wrong entity type!\n");
            return false;
        }

        m_GraphEntity = static_cast<GraphEntity*>(entity);
        m_GraphEntity->views().push_back(this);
        m_GraphEntity->listeners().push_back(this);

        initialize();

        return true;
    }

    void initialize()
    {
        m_OpenCL.detect();
        m_OpenCL.dump();

        // NOTE : We are assuming the last device is the best one.
        const OpenCL::Device* device = m_OpenCL.devices().back();
        m_Context = m_OpenCL.createContext(*device);
        m_Queue = m_OpenCL.createCommandQueue(*m_Context);

        std::string source((const char*)Resources_ParticleView_physics_cl, Resources_ParticleView_physics_cl_len);
        OpenCL::Program* program = m_OpenCL.loadProgram(*m_Context, "particles", source);
        m_RepulsionK = m_OpenCL.createKernel(*program, "repulsion");
        m_AttractionK = m_OpenCL.createKernel(*program, "attraction");
        m_MovementK = m_OpenCL.createKernel(*program, "movement");

        m_InputNodeBuffer = NULL;
        m_InputEdgeBuffer = NULL;
        m_ForceBuffer = NULL;
        m_OutputNodeBuffer = NULL;

        // Get the maximum work group size for executing the kernel on the device
        /*
        size_t local; // local domain size for our calculation
        error = clGetKernelWorkGroupInfo(m_OpenCL.kernel(m_KID).Object, m_OpenCL.device(did).ID, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
        if (error != CL_SUCCESS)
        {
            printf("[OpenCL] Error: Failed to retrieve kernel work group info! %d\n", error);
            throw;
        }
        LOG("LOCAL : %zu\n", local);
         */
    }

    virtual void draw()
    {
        glClearColor(0.2, 0.2, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        m_Shader->use();

        glm::mat4 model;

        for (auto n : m_Nodes)
        {
            model = glm::translate(glm::mat4(), glm::vec3(n.Position));
            m_Shader->uniform("u_ModelViewProjectionMatrix").set(m_Camera.getViewProjectionMatrix() * model);

            context()->geometry().bind(m_Sphere->getVertexBuffer(), *m_Shader);
            context()->geometry().drawElements(GL_TRIANGLES, m_Sphere->getIndexBuffer().size() / sizeof(unsigned short int), GL_UNSIGNED_SHORT, m_Sphere->getIndexBuffer().ptr());
            context()->geometry().unbind(m_Sphere->getVertexBuffer());
        }
    }

    void setPhysics(bool flag)
    {
        m_Physics = flag;
        if (flag)
        {
            unsigned long numParticles = m_Nodes.size();
            unsigned long numForces = m_Edges.size();

            float volume = 10 * 10 * 10;
            m_K = pow(volume / numParticles, 1.0 / 3.0);
            m_Temperature = 10.0;

            if (m_InputNodeBuffer != NULL)
                m_OpenCL.destroyBuffer(&m_InputNodeBuffer);
            if (m_InputEdgeBuffer != NULL)
                m_OpenCL.destroyBuffer(&m_InputEdgeBuffer);
            if (m_ForceBuffer != NULL)
                m_OpenCL.destroyBuffer(&m_ForceBuffer);
            if (m_OutputNodeBuffer != NULL)
                m_OpenCL.destroyBuffer(&m_OutputNodeBuffer);

            m_InputNodeBuffer = m_OpenCL.createBuffer(*m_Context, CL_MEM_READ_ONLY, m_Nodes.size() * sizeof(ParticleNode));
            m_InputEdgeBuffer = m_OpenCL.createBuffer(*m_Context, CL_MEM_READ_ONLY, m_Edges.size() * sizeof(ParticleEdge));
            m_ForceBuffer = m_OpenCL.createBuffer(*m_Context, CL_MEM_READ_WRITE, m_Nodes.size() * sizeof(ParticleNode));
            m_OutputNodeBuffer = m_OpenCL.createBuffer(*m_Context, CL_MEM_WRITE_ONLY, m_Nodes.size() * sizeof(ParticleNode));

            m_RepulsionK->setArgument(0, *m_InputNodeBuffer);
            m_RepulsionK->setArgument(1, *m_ForceBuffer);
            m_RepulsionK->setArgument(2, &numParticles, sizeof(unsigned long));
            m_RepulsionK->setArgument(3, &m_K, sizeof(float));

            m_AttractionK->setArgument(0, *m_InputNodeBuffer);
            m_AttractionK->setArgument(1, *m_InputEdgeBuffer);
            m_AttractionK->setArgument(2, *m_ForceBuffer);
            m_AttractionK->setArgument(3, &numForces, sizeof(unsigned long));
            m_AttractionK->setArgument(4, &m_K, sizeof(float));

            m_MovementK->setArgument(0, *m_InputNodeBuffer);
            m_MovementK->setArgument(1, *m_ForceBuffer);
            m_MovementK->setArgument(2, *m_OutputNodeBuffer);
            m_MovementK->setArgument(3, &m_Temperature, sizeof(float));

            m_Iterations = 0;
            m_Clock.reset();
        }
    }

    virtual void idle()
    {
        if (m_Physics)
        {
            unsigned long numParticles = m_Nodes.size();
            unsigned long numForces = m_Edges.size();

            m_OpenCL.enqueueWriteBuffer(*m_Queue, *m_InputNodeBuffer, CL_TRUE, 0, m_Nodes.size() * sizeof(ParticleNode), m_Nodes.data(), 0, NULL, NULL);
            m_OpenCL.enqueueWriteBuffer(*m_Queue, *m_InputEdgeBuffer, CL_TRUE, 0, m_Edges.size() * sizeof(ParticleEdge), m_Edges.data(), 0, NULL, NULL);

            m_OpenCL.enqueueNDRangeKernel(*m_Queue, *m_RepulsionK, 1, NULL, &numParticles, NULL, 0, NULL, NULL);
            m_OpenCL.enqueueNDRangeKernel(*m_Queue, *m_AttractionK, 1, NULL, &numForces, NULL, 0, NULL, NULL);
            m_OpenCL.enqueueNDRangeKernel(*m_Queue, *m_MovementK, 1, NULL, &numParticles, NULL, 0, NULL, NULL);

            clFinish(m_Queue->Object);
            m_OpenCL.enqueueReadBuffer(*m_Queue, *m_OutputNodeBuffer, CL_TRUE, 0, m_Nodes.size() * sizeof(ParticleNode), m_Nodes.data(), 0, NULL, NULL);

            float time = m_Clock.seconds();
            m_Iterations++;
            if (time > 0.0)
            {
                LOG("Iteration : %u, Time : %f, Iteration/sec : %f\n", m_Iterations, time, m_Iterations / time);
            }
        }
    }

    virtual void notify(IMessage* message)
    {
        (void) message;
    }

    // ----- GraphView interface -----

    virtual void onAddNode(Node::ID uid, const char* label)
    {
        (void) uid;
        (void) label;

        ParticleNode n;
        n.Position.x = 200 * RANDOM_FLOAT(-1.0, 1.0);
        n.Position.y = 200 * RANDOM_FLOAT(-1.0, 1.0);
        n.Position.z = 200 * RANDOM_FLOAT(-1.0, 1.0);
        m_Nodes.push_back(n);

        ParticleNode::ID id = m_Nodes.size() - 1;

        m_NodeMap.addRemoteID(uid, id);
    }

    virtual void onAddLink(Link::ID uid, Node::ID uid1, Node::ID uid2)
    {
        checkNodeUID(uid1);
        checkNodeUID(uid2);

        ParticleNode::ID node1 = m_NodeMap.getLocalID(uid1);
        ParticleNode::ID node2 = m_NodeMap.getLocalID(uid2);

        ParticleEdge e;
        e.Node1 = node1;
        e.Node2 = node2;
        m_Edges.push_back(e);

        ParticleEdge::ID id = m_Edges.size() - 1;

        m_EdgeMap.addRemoteID(uid, id);
    }

    virtual void onSetAttribute(const std::string& name, VariableType type, const std::string& value)
    {
        (void) name;
        (void) type;
        (void) value;
    }

    virtual IVariable* getNodeAttribute(Node::ID uid, std::string& name)
    {
        checkNodeUID(uid);

        ParticleNode::ID id = m_NodeMap.getLocalID(uid);

        if (name == "position")
        {
            Vec3Variable* variable = new Vec3Variable();
            variable->set(m_Nodes[id].Position.xyz());
            return variable;
        }

        return NULL;
    }

    virtual IVariable* getLinkAttribute(Link::ID id, std::string& name)
    {
        // TODO : ParticleView::getLinkAttribute
        (void) id;
        (void) name;
        return NULL;
    }

    virtual void onRemoveNode(Node::ID id)
    {
        (void) id;
    }

    virtual void onSetNodeAttribute(Node::ID id, const std::string& name, VariableType type, const std::string& value)
    {
        (void) id;
        (void) name;
        (void) type;
        (void) value;
    }

    virtual void onSetNodeLabel(Node::ID id, const char* label)
    {
        (void) id;
        (void) label;
    }

    virtual void onTagNode(Node::ID node, Sphere::ID sphere)
    {
        (void) node;
        (void) sphere;
    }

    virtual void onRemoveLink(Link::ID id)
    {
        (void) id;
    }

    virtual void onSetLinkAttribute(Link::ID id, const std::string& name, VariableType type, const std::string& value)
    {
        (void) id;
        (void) name;
        (void) type;
        (void) value;
    }

    virtual void onAddNeighbor(const std::pair<Node::ID, Link::ID>& element, const char* label, Node::ID neighbor)
    {
        (void) element;
        (void) label;
        (void) neighbor;
    }

    virtual void onAddSphere(Sphere::ID id, const char* label)
    {
        (void) id;
        (void) label;
    }

    // ----- Accessors / Helpers -----

    inline GraphContext* context() { return static_cast<GraphContext*>(m_GraphEntity->context()); }
    inline GraphModel* model() { return static_cast<GraphModel*>(m_GraphEntity->model()); }

    inline Camera* camera() { return &m_Camera; }

    void checkNodeUID(Node::ID uid)
    {
        if (!m_NodeMap.containsRemoteID(uid))
        {
            LOG("Node UID %lu not found !\n", uid);
            throw;
        }
    }
    void checkLinkUID(Link::ID uid)
    {
        if (!m_EdgeMap.containsRemoteID(uid))
        {
            LOG("Link UID %lu not found !\n", uid);
            throw;
        }
    }

private:
    GraphEntity* m_GraphEntity;

    Camera m_Camera;
    SphericalCameraController m_SphericalCameraController;
    SphereMesh* m_Sphere;
    Shader::Program* m_Shader;

    std::vector<ParticleNode> m_Nodes;
    std::vector<ParticleEdge> m_Edges;
    TranslationMap<ParticleNode::ID, Node::ID> m_NodeMap;
    TranslationMap<ParticleEdge::ID, Link::ID> m_EdgeMap;

    OpenCL m_OpenCL;
    OpenCL::Context* m_Context;
    OpenCL::CommandQueue* m_Queue;
    OpenCL::Kernel* m_RepulsionK;
    OpenCL::Kernel* m_AttractionK;
    OpenCL::Kernel* m_MovementK;
    OpenCL::Memory* m_InputNodeBuffer;
    OpenCL::Memory* m_InputEdgeBuffer;
    OpenCL::Memory* m_OutputNodeBuffer;
    OpenCL::Memory* m_ForceBuffer;

    bool m_Physics;
    float m_K;
    float m_Temperature;

    Clock m_Clock;
    unsigned int m_Iterations;
};
