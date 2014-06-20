#pragma once

#include "Entities/Graph/GraphModel.hh"

class LinkAttractionForce : public Physics::IForce
{
public:
	LinkAttractionForce()
	{
	    m_GraphModel = NULL;
	    m_NodeTranslationMap = NULL;
		m_MinNodeDistance = 10.0f;
	}

	virtual ~LinkAttractionForce()
	{
	}

	void bind(GraphModel* model, NodeTranslationMap* nodeTranslationMap)
	{
		m_GraphModel = model;
		m_NodeTranslationMap = nodeTranslationMap;
	}

	void apply(Scene::NodeVector& nodes)
	{
		if (!m_GraphModel)
		{
			LOG("[SPACEVIEW] Need a bound graph model to apply force !\n");
			return;
		}

		glm::vec3 pos1, dir1;
		glm::vec3 pos2, dir2;

		const float volume = 20 * 20 * 20; // NOTE : Graph should fit in this cube
		float k = pow(volume / m_GraphModel->countNodes(), 1.0 / 3.0);

		// Calculate link attractive forces
		std::vector<Link>::iterator itl;
		for (itl = m_GraphModel->links_begin(); itl != m_GraphModel->links_end(); ++itl)
		{
			SpaceNode::ID id1 = m_NodeTranslationMap->getLocalID(itl->data().Node1);
			SpaceNode::ID id2 = m_NodeTranslationMap->getLocalID(itl->data().Node2);

			pos1 = nodes[id1]->getPosition();
			pos2 = nodes[id2]->getPosition();
			dir1 = nodes[id1]->getDirection();
			dir2 = nodes[id2]->getDirection();

			glm::vec3 dir = pos1 - pos2;
			float d = glm::length(dir);
			if (d < m_MinNodeDistance)
				continue;

			// Attractive Force : fa(x) = x * x / k
			dir1 = dir1 - (dir / d) * (d * d / k);
			dir2 = dir2 + (dir / d) * (d * d / k);

			nodes[id1]->setDirection(dir1, false);
			nodes[id2]->setDirection(dir2, false);
		}
	}

private:
	GraphModel* m_GraphModel;
	NodeTranslationMap* m_NodeTranslationMap;

	float m_MinNodeDistance;
};

class NodeRepulsionForce : public Physics::IForce
{
public:
	NodeRepulsionForce()
	{
	    m_GraphModel = NULL;
	    m_NodeTranslationMap = NULL;
	}

	virtual ~NodeRepulsionForce()
	{
	}

	void bind(GraphModel* model, NodeTranslationMap* nodeTranslationMap)
	{
		m_GraphModel = model;
		m_NodeTranslationMap = nodeTranslationMap;
	}

	void apply(Scene::NodeVector& nodes)
	{
		if (!m_GraphModel)
		{
			LOG("[SPACEVIEW] Need a bound graph model to apply force !\n");
			return;
		}

		const float volume = 20 * 20 * 20; // NOTE : Graph should fit in this cube
		float k = pow(volume / m_GraphModel->countNodes(), 1.0 / 3.0);

		// TODO / NOTE : It may be necessary to discard the linked nodes to remove the vibration glitch
		// However it needs a better graph model to access the node neighbors quickly

		glm::vec3 pos1, dir1;
		glm::vec3 pos2, dir2;

		std::vector<Node>::iterator itn1;
		std::vector<Node>::iterator itn2;

		for (itn1 = m_GraphModel->nodes_begin(); itn1 != m_GraphModel->nodes_end(); ++itn1)
		{
			SpaceNode::ID id1 = m_NodeTranslationMap->getLocalID(itn1->id());

			pos1 = nodes[id1]->getPosition();
			dir1 = nodes[id1]->getDirection();

			for (itn2 = m_GraphModel->nodes_begin(); itn2 != m_GraphModel->nodes_end(); ++itn2)
			{
				if (itn1 == itn2)
					break;

				SpaceNode::ID id2 = m_NodeTranslationMap->getLocalID(itn2->id());

				pos2 = nodes[id2]->getPosition();
				dir2 = nodes[id2]->getDirection();

				glm::vec3 dir = pos1 - pos2;
				float d = glm::length(dir);
				if (d < 0.1f)
				{
					// NOTE : Nodes are too close, randomize direction
					float rnd_theta = ((float) rand() / RAND_MAX) * 2.0 * M_PI;
					float rnd_z = ((float) rand() / RAND_MAX) - 1.0;
					dir.x = sqrt(1 - rnd_z * rnd_z) * cos(rnd_theta);
					dir.y = sqrt(1 - rnd_z * rnd_z) * sin(rnd_theta);
					dir.z = rnd_z;
					d = 1.0;
				}

				// Repulsive Force : fr(x) = k * k / x
				glm::vec3 fr = (dir / d) * (k * k / d);
				dir1 = dir1 + fr;
				dir2 = dir2 - fr;

				nodes[id2]->setDirection(dir2, false);
			}

			nodes[id1]->setDirection(dir1, false);
		}
	}

private:
	GraphModel* m_GraphModel;
	NodeTranslationMap* m_NodeTranslationMap;
};

class DustAttractor : public Physics::IForce
{
public:
	DustAttractor()
	{
		m_Position = glm::vec3(0, 0, 0);
		m_Radius = 200.0f;
		m_Factor = 100.0f;
	}

	virtual ~DustAttractor()
	{
	}

	void apply(Scene::NodeVector& nodes)
	{
		Scene::NodeVector::iterator it;
		for (it = nodes.begin(); it != nodes.end(); ++it)
		{
			if (*it == NULL)
				continue;

			glm::vec3 d = m_Position - (*it)->getPosition();
			if (glm::length2(d) > m_Radius * m_Radius)
				(*it)->setDirection((*it)->getDirection() + glm::normalize(d) * m_Factor, false);
		}
	}

	inline float getRadius() { return m_Radius; }
private:
	glm::vec3 m_Position;
	float m_Radius;
	float m_Factor;
};
