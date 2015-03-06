#pragma once

#include <graphiti/Entities/Graph/GraphModel.hh>

class GraphContext : public EntityContext
{
};

class GraphListener : public EntityListener
{
public:
    virtual void onAddNode(Node::ID uid, const char* label)
    { (void) uid; (void) label; }

    virtual void onRemoveNode(Node::ID uid)
    { (void) uid; }

    virtual void onSetNodeAttribute(Node::ID uid, const std::string& name, VariableType type, const std::string& value)
    { (void) uid; (void) name; (void) type; (void) value; }

    virtual void onSetNodeLabel(Node::ID uid, const char* label)
    { (void) uid; (void) label; }

    virtual void onTagNode(Node::ID uid, Sphere::ID sphere)
    { (void) uid; (void) sphere; }

    virtual void onAddEdge(Edge::ID uid, Node::ID uid1, Node::ID uid2)
    { (void) uid; (void) uid1; (void) uid2; }

    virtual void onRemoveEdge(Edge::ID uid)
    { (void) uid; }

    virtual void onSetEdgeAttribute(Edge::ID uid, const std::string& name, VariableType type, const std::string& value)
    { (void) uid; (void) name; (void) type; (void) value; }

    virtual void onAddNeighbor(const std::pair<Node::ID, Edge::ID>& element, const char* label, Node::ID neighbor)
    { (void) element; (void) label; (void) neighbor; }

    virtual void onAddSphere(Sphere::ID uid, const char* label)
    { (void) uid; (void) label; }
};

class GraphView : public EntityView, public GraphListener
{
public:
    virtual IVariable* getNodeAttribute(Node::ID id, std::string& name) = 0;

    virtual IVariable* getEdgeAttribute(Edge::ID id, std::string& name) = 0;
};

class GraphController : public EntityController, public GraphListener
{
};

// Exposed Graph API
class GraphEntity : public Entity
{
public:
    GraphEntity()
    : Entity(GRAPH)
    {
        m_GraphContext = new GraphContext();
        m_GraphModel = new GraphModel();
    }

    virtual ~GraphEntity()
    {
        SAFE_DELETE(m_GraphModel);
        SAFE_DELETE(m_GraphContext);
    }

    virtual void send(const Variables& input, Variables& output)
    {
        (void) output;
        LOG("[GRAPH] Message :\n");
        input.dump();
    }

    Node::ID addNode(const char* label)
    {
        Node::ID id;
        Node::Data data;

        data.Label = std::string(label);

        id = m_GraphModel->addNode(data);

        for (auto l : listeners())
            static_cast<GraphListener*>(l)->onAddNode(id, label);

        return id;
    }

    void removeNode(Node::ID id)
    {
        m_GraphModel->removeNode(id);

        for (auto l : listeners())
            static_cast<GraphListener*>(l)->onRemoveNode(id);
    }

    void tagNode(Node::ID node, Sphere::ID sphere)
    {
        m_GraphModel->sphere(sphere).data().Nodes.push_back(node);

        for (auto l : listeners())
            static_cast<GraphListener*>(l)->onTagNode(node, sphere);
    }

    unsigned long countNodes() { return m_GraphModel->countNodes(); }

    Node::ID getNodeID(unsigned int i) { return m_GraphModel->node(i)->id(); }

    std::vector<Node::ID> getNodeIDs()
    {
        std::vector<Node::ID> result;

        for (auto it = m_GraphModel->nodes_begin(); it != m_GraphModel->nodes_end(); ++it)
            result.push_back(it->id());

        return result;
    }

    void setNodeLabel(Node::ID id, const char* label)
    {
        Node::Data data = m_GraphModel->node(id)->data();
        data.Label.assign(label);
        m_GraphModel->node(id)->data(data);

        for (auto l : listeners())
            static_cast<GraphListener*>(l)->onSetNodeLabel(id, label);
    }

    const char* getNodeLabel(Node::ID id) { return m_GraphModel->node(id)->data().Label.c_str(); };

    void setNodeAttribute(Node::ID id, const char* name, const char* type, const char* value)
    {
        std::string sname(name);
        std::string stype(type);
        std::string svalue(value);

        VariableType vtype;

        if (stype == "float")
            vtype = RD_FLOAT;
        else if (stype == "string")
            vtype = RD_STRING;
        else if (stype == "int")
            vtype = RD_INT;
        else if (stype == "bool")
            vtype = RD_BOOLEAN;
        else if (stype == "vec2")
            vtype = RD_VEC2;
        else if (stype == "vec3")
            vtype = RD_VEC3;
        else if (stype == "vec4")
            vtype = RD_VEC4;
        else
        {
            std::cout << "Unknown attribute type \"" << stype << "\" !" << std::endl;
            return;
        }

        unsigned long pos = sname.find(":");
        std::string category = sname.substr (0, pos);

        // TODO : Remove 'raindance' attribute namespace whenever possible.
        if (category == "raindance" || category == "graphiti" || category == "og")
        {
            sname = sname.substr(pos + 1);
        }
        else
        {
            m_GraphModel->node(id)->attributes().set(sname, vtype, svalue);
        }

        for (auto l : listeners())
            static_cast<GraphListener*>(l)->onSetNodeAttribute(id, sname, vtype, svalue);
    }

    IVariable* getNodeAttribute(Node::ID id, const char* name)
    {
        std::string sname(name);

        unsigned long pos1 = sname.find(":");
        std::string category = sname.substr (0, pos1);

        // TODO : Remove 'raindance' attribute namespace whenever possible.
        if (category == "raindance" || category == "graphiti" || category == "og")
        {
            std::string rest = sname.substr(pos1 + 1);
            unsigned long pos2 = rest.find(":");
            std::string view = rest.substr(0, pos2);
            rest = rest.substr(pos2 + 1);

            for (auto v : views())
            {
                if (view == std::string(v->name()))
                    return static_cast<GraphView*>(v)->getNodeAttribute(id, rest);
            }
            return NULL;
        }
        else
        {
            IVariable* attribute = m_GraphModel->node(id)->attributes().get(sname);
            if (attribute)
                return attribute->duplicate();
            else
                return NULL;
        }
    }

    // ---- Edges -----

    Edge::ID addEdge(Node::ID uid1, Node::ID uid2)
    {
        Edge::ID uid;
        Edge::Data data;
        data.Node1 = uid1;
        data.Node2 = uid2;

        uid = m_GraphModel->addEdge(data);

        for (auto l : listeners())
            static_cast<GraphListener*>(l)->onAddEdge(uid, uid1, uid2);

        return uid;
    }

    void removeEdge(Edge::ID id)
    {
        m_GraphModel->removeEdge(id);

        for (auto l : listeners())
            static_cast<GraphListener*>(l)->onRemoveEdge(id);
    }

    unsigned long countEdges() { return m_GraphModel->countEdges(); }

    Edge::ID getEdgeID(unsigned int i) { return m_GraphModel->edge(i)->id(); }

    std::vector<Edge::ID> getEdgeIDs()
    {
        std::vector<Edge::ID> result;

        for (auto it = m_GraphModel->edges_begin(); it != m_GraphModel->edges_end(); ++it)
            result.push_back(it->id());

        return result;
    }

    Node::ID getEdgeNode1(Edge::ID id) { return m_GraphModel->edge(id)->data().Node1; }

    Node::ID getEdgeNode2(Edge::ID id) { return m_GraphModel->edge(id)->data().Node2; }

    void setEdgeAttribute(Edge::ID id, const char* name, const char* type, const char* value)
    {
        std::string sname(name);
        std::string stype(type);
        std::string svalue(value);

        VariableType vtype;

        if (stype == "float")
            vtype = RD_FLOAT;
        else if (stype == "string")
            vtype = RD_STRING;
        else if (stype == "int")
            vtype = RD_INT;
        else if (stype == "bool")
            vtype = RD_BOOLEAN;
        else if (stype == "vec2")
            vtype = RD_VEC2;
        else if (stype == "vec3")
            vtype = RD_VEC3;
        else if (stype == "vec4")
            vtype = RD_VEC4;
        else
        {
            std::cout << "Unknown attribute type \"" << stype << "\" !" << std::endl;
            return;
        }

        unsigned long pos = sname.find(":");
        std::string category = sname.substr (0, pos);

        // TODO : Remove 'raindance' attribute namespace whenever possible.
        if (category == "raindance" || category == "graphiti" || category == "og")
        {
            sname = sname.substr(pos + 1);
        }
        else
        {
            m_GraphModel->edge(id)->attributes().set(sname, vtype, svalue);
        }

        for (auto l : listeners())
            static_cast<GraphListener*>(l)->onSetEdgeAttribute(id, sname, vtype, svalue);
    }

    IVariable* getEdgeAttribute(Edge::ID id, const char* name)
    {
        std::string sname(name);

        unsigned long pos1 = sname.find(":");
        std::string category = sname.substr (0, pos1);

        // TODO : Remove 'raindance' attribute namespace whenever possible.
        if (category == "raindance" || category == "graphiti" || category == "og")
        {
            std::string rest = sname.substr(pos1 + 1);
            unsigned long pos2 = rest.find(":");
            std::string view = rest.substr(0, pos2);
            rest = rest.substr(pos2 + 1);

            for (auto v : views())
            {
                if (view == std::string(v->name()))
                    return static_cast<GraphView*>(v)->getEdgeAttribute(id, rest);
            }
            return NULL;
        }
        else
        {
            IVariable* attribute = m_GraphModel->edge(id)->attributes().get(sname);
            if (attribute)
                return attribute->duplicate();
            else
                return NULL;
        }
    }

    // ----- Spheres -----

    Sphere::ID addSphere(const char* label)
    {
        Sphere::ID id;
        Sphere::Data data;
        data.Label = std::string(label);
        data.Mark = 0;

        id = m_GraphModel->addSphere(data);

        for (auto l : listeners())
            static_cast<GraphListener*>(l)->onAddSphere(id, label);

        return id;
    }

    // ----- Helpers -----

    std::pair<Node::ID, Edge::ID> addNeighbor(const char* label, Node::ID neighbor)
    {
        std::pair<Node::ID, Edge::ID> element;
        Node::Data ndata;

        ndata.Label = std::string(label);

        Edge::Data ldata = Edge::Data();

        element = m_GraphModel->addNeighbor(ndata, ldata, neighbor);

        for (auto l : listeners())
            static_cast<GraphListener*>(l)->onAddNeighbor(element, label, neighbor);

        return element;
    }

    unsigned long countSelectedNodes() { return m_GraphModel->countSelectedNodes(); }

    Node::ID getSelectedNode(unsigned int index) { return m_GraphModel->selectedNode(index).id(); }

    // ----- Entity Properties -----

    inline GraphModel* model() { return m_GraphModel; }
    inline EntityContext* context() { return m_GraphContext; }
private:
    GraphContext* m_GraphContext;
    GraphModel* m_GraphModel;
};
