#pragma once

#include <raindance/Core/Headers.hh>
#include <raindance/Core/Transformation.hh>
#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/FS.hh>
#include <raindance/Core/Batch.hh>

class Earth
{
public:

	Earth()
	{
		float radius = 20.0f;

		m_SphereTexture = ResourceManager::getInstance().loadTexture("earth", Assets_Earth_world_2048x1024_jpg, sizeof(Assets_Earth_world_2048x1024_jpg));
		m_SphereMesh = new SphereMesh(radius, 100, 100);
		
		FS::TextFile globe_vert("Assets/Earth/globe.vert");
		FS::TextFile globe_frag("Assets/Earth/globe.frag");

		m_SphereShader = ResourceManager::getInstance().loadShader("Earth/globe", globe_vert.content(), globe_frag.content());
		//m_SphereShader->dump();

		m_SphereMaterial.setDiffuse(glm::vec4(1.0, 1.0, 1.0, 1.0));
		m_SphereMaterial.setShininess(20.0f);

		m_Sun.setPosition(glm::vec3(100.0, 100.0, 100.0));
	}

	virtual ~Earth()
	{
		ResourceManager::getInstance().unload(m_SphereTexture);
		ResourceManager::getInstance().unload(m_SphereShader);
		delete m_SphereMesh;
	}

	void draw(Context* context, const Camera& camera, Transformation& transformation)
	{
		transformation.push();

		transformation.rotate(-180.0f, glm::vec3(0, 1, 0));
		transformation.rotate(180.0f, glm::vec3(1, 0, 0));

		m_SphereShader->use();

		m_SphereShader->uniform("u_ModelMatrix").set(transformation.state());
		m_SphereShader->uniform("u_ViewMatrix").set(camera.getViewMatrix());
		m_SphereShader->uniform("u_ProjectionMatrix").set(camera.getProjectionMatrix());

		// NOTE : Normal Matrix is the transpose of the inverse of the model view matrix
		// Using the Model View Matrix doesn't work if using non-homogeneous scales
		m_SphereShader->uniform("u_NormalMatrix").set(glm::transpose(glm::inverse(glm::mat3(camera.getViewMatrix() * transformation.state()))));

		m_SphereShader->uniform("u_Texture").set(*m_SphereTexture);

		m_SphereShader->uniform("u_Light.Type").set(m_Sun.getType());
		m_SphereShader->uniform("u_Light.Position").set(m_Sun.getPosition());
		m_SphereShader->uniform("u_Light.Direction").set(m_Sun.getDirection());
		m_SphereShader->uniform("u_Light.Color").set(m_Sun.getColor());

		m_SphereShader->uniform("u_Material.Ambient").set(m_SphereMaterial.getAmbient());
		m_SphereShader->uniform("u_Material.Diffuse").set(m_SphereMaterial.getDiffuse());
		m_SphereShader->uniform("u_Material.Specular").set(m_SphereMaterial.getSpecular());
		m_SphereShader->uniform("u_Material.Shininess").set(m_SphereMaterial.getShininess());

		context->geometry().bind(m_SphereMesh->getVertexBuffer(), *m_SphereShader);
		context->geometry().drawArrays(GL_TRIANGLES, 0, m_SphereMesh->getVertexBuffer().size() / sizeof(SphereMesh::Vertex));
		context->geometry().unbind(m_SphereMesh->getVertexBuffer());

		transformation.pop();
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

	Light& sun() { return m_Sun; }

private:
	SphereMesh* m_SphereMesh;
	Texture* m_SphereTexture;
	Shader::Program* m_SphereShader;
	Material m_SphereMaterial;
	Light m_Sun;
};

class Globe : public Document::Node
{
public:

	struct Node
	{
		glm::vec2 Location;
		glm::vec4 Color;
		float Size;
	};

	Globe(Document::Node* parent = NULL)
	: Document::Node(parent)
	{       
		m_Earth = new Earth();

		FS::TextFile vert("Assets/Globe/node.vert");
    	FS::TextFile geom("Assets/Globe/node.geom");
    	FS::TextFile frag("Assets/Globe/node.frag");
    	m_Shader = ResourceManager::getInstance().loadShader("Globe/node",
    		vert.content(),
    		frag.content(),
    		geom.content());
    	m_Shader->dump();

		Node node;
		node.Size = 2.5;

		node.Color = glm::vec4(SKY_BLUE, 0.75);
		node.Location = glm::vec2(37.783333, -122.416667);
		m_VertexBuffer.push(&node, sizeof(Node));

		node.Color = glm::vec4(LOVE_RED, 0.75);
		node.Location = glm::vec2(48.8567, 2.3508);
		m_VertexBuffer.push(&node, sizeof(Node));

		node.Color = glm::vec4(YELLOW, 0.75);
		node.Location = glm::vec2(-33.865, 151.209444);
		m_VertexBuffer.push(&node, sizeof(Node));

    	m_VertexBuffer.describe("a_Location", GL_FLOAT, 2);
        m_VertexBuffer.describe("a_Color",    GL_FLOAT, 4);
        m_VertexBuffer.describe("a_Size",     GL_FLOAT, 1);
    	m_VertexBuffer.generate(Buffer::DYNAMIC);

		auto drawcall = new DrawArrays();
		drawcall->shader(m_Shader);
		drawcall->add(&m_VertexBuffer);
		m_Batch.add(drawcall);
	}

	virtual ~Globe()
	{
		SAFE_DELETE(m_Earth);
	}

	void draw(Context* context) override
	{
		glClear(GL_DEPTH_BUFFER_BIT); // | GL_COLOR_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		Transformation transformation;

		m_Camera.setPerspectiveProjection(60.0f, (float)this->content().getWidth() /  (float)this->content().getHeight(), 0.1f, 1024.0f);

		float radius = 50;
		auto t = context->clock().seconds() / 2;

		auto pos = glm::vec3(radius * cos(t), 5, radius * sin(t));
        m_Camera.lookAt(pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        m_Earth->sun().setPosition(pos);
		
		m_Earth->draw(context, m_Camera, transformation);

		m_Shader->use();
		m_Shader->uniform("u_Radius").set(20.0f);
		m_Shader->uniform("u_ProjectionMatrix").set(m_Camera.getProjectionMatrix());
		m_Shader->uniform("u_ViewMatrix").set(m_Camera.getViewMatrix());
		//m_Shader->uniform("u_ModelMatrix").set(transformation.state());
		m_Batch.execute(context);
	}

	void idle(Context* context) override
	{
		(void) context;
	}

	void request(const Variables& input, Variables& output) override
	{
		//LOG("[GLOBE] Request received!\n");
		//input.dump();

		std::string function;
		if (!input.get("function", function))
			return;

		bool error = false;

		if (function == "add")
		{
			float latitude = 0;
			float longitude = 0;
			float size = 5.0;
			glm::vec4 color = glm::vec4(WHITE, 0.75);

			error &= input.get("latitude", &latitude);
			error &= input.get("longitude", &longitude);
			error &= input.get("color.r", &color.r);
			error &= input.get("color.g", &color.g);
			error &= input.get("color.b", &color.b);
			error &= input.get("color.a", &color.a);
			error &= input.get("size", &size);

			Node node;
			node.Size = size;
			node.Color = color;
			node.Location = glm::vec2(latitude, longitude);
			m_VertexBuffer.push(&node, sizeof(Node));
			m_VertexBuffer.update();
		}

		auto var = new IntVariable();
		var->set(error ? -1 : 1);
		output.set("error", var);
	}

private:
	Camera m_Camera;
	Earth* m_Earth;
	Batch m_Batch;
	Shader::Program* m_Shader;
	Buffer m_VertexBuffer;
};


