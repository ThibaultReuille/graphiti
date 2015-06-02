#pragma once

#include <raindance/Core/Headers.hh>
#include <raindance/Core/Transformation.hh>
#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/FS.hh>

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
	Globe(Document::Node* parent = NULL)
	: Document::Node(parent)
	{       
		m_Earth = new Earth();
	}

	virtual ~Globe()
	{
		SAFE_DELETE(m_Earth);
	}

	void draw(Context* context) override
	{
		glClear(GL_DEPTH_BUFFER_BIT); // | GL_COLOR_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);

		Transformation transformation;

		m_Camera.setPerspectiveProjection(60.0f, (float)this->content().getWidth() /  (float)this->content().getHeight(), 0.1f, 1024.0f);

		float radius = 50;
		auto t = context->clock().seconds() / 2;

		auto pos = glm::vec3(radius * cos(t), 0, radius * sin(t));
        m_Camera.lookAt(pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        m_Earth->sun().setPosition(pos);
		
		m_Earth->draw(context, m_Camera, transformation);
	}

	void idle(Context* context) override
	{
		(void) context;
	}
private:
	Camera m_Camera;
	Earth* m_Earth;
};


