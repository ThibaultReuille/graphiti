#pragma once

#include "Entities/Graph/GraphModel.hh"

class GraphContext : public EntityContext
{
};

class GraphListener : public EntityListener
{
public:
    virtual void onSetAttribute(const std::string& name, VariableType type, const std::string& value) = 0;

    virtual void onAddNode(Node::ID id, const char* label) = 0;

    virtual void onRemoveNode(Node::ID id) = 0;

    virtual void onSetNodeAttribute(Node::ID id, const std::string& name, VariableType type, const std::string& value) = 0;

    virtual void onSetNodeLabel(Node::ID id, const char* label) = 0;

    virtual void onTagNode(Node::ID node, Sphere::ID sphere) = 0;

    virtual void onAddLink(Link::ID uid, Node::ID uid1, Node::ID uid2) = 0;

    virtual void onRemoveLink(Link::ID id) = 0;

    virtual void onSetLinkAttribute(Link::ID id, const std::string& name, VariableType type, const std::string& value) = 0;

    virtual void onAddNeighbor(const std::pair<Node::ID, Link::ID>& element, const char* label, Node::ID neighbor) = 0;

    virtual void onAddSphere(Sphere::ID id, const char* label) = 0;
};

class GraphView : public EntityView, public GraphListener
{
public:
    virtual IVariable* getNodeAttribute(Node::ID id, std::string& name) = 0;

    virtual IVariable* getLinkAttribute(Link::ID id, std::string& name) = 0;
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

    void setAttribute(const char* name, const char* type, const char* value)
    {
        std::string sname(name);
        std::string stype(type);
        std::string svalue(value);

        VariableType vtype;

        if (stype == "float")
            vtype = FLOAT;
        else if (stype == "string")
            vtype = STRING;
        else if (stype == "int")
            vtype = INT;
        else if (stype == "bool")
            vtype = BOOLEAN;
        else if (stype == "vec2")
            vtype = VEC2;
        else if (stype == "vec3")
            vtype = VEC3;
        else if (stype == "vec4")
            vtype = VEC4;
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
            m_GraphModel->attributes().set(sname, vtype, svalue);
        }

        for (auto l : listeners())
            static_cast<GraphListener*>(l)->onSetAttribute(sname, vtype, svalue);
    }

    Node::ID addNode(const char* label)
    {
        Node::ID id;
        Node::Data data;

        data.Label = std::string(label);
        data.Weight = 1.0;
        data.Mark = 0;

        id = m_GraphModel->addNode(Node::DISK, data);

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
            vtype = FLOAT;
        else if (stype == "string")
            vtype = STRING;
        else if (stype == "int")
            vtype = INT;
        else if (stype == "bool")
            vtype = BOOLEAN;
        else if (stype == "vec2")
            vtype = VEC2;
        else if (stype == "vec3")
            vtype = VEC3;
        else if (stype == "vec4")
            vtype = VEC4;
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

    // ---- Links -----

    Link::ID addLink(Node::ID uid1, Node::ID uid2)
    {
        Link::ID uid;
        Link::Data data;
        data.Node1 = uid1;
        data.Node2 = uid2;

        uid = m_GraphModel->addLink(Link::DEFAULT, data);

        for (auto l : listeners())
            static_cast<GraphListener*>(l)->onAddLink(uid, uid1, uid2);

        return uid;
    }

    void removeLink(Link::ID id)
    {
        m_GraphModel->removeLink(id);

        for (auto l : listeners())
            static_cast<GraphListener*>(l)->onRemoveLink(id);
    }

    unsigned long countLinks() { return m_GraphModel->countLinks(); }

    Link::ID getLinkID(unsigned int i) { return m_GraphModel->link(i)->id(); }

    std::vector<Link::ID> getLinkIDs()
    {
        std::vector<Link::ID> result;

        for (auto it = m_GraphModel->links_begin(); it != m_GraphModel->links_end(); ++it)
            result.push_back(it->id());

        return result;
    }

    Node::ID getLinkNode1(Link::ID id) { return m_GraphModel->link(id)->data().Node1; }

    Node::ID getLinkNode2(Link::ID id) { return m_GraphModel->link(id)->data().Node2; }

    void setLinkAttribute(Link::ID id, const char* name, const char* type, const char* value)
    {
        std::string sname(name);
        std::string stype(type);
        std::string svalue(value);

        VariableType vtype;

        if (stype == "float")
            vtype = FLOAT;
        else if (stype == "string")
            vtype = STRING;
        else if (stype == "int")
            vtype = INT;
        else if (stype == "bool")
            vtype = BOOLEAN;
        else if (stype == "vec2")
            vtype = VEC2;
        else if (stype == "vec3")
            vtype = VEC3;
        else if (stype == "vec4")
            vtype = VEC4;
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
            m_GraphModel->link(id)->attributes().set(sname, vtype, svalue);
        }

        for (auto l : listeners())
            static_cast<GraphListener*>(l)->onSetLinkAttribute(id, sname, vtype, svalue);
    }

    IVariable* getLinkAttribute(Link::ID id, const char* name)
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
                    return static_cast<GraphView*>(v)->getLinkAttribute(id, rest);
            }
            return NULL;
        }
        else
        {
            IVariable* attribute = m_GraphModel->link(id)->attributes().get(sname);
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

        id = m_GraphModel->addSphere(Sphere::DEFAULT, data);

        for (auto l : listeners())
            static_cast<GraphListener*>(l)->onAddSphere(id, label);

        return id;
    }

    // ----- Helpers -----

    std::pair<Node::ID, Link::ID> addNeighbor(const char* label, Node::ID neighbor)
    {
        std::pair<Node::ID, Link::ID> element;
        Node::Type ntype = Node::DISK;
        Node::Data ndata;

        ndata.Label = std::string(label);
        ndata.Weight = 1.0;
        ndata.Mark = 0;

        Link::Data ldata;

        element = m_GraphModel->addNeighbor(ntype, ndata, Link::DEFAULT, ldata, neighbor);

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
