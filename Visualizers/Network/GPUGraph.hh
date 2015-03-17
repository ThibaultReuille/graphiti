#include <raindance/Core/Headers.hh>

#include <raindance/Core/OpenCL.hh>

class GPUGraph 
{
public:

	struct NodeParticle
	{
		glm::vec4 Origin;
	};

	struct Node
	{
		typedef unsigned int ID;

        glm::vec4 Position;
        glm::vec4 Color;
        GLfloat Size;

        glm::vec3 __Unused; // NOTE: Alignment with OpenCL, reserved for later.
	};

    struct EdgeParticle
    {
        glm::vec4 Origin;
    };

	struct Edge
	{
		typedef unsigned int ID;

		glm::vec4 SourcePosition;
        glm::vec4 SourceColor;

        glm::vec4 TargetPosition;
        glm::vec4 TargetColor;

		GLfloat Width;

        glm::vec3 __Unused; // NOTE : Alignment with OpenCL, reserved for later.
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
        // m_EdgeShader->dump();

        m_NeedsUpdate = true;
	}

	virtual ~GPUGraph()
	{
		SAFE_DELETE(m_Icon);
		ResourceManager::getInstance().unload(m_NodeShader);
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
            m_CL.Test1K = m_OpenCL.createKernel(*m_CL.Program, "node_animation");
            m_CL.Test2K = m_OpenCL.createKernel(*m_CL.Program, "edge_animation");
        
            clFinish(m_CL.Queue->Object);
        }
    }

    void updateOpenGL(Context* context)
    {
        // LOG("updateOpenGL: %u nodes, %u edges.\n", m_NodeBuffer.size() / sizeof(Node), m_EdgeBuffer.size() / sizeof(Edge));

        if (m_NodeBuffer.size() == 0) // TODO: HACK: OpenGL doesn't like VBO resizing, Find a way to handle it properly.
            return;

        // --- Define Node Instances

        static bool m_NodeBufferFirstUpdate = true;

        if (!m_NodeBufferFirstUpdate)
            m_NodeBuffer.update();

        m_NodeBuffer.describe("a_Position", 4, GL_FLOAT, sizeof(Node), 0);
        m_NodeBuffer.describe("a_Color",    4, GL_FLOAT, sizeof(Node), 1 * sizeof(glm::vec4));
        m_NodeBuffer.describe("a_Size",     1, GL_FLOAT, sizeof(Node), 2 * sizeof(glm::vec4));
 
        if (m_NodeBufferFirstUpdate)
            m_NodeBuffer.generate(Buffer::DYNAMIC);

        m_NodeBufferFirstUpdate = false;

        // --- Define Edge Instances

        static bool m_EdgeBufferFirstUpdate = true;
       
        if (!m_EdgeBufferFirstUpdate)
            m_EdgeBuffer.update();

        m_EdgeBuffer.describe("a_SourcePosition", 4, GL_FLOAT, sizeof(Edge), 0);
        m_EdgeBuffer.describe("a_SourceColor",    4, GL_FLOAT, sizeof(Edge), 1 * sizeof(glm::vec4));
        m_EdgeBuffer.describe("a_TargetPosition", 4, GL_FLOAT, sizeof(Edge), 2 * sizeof(glm::vec4));
        m_EdgeBuffer.describe("a_TargetColor",    4, GL_FLOAT, sizeof(Edge), 3 * sizeof(glm::vec4));
        m_EdgeBuffer.describe("a_Width",          1, GL_FLOAT, sizeof(Edge), 4 * sizeof(glm::vec4));
        
        if (m_EdgeBufferFirstUpdate)
            m_EdgeBuffer.generate(Buffer::DYNAMIC);
        
        m_EdgeBufferFirstUpdate = false;   
    }

    void updateOpenCL(Context* context)
    {
        size_t node_count = m_NodeBuffer.size() / sizeof(Node);
        size_t edge_count = m_EdgeBuffer.size() / sizeof(Edge);

        if (node_count == 0) // TODO: HACK: OpenGL doesn't like VBO resizing, Find a way to handle it properly.
            return;

        static bool m_OCLNeedsUpdate = true;
        if (m_OCLNeedsUpdate)
        {
            m_CL.NodeInstanceBuffer = m_OpenCL.createFromGLBuffer(*m_CL.Context, CL_MEM_READ_WRITE, m_NodeBuffer.vbo());
            m_CL.EdgeInstanceBuffer = m_OpenCL.createFromGLBuffer(*m_CL.Context, CL_MEM_READ_WRITE, m_EdgeBuffer.vbo());
            clFinish(m_CL.Queue->Object);
            m_OCLNeedsUpdate = false;
        }

        m_CL.Time = context->clock().seconds();

        // ----- Node Animation -----

        m_OpenCL.enqueueAcquireGLObjects(*m_CL.Queue, 1, &m_CL.NodeInstanceBuffer->Object, 0, 0, NULL);
        {
            m_CL.Time = context->clock().seconds();

            m_CL.Test1K->setArgument(0, *m_CL.NodeInstanceBuffer);
            m_CL.Test1K->setArgument(1, &m_CL.Time, sizeof(float));

            m_OpenCL.enqueueNDRangeKernel(*m_CL.Queue, *m_CL.Test1K, 1, NULL, &node_count, NULL, 0, NULL, NULL);
        }
        m_OpenCL.enqueueReleaseGLObjects(*m_CL.Queue, 1, &m_CL.NodeInstanceBuffer->Object, 0, 0, NULL);

        // ----- Edge Animation -----
        
        m_OpenCL.enqueueAcquireGLObjects(*m_CL.Queue, 1, &m_CL.EdgeInstanceBuffer->Object, 0, 0, NULL);
        {
            m_CL.Time = context->clock().seconds();

            m_CL.Test2K->setArgument(0, *m_CL.EdgeInstanceBuffer);
            m_CL.Test2K->setArgument(1, &m_CL.Time, sizeof(float));

            m_OpenCL.enqueueNDRangeKernel(*m_CL.Queue, *m_CL.Test2K, 1, NULL, &edge_count, NULL, 0, NULL, NULL);
        }
        m_OpenCL.enqueueReleaseGLObjects(*m_CL.Queue, 1, &m_CL.EdgeInstanceBuffer->Object, 0, 0, NULL);
        

        clFinish(m_CL.Queue->Object);
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
        glVertexAttribDivisorARB(m_NodeShader->attribute("a_Color").location(), 1);
        glVertexAttribDivisorARB(m_NodeShader->attribute("a_Size").location(), 1);
        
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

    // ----- Nodes Accessors -----

    inline Buffer& getNodes() { return m_NodeBuffer; }
    
    void addNode(const Node& node)
    {
        m_NodeBuffer.push(&node, sizeof(Node));
        m_NeedsUpdate = true;
    }

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

    // ----- Edges Accessors -----

    inline Buffer& getEdges() { return m_EdgeBuffer; }

    void addEdge(const Edge& edge)
    {
        m_EdgeBuffer.push(&edge, sizeof(Edge));
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

    struct OCLData
    {
        OCLData() {}

        OpenCL::Device* Device;
        OpenCL::Context* Context;
        OpenCL::CommandQueue* Queue;
        OpenCL::Program* Program;
        OpenCL::Kernel* Test1K;
        OpenCL::Kernel* Test2K;

        OpenCL::Memory* NodeInstanceBuffer;
        OpenCL::Memory* EdgeInstanceBuffer;
        float Time;
    };

    OpenCL m_OpenCL;
    OCLData m_CL;
};
