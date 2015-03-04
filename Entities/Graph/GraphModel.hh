#pragma once

#include <raindance/Core/Variables.hh>

class Node;
class Edge;
class Sphere;
class GraphModel;

template <class T, class U>
class TranslationMap
{
public:
    void addRemoteID(T rid, U lid)
    {
        m_Local[rid] = lid;
        m_Remote[lid] = rid;
    }

    void eraseRemoteID(T rid, U lid)
    {
        m_Local.erase(rid);
        m_Remote.erase(lid);
    }

    void removeLocalID(U lid)
    {
        m_Local.erase(m_Remote[lid]);
        m_Remote.erase(lid);
    }

    inline bool containsRemoteID(T rid) const { return (m_Local.find(rid) == m_Local.end() ? false : true); }
    inline bool containsLocalID(U lid) const { return (m_Remote.find(lid) == m_Remote.end() ? false : true); }

    inline U getLocalID(T rid) { return m_Local[rid]; }
    inline T getRemoteID(U lid) { return m_Remote[lid]; }

private:
	std::unordered_map<U, T> m_Local;
	std::unordered_map<T, U> m_Remote;
};

class Node
{
public:
	typedef enum Type
	{
		DISK,
		SQUARE,
		STAR,
	} Type;

	typedef unsigned long ID;

	struct Data
	{
		std::string Label;
	};

	Node()
	{
	}

	Node(Type type, ID id, Data data)
	{
		set(type, id, data);
	}

	void set(Type type, ID id, Data data)
	{
		m_Type = type;
		m_ID = id;
		m_Data = data;
	}

	// Property getters
	inline ID id() const { return m_ID; }
	inline Type type() const { return m_Type; }
	inline const Data& data() const { return m_Data; }

	// Property setters
	inline void data(Data& d) { m_Data = d; }
	inline void type(Type t)  { m_Type = t; }

	// Accessors
	inline Variables& attributes() { return m_Attributes; }

private:
	Type m_Type;
	ID m_ID;
	Data m_Data;
	Variables m_Attributes;
};

class Edge
{
public:
	typedef enum Type
	{
		DEFAULT
	} Type;

	typedef unsigned long ID;

	struct Data
	{
		Node::ID Node1;
		Node::ID Node2;
	};

	Edge()
	{
	}

	Edge(Type type, ID id, Data data)
	{
		set(type, id, data);
	}

	void set(Type type, ID id, Data data)
	{
		m_Type = type;
		m_ID = id;
		m_Data = data;
	}

	// Property getters
	inline ID id() const { return m_ID; }
	inline Type type() const { return m_Type; }
	inline const Data& data() const { return m_Data; }

	// Property setters
	inline void data(Data& d) { m_Data = d; }
	inline void type(Type t)  { m_Type = t; }

	// Accessors
	inline Variables& attributes() { return m_Attributes; }

private:
	Type m_Type;
	ID m_ID;
	Data m_Data;
	Variables m_Attributes;
};

class Sphere
{
public:
	typedef enum Type
	{
		DEFAULT
	} Type;

	typedef unsigned long ID;

	struct Data
	{
		std::string Label;
		unsigned int Mark;
		std::vector<Node::ID> Nodes;
	};

	Sphere()
	{
	}

	Sphere(Type type, ID id, Data data)
	{
		set(type, id, data);
	}

	void set(Type type, ID id, Data data)
	{
		m_Type = type;
		m_ID = id;
		m_Data = data;
	}

	// Property getters
	inline ID id()   const { return m_ID; }
	inline Type type() const { return m_Type; }
	inline Data& data() { return m_Data; }

	// Property setters
	inline void data(Data& d) { m_Data = d; }
	inline void type(Type t)  { m_Type = t; }

	// Accessors
	inline Variables& attributes() { return m_Attributes; }

private:
	Type m_Type;
	ID m_ID;
	Data m_Data;
	Variables m_Attributes;
};

class GraphModel : public EntityModel
{
public:

	GraphModel()
	{
		m_NodeCounter = 0;
		m_EdgeCounter = 0;
	}

	~GraphModel()
	{
	}

	// ----- Nodes -----

	Node::ID addNode(Node::Type type, Node::Data data)
	{
		Node n(type, m_NodeCounter, data);
		m_Nodes.push_back(n);
		m_NodeCounter++;

		return n.id();
	}
	void removeNode(Node::ID id)
	{
		if (m_SelectedNodes.find(id) != m_SelectedNodes.end())
			m_SelectedNodes.erase(id);

		std::vector<Sphere>::iterator its;
		for (its = m_Spheres.begin(); its != m_Spheres.end(); ++its)
		{
			std::vector<Node::ID>::iterator itsn;
			for (itsn = its->data().Nodes.begin(); itsn != its->data().Nodes.end();)
				if (*itsn == id)
					its->data().Nodes.erase(itsn);
				else
					++itsn;
		}

		std::vector<Edge>::iterator itl;
		for (itl = m_Edges.begin(); itl != m_Edges.end();)
		{
			if (itl->data().Node1 == id || itl->data().Node2 == id)
				m_Edges.erase(itl);
			else
				++itl;
		}

		std::vector<Node>::iterator itn;
		for (itn = m_Nodes.begin(); itn != m_Nodes.end(); ++itn)
		{
			if (itn->id() == id)
			{
				m_Nodes.erase(itn);
				break;
			}
		}
	}
	inline unsigned long countNodes() const { return m_Nodes.size(); }

	Node* node(Node::ID id)
	{
		std::vector<Node>::iterator itn;
		for (itn = m_Nodes.begin(); itn != m_Nodes.end(); ++itn)
			if (itn->id() == id)
				return &(*itn);
		return NULL;
	}

	inline const std::vector<Node>::iterator nodes_begin() { return m_Nodes.begin(); }
	inline const std::vector<Node>::iterator nodes_end() { return m_Nodes.end(); }

	inline void selectNode(const Node::ID id) { m_SelectedNodes.insert(id); }
	inline void unselectNode(const Node::ID id) { m_SelectedNodes.erase(id); }
	inline unsigned long countSelectedNodes() const { return m_SelectedNodes.size(); }
	inline Node& selectedNode(unsigned int index)
	{
		std::set<Node::ID>::iterator it;
		unsigned int count = 0;
		for (it = m_SelectedNodes.begin(); it!= m_SelectedNodes.end(); ++it)
			if (count == index)
				break;
		return m_Nodes[*it];
	}
	inline const std::set<Node::ID>::iterator selectedNodes_begin() { return m_SelectedNodes.begin(); }
	inline const std::set<Node::ID>::iterator selectedNodes_end() { return m_SelectedNodes.end(); }

	// ----- Edges -----

	Edge::ID addEdge(Edge::Type type, Edge::Data data)
	{
		Edge l(type, m_EdgeCounter, data);
		m_Edges.push_back(l);
		m_EdgeCounter++;

		return l.id();
	}

	void removeEdge(Edge::ID id)
	{
		std::vector<Edge>::iterator it;
		for (it = m_Edges.begin(); it != m_Edges.end(); ++it)
		{
			if (it->id() == id)
			{
				m_Edges.erase(it);
				break;
			}
		}
	}

	Edge* edge(Edge::ID id)
	{
		std::vector<Edge>::iterator itl;
		for (itl = m_Edges.begin(); itl != m_Edges.end(); ++itl)
			if (itl->id() == id)
				return &(*itl);
		return NULL;
	}

    inline unsigned long countEdges() const { return m_Edges.size(); }
	inline const std::vector<Edge>::iterator edges_begin() { return m_Edges.begin(); }
	inline const std::vector<Edge>::iterator edges_end() { return m_Edges.end(); }

	// ----- Spheres -----

	Sphere::ID addSphere(Sphere::Type type, Sphere::Data data)
	{
		Sphere s(type, m_Spheres.size(), data);
		m_Spheres.push_back(s);
		return s.id();
	}

	void removeSphere(Sphere::ID id)
	{
		(void) id;
		// TODO
	}

	inline unsigned long countSpheres() const { return m_Spheres.size(); }
	inline Sphere& sphere(Sphere::ID id) { return m_Spheres[id]; }
	inline const std::vector<Sphere>::iterator spheres_begin() { return m_Spheres.begin(); }
	inline const std::vector<Sphere>::iterator spheres_end() { return m_Spheres.end(); }

	// ----- Attributes -----

	inline Variables& attributes() { return m_Attributes; }

	// ----- Helpers -----

	std::pair<Node::ID, Edge::ID> addNeighbor(Node::Type ntype, Node::Data ndata, Edge::Type ltype, Edge::Data ldata, Node::ID neighbor)
	{
		Node n(ntype, m_NodeCounter, ndata);
		m_Nodes.push_back(n);

		ldata.Node1 = neighbor;
		ldata.Node2 = n.id();

		Edge l(ltype, m_EdgeCounter, ldata);
		m_Edges.push_back(l);

		m_NodeCounter++;
		m_EdgeCounter++;

		return std::pair<Node::ID, Edge::ID>(n.id(), l.id());
	}

private:
	std::vector<Node> m_Nodes;
	std::vector<Edge> m_Edges;
	std::vector<Sphere> m_Spheres;

	std::set<Node::ID> m_SelectedNodes;

	Variables m_Attributes;

	Node::ID m_NodeCounter;
	Edge::ID m_EdgeCounter;
};
