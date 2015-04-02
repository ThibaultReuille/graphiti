#include <raindance/Core/Headers.hh>

#include <raindance/Core/OpenCL.hh>

class GPUGraph 
{
public:

    struct ParticleForce 
    {
        glm::vec4 Direction;
    };

	struct NodeParticle
	{
		glm::vec4 Origin;
	};

	struct NodeInstance
	{
		typedef unsigned int ID;

        glm::vec4 Position;
        glm::vec4 Color;
        cl_float Size;

        ParticleForce Force;

        glm::vec3 __Unused; // NOTE: Memory alignment with OpenCL, reserved for later.
	};

    struct EdgeParticle
    {
        glm::vec4 Origin;
    };

	struct EdgeInstance
	{
		typedef unsigned int ID;

		glm::vec4 SourcePosition;
        glm::vec4 SourceColor;

        glm::vec4 TargetPosition;
        glm::vec4 TargetColor;

		cl_float Width;

        cl_uint SourceID;
        cl_uint TargetID;

        cl_float __Unused; // NOTE : Memory alignment with OpenCL, reserved for later.
	};

	GPUGraph()
	{
		m_NodeIcon = new Icon();
        m_NodeIcon->load("network/node/disk", FS::BinaryFile("Assets/NetworkView/node_disk.png"));

		FS::TextFile nodes_vert("Assets/NetworkView/nodes.vert");
		FS::TextFile nodes_geom("Assets/NetworkView/nodes.geom");
		FS::TextFile nodes_frag("Assets/NetworkView/nodes.frag");
        m_NodeShader = ResourceManager::getInstance().loadShader("NetworkView/nodes",
        	nodes_vert.content(),
        	nodes_frag.content(),
        	nodes_geom.content());
        // m_NodeShader->dump();


        m_EdgeIcon = new Icon();
        m_EdgeIcon->load("network/edge/solid", FS::BinaryFile("Assets/NetworkView/edge_solid.png"));

        FS::TextFile edges_vert("Assets/NetworkView/edges.vert");
		FS::TextFile edges_geom("Assets/NetworkView/edges.geom");
		FS::TextFile edges_frag("Assets/NetworkView/edges.frag");
        m_EdgeShader = ResourceManager::getInstance().loadShader("NetworkView/edges",
        	edges_vert.content(),
        	edges_frag.content(),
        	edges_geom.content());
        // m_EdgeShader->dump();

        m_NeedsUpdate = true;
        m_CL.RunPhysics = false;
	}

	virtual ~GPUGraph()
	{
		SAFE_DELETE(m_NodeIcon);
		ResourceManager::getInstance().unload(m_NodeShader);

        SAFE_DELETE(m_EdgeIcon);
        ResourceManager::getInstance().unload(m_EdgeShader);
	}

    void initialize(Context* context)
    {
        // OpenGL
        {
            // ----- Node Particle Geometry -----
            m_NodeParticleBuffer << glm::vec4(0, 0, 0, 0);
            m_NodeParticleBuffer.describe("a_Origin", 4, GL_FLOAT, sizeof(NodeParticle), 0);
            m_NodeParticleBuffer.generate(Buffer::STATIC);

            // ----- Edge Particle Geometry -----
            m_EdgeParticleBuffer << glm::vec4(0, 0, 0, 0);
            m_EdgeParticleBuffer.describe("a_Origin", 4, GL_FLOAT, sizeof(EdgeParticle), 0);
            m_EdgeParticleBuffer.generate(Buffer::STATIC);
        }

        // OpenCL
        {
            m_OpenCL.detect();
            //m_OpenCL.dump();

            // NOTE : We are assuming the last device is the best one.
            m_CL.Device = m_OpenCL.devices().back();

            m_OpenCL.dump(*m_CL.Device);

            m_CL.Context = m_OpenCL.createContext(*m_CL.Device);
            m_CL.Queue = m_OpenCL.createCommandQueue(*m_CL.Context);

            auto source = FS::TextFile("./Assets/NetworkView/physics.cl");
         
            m_CL.Program = m_OpenCL.loadProgram(*m_CL.Context, "physics", source.content());

            m_CL.RepulsionK = m_OpenCL.createKernel(*m_CL.Program, "repulsion");
            m_CL.AttractionK = m_OpenCL.createKernel(*m_CL.Program, "attraction");
            m_CL.NodeAnimationK = m_OpenCL.createKernel(*m_CL.Program, "node_animation");
            m_CL.EdgeAnimationK = m_OpenCL.createKernel(*m_CL.Program, "edge_animation");

            clFinish(m_CL.Queue->Object);
        }
    }

    void updateOpenGL(Context* context)
    {
        // LOG("updateOpenGL: %u nodes, %u edges.\n", m_NodeBuffer.size() / sizeof(Node), m_EdgeBuffer.size() / sizeof(Edge));

        if (m_NodeInstanceBuffer.size() == 0) // TODO: HACK: OpenGL doesn't like VBO resizing, Find a way to handle it properly.
            return;

        // --- Define Node Instances

        static bool m_NodeInstanceBufferFirstUpdate = true;

        if (!m_NodeInstanceBufferFirstUpdate)
            m_NodeInstanceBuffer.update();

        m_NodeInstanceBuffer.describe("a_Position", 4, GL_FLOAT, sizeof(NodeInstance), 0);
        m_NodeInstanceBuffer.describe("a_Color",    4, GL_FLOAT, sizeof(NodeInstance), 1 * sizeof(glm::vec4));
        m_NodeInstanceBuffer.describe("a_Size",     1, GL_FLOAT, sizeof(NodeInstance), 2 * sizeof(glm::vec4));
 
        if (m_NodeInstanceBufferFirstUpdate)
            m_NodeInstanceBuffer.generate(Buffer::DYNAMIC);

        m_NodeInstanceBufferFirstUpdate = false;

        // --- Define Edge Instances

        static bool m_EdgeInstanceBufferFirstUpdate = true;
       
        if (!m_EdgeInstanceBufferFirstUpdate)
            m_EdgeInstanceBuffer.update();

        m_EdgeInstanceBuffer.describe("a_SourcePosition", 4, GL_FLOAT, sizeof(EdgeInstance), 0);
        m_EdgeInstanceBuffer.describe("a_SourceColor",    4, GL_FLOAT, sizeof(EdgeInstance), 1 * sizeof(glm::vec4));
        m_EdgeInstanceBuffer.describe("a_TargetPosition", 4, GL_FLOAT, sizeof(EdgeInstance), 2 * sizeof(glm::vec4));
        m_EdgeInstanceBuffer.describe("a_TargetColor",    4, GL_FLOAT, sizeof(EdgeInstance), 3 * sizeof(glm::vec4));
        m_EdgeInstanceBuffer.describe("a_Width",          1, GL_FLOAT, sizeof(EdgeInstance), 4 * sizeof(glm::vec4));
        
        if (m_EdgeInstanceBufferFirstUpdate)
            m_EdgeInstanceBuffer.generate(Buffer::DYNAMIC);
        
        m_EdgeInstanceBufferFirstUpdate = false;   
    }

    void updateOpenCL(Context* context)
    {
        size_t node_count = m_NodeInstanceBuffer.size() / sizeof(NodeInstance);
        size_t edge_count = m_EdgeInstanceBuffer.size() / sizeof(EdgeInstance);

        if (node_count == 0 || !m_CL.RunPhysics) // TODO: HACK: OpenGL doesn't like VBO resizing, Find a way to handle it properly.
            return;

        static bool m_OCLNeedsUpdate = true;
        if (m_OCLNeedsUpdate)
        {
            m_CL.NodeInstanceBuffer = m_OpenCL.createFromGLBuffer(*m_CL.Context, CL_MEM_READ_WRITE, m_NodeInstanceBuffer.vbo());
            m_CL.EdgeInstanceBuffer = m_OpenCL.createFromGLBuffer(*m_CL.Context, CL_MEM_READ_WRITE, m_EdgeInstanceBuffer.vbo());
            clFinish(m_CL.Queue->Object);
            m_OCLNeedsUpdate = false;
        }

        // ----- Parameters -----

        static int Iterations = 0;
        if (Iterations >= 100)
            return;

        const float cube_side = 100; // NOTE : Graph should fit in this cube
        const float volume = cube_side * cube_side * cube_side; 

        m_CL.K = pow(volume / node_count, 1.0 / 3.0);

        m_CL.Time = context->clock().seconds();

        m_CL.Temperature = cube_side / (Iterations + 1);

        LOG("Iteration: %i, K: %f, Time: %f, Temperature: %f\n", Iterations, m_CL.K, m_CL.Time, m_CL.Temperature);

        m_OpenCL.enqueueAcquireGLObjects(*m_CL.Queue, 1, &m_CL.NodeInstanceBuffer->Object, 0, 0, NULL);
        m_OpenCL.enqueueAcquireGLObjects(*m_CL.Queue, 1, &m_CL.EdgeInstanceBuffer->Object, 0, 0, NULL);
        {
            // ----- Node Repulsion -----
            m_CL.RepulsionK->setArgument(0, *m_CL.NodeInstanceBuffer);
            m_CL.RepulsionK->setArgument(1, &m_CL.K, sizeof(float));
            m_OpenCL.enqueueNDRangeKernel(*m_CL.Queue, *m_CL.RepulsionK, 1, NULL, &node_count, NULL, 0, NULL, NULL);

            // ----- Edge Attraction -----

            m_CL.AttractionK->setArgument(0, *m_CL.NodeInstanceBuffer);
            m_CL.AttractionK->setArgument(1, *m_CL.EdgeInstanceBuffer);
            m_CL.AttractionK->setArgument(2, &m_CL.K, sizeof(float));
            m_OpenCL.enqueueNDRangeKernel(*m_CL.Queue, *m_CL.AttractionK, 1, NULL, &edge_count, NULL, 0, NULL, NULL);

            // ----- Node Animation -----

            m_CL.NodeAnimationK->setArgument(0, *m_CL.NodeInstanceBuffer);
            m_CL.NodeAnimationK->setArgument(1, &m_CL.Temperature, sizeof(float));
            m_OpenCL.enqueueNDRangeKernel(*m_CL.Queue, *m_CL.NodeAnimationK, 1, NULL, &node_count, NULL, 0, NULL, NULL);

            // ----- Edge Animation -----

            m_CL.EdgeAnimationK->setArgument(0, *m_CL.NodeInstanceBuffer);
            m_CL.EdgeAnimationK->setArgument(1, *m_CL.EdgeInstanceBuffer);  
            m_OpenCL.enqueueNDRangeKernel(*m_CL.Queue, *m_CL.EdgeAnimationK, 1, NULL, &edge_count, NULL, 0, NULL, NULL);
        }
        m_OpenCL.enqueueReleaseGLObjects(*m_CL.Queue, 1, &m_CL.EdgeInstanceBuffer->Object, 0, 0, NULL);
        m_OpenCL.enqueueReleaseGLObjects(*m_CL.Queue, 1, &m_CL.NodeInstanceBuffer->Object, 0, 0, NULL);

        clFinish(m_CL.Queue->Object);

        Iterations++;

        // ----- Copy back to CPU Data -----

        if (Iterations == 100)
        {
            m_OpenCL.enqueueAcquireGLObjects(*m_CL.Queue, 1, &m_CL.NodeInstanceBuffer->Object, 0, 0, NULL);
            m_OpenCL.enqueueAcquireGLObjects(*m_CL.Queue, 1, &m_CL.EdgeInstanceBuffer->Object, 0, 0, NULL);
            {
                m_OpenCL.enqueueReadBuffer(*m_CL.Queue, *m_CL.NodeInstanceBuffer, CL_TRUE, 0, m_NodeInstanceBuffer.size(), m_NodeInstanceBuffer.ptr(), 0, NULL, NULL);
                m_OpenCL.enqueueReadBuffer(*m_CL.Queue, *m_CL.EdgeInstanceBuffer, CL_TRUE, 0, m_EdgeInstanceBuffer.size(), m_EdgeInstanceBuffer.ptr(), 0, NULL, NULL);
            }
            m_OpenCL.enqueueReleaseGLObjects(*m_CL.Queue, 1, &m_CL.EdgeInstanceBuffer->Object, 0, 0, NULL);
            m_OpenCL.enqueueReleaseGLObjects(*m_CL.Queue, 1, &m_CL.NodeInstanceBuffer->Object, 0, 0, NULL);
            
            clFinish(m_CL.Queue->Object);
        }
    }

	void idle(Context* context)
	{
        if (m_NeedsUpdate)
        {
            updateOpenGL(context);
            m_NeedsUpdate = false; 
        }

        updateOpenCL(context);        
	}

	virtual void drawEdges(Context* context, Camera& camera, Transformation& transformation)
	{
        if (!m_EdgeInstanceBuffer.isGenerated() || m_EdgeInstanceBuffer.size() == 0)
            return;

        m_EdgeShader->use();
        //m_EdgeShader->uniform("u_Texture").set(m_NodeIcon->getTexture(0));

        m_EdgeShader->uniform("u_ModelViewMatrix").set(camera.getViewMatrix() * transformation.state());
        m_EdgeShader->uniform("u_ProjectionMatrix").set(camera.getProjectionMatrix());

        context->geometry().bind(m_EdgeParticleBuffer, *m_EdgeShader);        
        context->geometry().bind(m_EdgeInstanceBuffer, *m_EdgeShader);
        
        glVertexAttribDivisorARB(m_EdgeShader->attribute("a_Origin").location(), 0); // Same vertices per instance
        glVertexAttribDivisorARB(m_EdgeShader->attribute("a_SourcePosition").location(), 1);
        glVertexAttribDivisorARB(m_EdgeShader->attribute("a_SourceColor").location(), 1);
        glVertexAttribDivisorARB(m_EdgeShader->attribute("a_TargetPosition").location(), 1);
        glVertexAttribDivisorARB(m_EdgeShader->attribute("a_TargetColor").location(), 1);
        glVertexAttribDivisorARB(m_EdgeShader->attribute("a_Width").location(), 1);

        context->geometry().drawArraysInstanced(GL_POINTS, 0, m_EdgeParticleBuffer.size() / sizeof(EdgeParticle), m_EdgeInstanceBuffer.size() / sizeof(EdgeInstance));
        
        glVertexAttribDivisorARB(m_EdgeShader->attribute("a_Origin").location(), 0);
        glVertexAttribDivisorARB(m_EdgeShader->attribute("a_SourcePosition").location(), 0);
        glVertexAttribDivisorARB(m_EdgeShader->attribute("a_SourceColor").location(), 0);
        glVertexAttribDivisorARB(m_EdgeShader->attribute("a_TargetPosition").location(), 0);
        glVertexAttribDivisorARB(m_EdgeShader->attribute("a_TargetColor").location(), 0);
        glVertexAttribDivisorARB(m_EdgeShader->attribute("a_Width").location(), 0);

        context->geometry().unbind(m_EdgeParticleBuffer);
        context->geometry().unbind(m_EdgeInstanceBuffer);
	}

    virtual void drawNodes(Context* context, Camera& camera, Transformation& transformation)
    {       
        if (!m_NodeInstanceBuffer.isGenerated() || m_NodeInstanceBuffer.size() == 0)
            return;

        m_NodeShader->use();
        m_NodeShader->uniform("u_Texture").set(m_NodeIcon->getTexture(0));

        m_NodeShader->uniform("u_ModelViewMatrix").set(camera.getViewMatrix() * transformation.state());
        m_NodeShader->uniform("u_ProjectionMatrix").set(camera.getProjectionMatrix());

        context->geometry().bind(m_NodeParticleBuffer, *m_NodeShader);        
        context->geometry().bind(m_NodeInstanceBuffer, *m_NodeShader);
        
        glVertexAttribDivisorARB(m_NodeShader->attribute("a_Origin").location(), 0); // Same vertices per instance
        glVertexAttribDivisorARB(m_NodeShader->attribute("a_Position").location(), 1);
        glVertexAttribDivisorARB(m_NodeShader->attribute("a_Color").location(), 1);
        glVertexAttribDivisorARB(m_NodeShader->attribute("a_Size").location(), 1);

        context->geometry().drawArraysInstanced(GL_POINTS, 0, m_NodeParticleBuffer.size() / sizeof(NodeParticle), m_NodeInstanceBuffer.size() / sizeof(NodeInstance));
        
        glVertexAttribDivisorARB(m_NodeShader->attribute("a_Origin").location(), 0);
        glVertexAttribDivisorARB(m_NodeShader->attribute("a_Position").location(), 0);
        glVertexAttribDivisorARB(m_NodeShader->attribute("a_Color").location(), 0);
        glVertexAttribDivisorARB(m_NodeShader->attribute("a_Size").location(), 0);

        context->geometry().unbind(m_NodeParticleBuffer);
        context->geometry().unbind(m_NodeInstanceBuffer);
    }

    virtual void draw(Context* context, Camera& camera, Transformation& transformation)
    {
        drawEdges(context, camera, transformation);
        drawNodes(context, camera, transformation);
    }

    void computeBoundingBox(glm::vec3& min, glm::vec3& max)
    {
        NodeInstance node;
        for (unsigned int i = 0; i < m_NodeInstanceBuffer.size() / sizeof(NodeInstance); i++)
        {
            m_NodeInstanceBuffer.get(i, &node, sizeof(NodeInstance));
            if (i == 0)
            {
                min = node.Position.xyz();
                max = node.Position.xyz();
            }
            else
            {
                if (node.Position.x < min.x) min.x = node.Position.x;
                if (node.Position.y < min.y) min.y = node.Position.y;
                if (node.Position.z < min.z) min.z = node.Position.z;

                if (node.Position.x > max.x) max.x = node.Position.x;
                if (node.Position.y > max.y) max.y = node.Position.y;
                if (node.Position.z > max.z) max.z = node.Position.z;
            }
        }
    }

    // ----- Nodes Accessors -----

    inline Buffer& getNodes() { return m_NodeInstanceBuffer; }
    
    void addNode(const NodeInstance& node)
    {
        m_NodeInstanceBuffer.push(&node, sizeof(NodeInstance));
        m_NeedsUpdate = true;
    }

    inline NodeInstance getNode(NodeInstance::ID id)
    {
        NodeInstance node;
        m_NodeInstanceBuffer.get(id, &node, sizeof(NodeInstance));
        return node;
    }

    inline void setNode(NodeInstance::ID id, const NodeInstance& node)
    {
        m_NodeInstanceBuffer.set(id, &node, sizeof(NodeInstance));
        m_NeedsUpdate = true;
    }

    // ----- Edges Accessors -----

    inline Buffer& getEdges() { return m_EdgeInstanceBuffer; }

    void addEdge(const EdgeInstance& edge)
    {
        m_EdgeInstanceBuffer.push(&edge, sizeof(EdgeInstance));
        m_NeedsUpdate = true;
    }

    inline EdgeInstance getEdge(EdgeInstance::ID id)
    {
        EdgeInstance edge;
        m_EdgeInstanceBuffer.get(id, &edge, sizeof(EdgeInstance));
        return edge;
    }

    inline void setEdge(EdgeInstance::ID id, const EdgeInstance& edge)
    {
        m_EdgeInstanceBuffer.set(id, &edge, sizeof(EdgeInstance));
        m_NeedsUpdate = true;
    }

    // ----- Parameters ------

    inline void setPhysics(bool flag) { m_CL.RunPhysics = flag; }
    inline bool getPhysics() { return m_CL.RunPhysics; }

private:
	Icon* m_NodeIcon;
    Icon* m_EdgeIcon;
    bool m_NeedsUpdate;

	Shader::Program* m_NodeShader;
    Buffer m_NodeParticleBuffer;
	Buffer m_NodeInstanceBuffer;

	Shader::Program* m_EdgeShader;
    Buffer m_EdgeParticleBuffer;
    Buffer m_EdgeInstanceBuffer;

    struct OCLData
    {
        OCLData() {}

        bool RunPhysics;

        OpenCL::Device* Device;
        OpenCL::Context* Context;
        OpenCL::CommandQueue* Queue;
        OpenCL::Program* Program;

        OpenCL::Kernel* RepulsionK;
        OpenCL::Kernel* AttractionK;
        OpenCL::Kernel* NodeAnimationK;
        OpenCL::Kernel* EdgeAnimationK;

        OpenCL::Memory* NodeInstanceBuffer;
        OpenCL::Memory* EdgeInstanceBuffer;

        float Time;
        float K;
        float Temperature;
    };

    OpenCL m_OpenCL;
    OCLData m_CL;
};
