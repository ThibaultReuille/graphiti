#pragma once

#include <raindance/Core/Headers.hh>
#include <raindance/Core/Clock.hh>
#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/Mesh.hh>
#include <raindance/Core/Primitives/Quad.hh>
#include <raindance/Core/PointCloud.hh>
#include <raindance/Core/MarchingCubes.hh>
#include <raindance/Core/Resources/Texture.hh>

#include "Entities/MVC.hh"

class CloudView : public GraphView
{
public:
	struct GeoPoint
	{
		float Longitude;
		float Latitude;
		glm::vec3 PositionMap;
		glm::vec3 PositionEarth;
		glm::vec3 Color;
	};

	CloudView()
	{
		LOG("[CLOUDVIEW] Creating cloud view ...\n");

		m_WindowWidth = glutGet(GLUT_WINDOW_WIDTH);
		m_WindowHeight = glutGet(GLUT_WINDOW_HEIGHT);

		m_Camera2D.setOrthographicProjection(0.0f, (float)m_WindowWidth, 0.0f, (float)m_WindowHeight, 0.001f, 100.f);
		m_Camera2D.lookAt(glm::vec3(0, 0, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_Camera3D.setPerspectiveProjection(60.0f, m_WindowWidth / (float)m_WindowHeight, 0.1f, 1024.0f);
		m_Camera3D.lookAt(glm::vec3(0, 0, 30), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_GraphEntity = NULL;

		m_PointCloud = new PointCloud();

		m_Font = new Font();
		m_AxisLabels[0].set("x", m_Font);
		m_AxisLabels[0].setColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
		m_AxisLabels[1].set("y", m_Font);
		m_AxisLabels[1].setColor(glm::vec4(0.0, 1.0, 0.0, 1.0));
		m_AxisLabels[2].set("z", m_Font);
		m_AxisLabels[2].setColor(glm::vec4(0.0, 0.0, 1.0, 1.0));

		m_SliceTexture = NULL;
		m_SliceQuad = NULL;
		m_SliceShader = ResourceManager::getInstance().loadShader("quad", Resources_quad_vert, sizeof(Resources_quad_vert), Resources_quad_frag, sizeof(Resources_quad_frag));

		m_Mesh = NULL;
		m_MeshShader = ResourceManager::getInstance().loadShader("isovolume", Resources_Shaders_isovolume_vert, sizeof(Resources_Shaders_isovolume_vert),
		                                                                      Resources_Shaders_isovolume_frag, sizeof(Resources_Shaders_isovolume_frag));
		m_MeshMaterial.setDiffuse(glm::vec4(SKY_BLUE, 1.0));
		m_MeshMaterial.setShininess(45.0f);
		m_MeshLight.setPosition(glm::vec3(0.0, 10000.0, 0.0));
	}

	virtual ~CloudView()
	{
		SAFE_DELETE(m_SliceTexture);
		SAFE_DELETE(m_SliceQuad);
		SAFE_DELETE(m_Font);
		SAFE_DELETE(m_Mesh);
		SAFE_DELETE(m_PointCloud);
	}

	virtual const char* name() const { return "cloud"; }

	virtual bool bind(Entity* entity)
	{
        if (entity->type() != Entity::GRAPH)
        {
            LOG("[CLOUD] Couldn't bind entity to view : Wrong entity type!\n");
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
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		Transformation transformation;

		glm::vec3 center = m_PointCloud->box().center();
		glm::vec3 min = m_PointCloud->box().min();

		transformation.translate(-center);

		m_PointCloud->draw(context(), m_Camera3D.getProjectionMatrix(), m_Camera3D.getViewMatrix(), transformation.state());

		// IsoVolume rendering
		if (m_Mesh != NULL)
		{
			m_MeshShader->use();
			{
				m_MeshShader->uniform("u_ModelMatrix").set(transformation.state());
				m_MeshShader->uniform("u_ViewMatrix").set(m_Camera3D.getViewMatrix());
				m_MeshShader->uniform("u_ProjectionMatrix").set(m_Camera3D.getProjectionMatrix());
				m_MeshShader->uniform("u_NormalMatrix").set(glm::transpose(glm::inverse(glm::mat3(m_Camera3D.getViewMatrix() * transformation.state()))));

				m_MeshShader->uniform("u_Light.Type").set(m_MeshLight.getType());
				m_MeshShader->uniform("u_Light.Position").set(m_MeshLight.getPosition());
				m_MeshShader->uniform("u_Light.Direction").set(m_MeshLight.getDirection());
				m_MeshShader->uniform("u_Light.Color").set(m_MeshLight.getColor());

				m_MeshShader->uniform("u_Material.Ambient").set(m_MeshMaterial.getAmbient());
				m_MeshShader->uniform("u_Material.Diffuse").set(m_MeshMaterial.getDiffuse());
				m_MeshShader->uniform("u_Material.Specular").set(m_MeshMaterial.getSpecular());
				m_MeshShader->uniform("u_Material.Shininess").set(m_MeshMaterial.getShininess());
			}

			context()->geometry().bind(m_Mesh->getVertexBuffer(), *m_MeshShader);
			context()->geometry().drawElements(GL_TRIANGLES, m_Mesh->getTriangleBuffer().size() / sizeof(unsigned int), GL_UNSIGNED_INT, m_Mesh->getTriangleBuffer().ptr());
			context()->geometry().unbind(m_Mesh->getVertexBuffer());
		}

		if (m_SliceQuad != NULL)
		{
			transformation.push();

			glm::vec3 diff = m_PointCloud->box().max() - m_PointCloud->box().min();
			transformation.scale(glm::vec3(diff.x, diff.y, diff.z));
			transformation.rotate(90, glm::vec3(1.0, 0.0, 0.0));

			m_SliceShader->use();
			m_SliceShader->uniform("u_ModelViewProjectionMatrix").set(m_Camera3D.getViewProjectionMatrix() * transformation.state());
			// m_SliceShader->uniform("u_NormalMatrix").set(glm::transpose(glm::inverse(glm::mat3(m_Camera3D.getViewMatrix() * transformation.state()))));
			m_SliceShader->uniform("u_Texture").set(*m_SliceTexture);

			context()->geometry().bind(m_SliceQuad->getVertexBuffer(), *m_SliceShader);
			context()->geometry().drawElements(GL_TRIANGLES, m_SliceQuad->getTriangleBuffer().size() / sizeof(unsigned char), GL_UNSIGNED_BYTE, m_SliceQuad->getTriangleBuffer().ptr());
			context()->geometry().unbind(m_SliceQuad->getVertexBuffer());

			transformation.pop();
		}

		// Axis label rendering
		float textSize = 0.04;

		transformation.push();
		{
			transformation.translate(-min);

			transformation.push();
			transformation.translate(glm::vec3(center.x, 0, 0));
			transformation.scale(glm::vec3(textSize, textSize, textSize));
			m_AxisLabels[0].draw(context(), m_Camera3D.getProjectionMatrix() * m_Camera3D.getViewMatrix() * transformation.state());
			transformation.pop();

			transformation.push();
			transformation.translate(glm::vec3(0.0, center.y, 0.0));
			transformation.rotate(-90, glm::vec3(0, 0, 1.0));
			transformation.scale(glm::vec3(textSize, textSize, textSize));
			m_AxisLabels[1].draw(context(), m_Camera3D.getProjectionMatrix() * m_Camera3D.getViewMatrix() * transformation.state());
			transformation.pop();

			transformation.push();
			transformation.translate(glm::vec3(0.0, 0.0, center.z));
			transformation.rotate(-90, glm::vec3(0, 1.0, 0.0));
			transformation.scale(glm::vec3(textSize, textSize, textSize));
			m_AxisLabels[2].draw(context(), m_Camera3D.getProjectionMatrix() * m_Camera3D.getViewMatrix() * transformation.state());
			transformation.pop();
		}
		transformation.pop();
	}

	virtual void idle()
	{
	}

	virtual IVariable* getNodeAttribute(Node::ID id, std::string& name)
	{
		LOG("[CLOUDVIEW] getNodeAttribute(%lu, %s)\n", id, name.c_str());
		return NULL;
	}

	virtual IVariable* getLinkAttribute(Link::ID id, std::string& name)
	{
		LOG("[CLOUDVIEW] getLinkAttribute(%lu, %s)\n", id, name.c_str());
		return NULL;
	}

	void notify(IMessage* message)
	{
		(void) message;
	}

	inline PointCloud& getPointCloud() { return *m_PointCloud; }

	// ----- Graph Events -----

	virtual void onSetAttribute(const std::string& name, VariableType type, const std::string& value)
	{
		LOG("[CLOUDVIEW] setAttribute(%s, %s)\n", name.c_str(), value.c_str());

		if (name == "cloud:update" && type == BOOLEAN)
		{
			m_PointCloud->update();
		}
		else if (name == "cloud:isovolume")
		{
			SAFE_DELETE(m_Mesh);
			m_Mesh = new Mesh("isovolume");
			unsigned int dimension[3] = {50, 50, 50};
			m_MarchingCubes.buildGrid(m_PointCloud, dimension, m_PointCloud->box().min(), m_PointCloud->box().max());
			m_MarchingCubes.polygonize(0.06, *m_Mesh); // IP A.B.C.D Set
			// m_MarchingCubes.polygonize(17.0, *m_Mesh); // IP Cylinder A.B.C.D set
			// m_MarchingCubes.polygonize(9, *m_Mesh); // IP Geolocation Data set
			m_Mesh->update();
		}
		else if (name == "cloud:slice")
		{
			SAFE_DELETE(m_SliceQuad);
			SAFE_DELETE(m_SliceTexture);

			glm::vec3 diff = m_PointCloud->box().max() - m_PointCloud->box().min();
			unsigned int sliceTextureWidth = 4096;
			unsigned int sliceTextureHeight = (unsigned int) ((float)sliceTextureWidth * diff.y / diff.x);

			unsigned int dimension[3];
			dimension[0] = sliceTextureWidth;
			dimension[1] = sliceTextureHeight;
			dimension[2] = 1;

			m_SliceTexture = new Texture("slice", dimension[0], dimension[1], 3);
			m_SliceQuad = new Quad();

			glm::vec3 step;
			step.x = diff.x / (float)dimension[0];
			step.y = diff.y / (float)dimension[1];
			step.z = diff.z / (float)dimension[2];

			for (unsigned int z = 0; z < dimension[2]; z++)
				for (unsigned int y = 0; y < dimension[1]; y++)
					for (unsigned int x = 0; x < dimension[0]; x++)
					{
						glm::vec3 pos = m_PointCloud->box().min() + glm::vec3((float) x, (float) y, (float)z) * step;

						PointCloud::ID id;
						id = m_PointCloud->getNearestPoint(pos);
						PointCloud::PointVertex v;
						m_PointCloud->getPoint(id, &v);

						float factor;
						float d = glm::length(pos - v.Position);
						if (d < 0.0000001f)
							factor = 1.0f;
						else
							factor = 1 / log(1 + d);

						// factor = 1 - factor;

						unsigned char color[3];
						color[0] = (unsigned char)std::min(v.Color[0] * factor * 255.0f, 255.0f);
						color[1] = (unsigned char)std::min(v.Color[1] * factor * 255.0f, 255.0f);
						color[2] = (unsigned char)std::min(v.Color[2] * factor * 255.0f, 255.0f);

						m_SliceTexture->setPixel(x, y, color, 3);
					}

			m_SliceTexture->update();
			m_SliceTexture->dump("voronoi.tga");
		}
		else if (name == "cloud:x:label" && type == STRING)
		{
			m_AxisLabels[0].set(value.c_str(), m_Font);
		}
		else if (name == "cloud:y:label" && type == STRING)
		{
			m_AxisLabels[1].set(value.c_str(), m_Font);
		}
		else if (name == "cloud:z:label" && type == STRING)
		{
			m_AxisLabels[2].set(value.c_str(), m_Font);
		}
	}

	void onAddNode(Node::ID id, const char* label)
	{
		(void) id;
		(void) label;

		PointCloud::PointVertex point;
		point.Position = glm::vec3(0, 0, 0);
		point.Color = glm::vec4(1.0, 1.0, 1.0, 1.0);
		m_PointCloud->addPoint(point);
	}

	void onRemoveNode(Node::ID id) { (void) id; }

	void onSetNodeAttribute(Node::ID id, const std::string& name, VariableType type, const std::string& value)
	{
	    Vec3Variable vvec3;
	    Vec4Variable vvec4;

		if (name == "cloud:position" && type == VEC3)
		{
			vvec3.set(value);
			PointCloud::PointVertex point;
			m_PointCloud->getPoint(id, &point);
			point.Position = vvec3.value();
			m_PointCloud->setPoint(id, &point);
		}
        else if (name == "cloud:color" && type == VEC3)
        {
            vvec3.set(value);
            PointCloud::PointVertex point;
            m_PointCloud->getPoint(id, &point);
            point.Color = glm::vec4(vvec3.value(), 1.0);
            m_PointCloud->setPoint(id, &point);
        }
        else if (name == "cloud:color" && type == VEC4)
        {
            vvec4.set(value);
            PointCloud::PointVertex point;
            m_PointCloud->getPoint(id, &point);
            point.Color = vvec4.value();
            m_PointCloud->setPoint(id, &point);
        }
		else
		{
			LOG("[CLOUD] Attribute '%s' ignored.\n", value.c_str());
		}
	}

	void onSetNodeLabel(Node::ID id, const char* label)
	{
		(void) id;
		(void) label;
	}

	void onSetNodeMark(Node::ID id, unsigned int mark)
	{
		(void) id;
		(void) mark;
	}

	void onSetNodeWeight(Node::ID id, float weight)
	{
		(void) id;
		(void) weight;
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

	void onSetSphereMark(Sphere::ID id, unsigned int mark)
	{
		(void) id;
		(void) mark;
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
	PointCloud* m_PointCloud;

	Mesh* m_Mesh;
	Shader::Program* m_MeshShader;
	Material m_MeshMaterial;
	Light m_MeshLight;

	Texture* m_SliceTexture;
	Quad* m_SliceQuad;
	Shader::Program* m_SliceShader;

	MarchingCubes m_MarchingCubes;

	Font* m_Font;
	Text m_AxisLabels[3];
};

