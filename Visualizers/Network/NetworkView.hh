#pragma once

#include <raindance/Core/Headers.hh>
#include <raindance/Core/Clock.hh>
#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/Primitives/Quad.hh>
#include <raindance/Core/Resources/Texture.hh>

#include "Entities/MVC.hh"

class ParticleSystem 
{
public:

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec2 UV;
	};

	struct Instance
	{
        glm::vec3 Translation;
        glm::vec3 Scale;
        glm::vec4 Color;
	};

	ParticleSystem()
	{
		// --- Load Resources ---

		m_Icon = new Icon();
        m_Icon->load("network/shapes/disk", FS::BinaryFile("Assets/NetworkView/shape_disk.png"));

		FS::TextFile geom("Assets/NetworkView/particles.geom");
		FS::TextFile vert("Assets/NetworkView/particles.vert");
		FS::TextFile frag("Assets/NetworkView/particles.frag");
        m_Shader = ResourceManager::getInstance().loadShader("NetworkView/particles", vert.content(), frag.content(), geom.content());
        m_Shader->dump();

        // --- Define Particle Geometry ---

        m_VertexBuffer << glm::vec3(-0.5, -0.5, 0.0) << glm::vec2(0, 1);
        m_VertexBuffer << glm::vec3( 0.5, -0.5, 0.0) << glm::vec2(1, 1);
        m_VertexBuffer << glm::vec3(-0.5,  0.5, 0.0) << glm::vec2(0, 0);
        m_VertexBuffer << glm::vec3( 0.5,  0.5, 0.0) << glm::vec2(1, 0);
        m_VertexBuffer.describe("a_Position", 3, GL_FLOAT, sizeof(Vertex), 0);
        m_VertexBuffer.describe("a_UV", 2, GL_FLOAT, sizeof(Vertex), sizeof(glm::vec3)); 
        m_VertexBuffer.generate(Buffer::STATIC);

        // --- Define Particle Instances ---

        int count = 1000000;

        for (int n = 0; n < count; n++)
        {
            float d = 50;

            Instance i;

            i.Translation = d * glm::vec3(
                RANDOM_FLOAT(-1.0, 1.0),
                RANDOM_FLOAT(-1.0, 1.0),
                RANDOM_FLOAT(-1.0, 1.0));
            
            float s = RANDOM_FLOAT(0.5, 2.0);
            i.Scale = glm::vec3(s, s, 1.0);
            
            i.Color = glm::vec4(
                RANDOM_FLOAT(0.0, 1.0),
                RANDOM_FLOAT(0.0, 1.0),
                RANDOM_FLOAT(0.0, 1.0),
                1.0);

            m_InstanceBuffer.push(&i, sizeof(Instance));
        }

        m_InstanceBuffer.describe("a_Translation", 3, GL_FLOAT, sizeof(Instance), 0);
        m_InstanceBuffer.describe("a_Scale", 3, GL_FLOAT, sizeof(Instance), sizeof(glm::vec3));
        m_InstanceBuffer.describe("a_Color", 4, GL_FLOAT, sizeof(Instance), 2 * sizeof(glm::vec3));

        m_InstanceBuffer.generate(Buffer::DYNAMIC);
	}

	virtual ~ParticleSystem()
	{
		SAFE_DELETE(m_Icon);
		ResourceManager::getInstance().unload(m_Shader);
	}

    virtual void draw(Context* context, Camera& camera, Transformation& transformation)
    {
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
        
        m_Shader->use();
        m_Shader->uniform("u_Texture").set(m_Icon->getTexture(0));

        m_Shader->uniform("u_ModelViewMatrix").set(camera.getViewMatrix() * transformation.state());
        m_Shader->uniform("u_ProjectionMatrix").set(camera.getProjectionMatrix());

        context->geometry().bind(m_VertexBuffer, *m_Shader);        
        context->geometry().bind(m_InstanceBuffer, *m_Shader);
        
        glVertexAttribDivisorARB(m_Shader->attribute("a_Position").location(), 0); // Same vertices per instance

        glVertexAttribDivisorARB(m_Shader->attribute("a_Translation").location(), 1);
        glVertexAttribDivisorARB(m_Shader->attribute("a_Scale").location(), 1);
        glVertexAttribDivisorARB(m_Shader->attribute("a_Color").location(), 1);

        context->geometry().drawArraysInstanced(GL_TRIANGLE_STRIP, 0, m_VertexBuffer.size() / sizeof(Vertex), m_InstanceBuffer.size() / sizeof(Instance));
        
        context->geometry().unbind(m_VertexBuffer);
        context->geometry().unbind(m_InstanceBuffer);
    }

private:
	Shader::Program* m_Shader;
	Icon* m_Icon;

	Buffer m_VertexBuffer;
	Buffer m_InstanceBuffer;
};


class NetworkView : public GraphView
{
public:

	NetworkView()
	{
		LOG("[NETWORK] Creating network view ...\n");

		m_GraphEntity = NULL;

		m_Font = new Font();

		m_ParticleSystem = new ParticleSystem();
	}

	virtual ~NetworkView()
	{
		SAFE_DELETE(m_Font);

		SAFE_DELETE(m_ParticleSystem);
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

		m_ParticleSystem->draw(context(), m_Camera3D, transformation);
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

	ParticleSystem* m_ParticleSystem;
};

