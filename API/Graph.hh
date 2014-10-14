#pragma once

#include "Entities/Graph/GraphCommands.hh"

namespace API {
namespace Graph {

inline GraphEntity* getActiveGraph()
{
    auto entity = g_Graphiti.entities().active();
    if (entity->type() != Entity::GRAPH)
    {
        LOG("[API] Active entity is not a graph!\n");
        throw;
    }
    return static_cast<GraphEntity*>(entity);
}

extern "C" {

    // ----- Nodes -----

    Node::ID addNode(const char* label)
    {
        // LOG("[API] addNode('%s')\n", label);
        return getActiveGraph()->addNode(label);
    }

    void removeNode(Node::ID id)
    {
        // LOG("[API] removeNode(%lu)\n", id);
        return getActiveGraph()->removeNode(id);
    }

    void tagNode(Node::ID node, Sphere::ID sphere)
    {
        // LOG("[API] tagNode(%lu, %lu)\n", node, sphere);
        return getActiveGraph()->tagNode(node, sphere);
    }

    unsigned long countNodes()
    {
        // LOG("[API] countNodes()\n");
        return getActiveGraph()->countNodes();
    }

    Node::ID getNodeID(unsigned int i)
    {
        // LOG("[API] getNodeID(%u)", i);
        return getActiveGraph()->getNodeID(i);
    }
}

    std::vector<Node::ID> getNodeIDs()
    {
        // LOG("[API] getNodeIDs()\n");
        return getActiveGraph()->getNodeIDs();
    }

extern "C"
{
    void setNodeLabel(Node::ID id, const char* label)
    {
        // LOG("[API] setNodeLabel(%lu, '%s')\n", id, label);
        return getActiveGraph()->setNodeLabel(id, label);
    }

    const char* getNodeLabel(Node::ID id)
    {
        // LOG("[API] getNodeLabel(%lu)\n", id);
        return getActiveGraph()->getNodeLabel(id);
    };

    void setNodeAttribute(Node::ID id, const char* name, const char* type, const char* value)
    {
        // LOG("[API] setNodeAttribute(%lu, '%s', '%s', '%s')\n", id, name, type, value);
        return getActiveGraph()->setNodeAttribute(id, name, type, value);
    }

    IVariable* getNodeAttribute(Node::ID id, const char* name)
    {
        // LOG("[API] getNodeAttribute(%lu, '%s')\n", id, name);
        return getActiveGraph()->getNodeAttribute(id, name);
    }

    // ---- Links -----

    Link::ID addLink(Node::ID uid1, Node::ID uid2)
    {
        // LOG("[API] addLink(%lu, %lu)\n", uid1, uid2);
        return getActiveGraph()->addLink(uid1, uid2);
    }

    void removeLink(Link::ID id)
    {
        // LOG("[API] removeLink(%lu)\n", id);
        return getActiveGraph()->removeLink(id);
    }

    unsigned long countLinks()
    {
        // LOG("[API] countLinks()\n");
        return getActiveGraph()->countLinks();
    }

    Link::ID getLinkID(unsigned int i)
    {
        // LOG("[API] getLinkID(%lu)\n", i);
        return getActiveGraph()->getLinkID(i);
    }
}

    std::vector<Link::ID> getLinkIDs()
    {
        // LOG("[API] getLinkIDs()\n");
        return getActiveGraph()->getLinkIDs();
    }

extern "C"
{
    Node::ID getLinkNode1(Link::ID id)
    {
        // LOG("[API] getLinkNode1(%lu)", id);
        return getActiveGraph()->getLinkNode1(id);
    }

    Node::ID getLinkNode2(Link::ID id)
    {
        // LOG("[API] getLinkNode2(%lu)", id);
        return getActiveGraph()->getLinkNode2(id);
    }

    void setLinkAttribute(Link::ID id, const char* name, const char* type, const char* value)
    {
        // LOG("[API] setLinkAttribute(%lu, '%s', '%s', '%s')\n", id, name, type, value);
        return getActiveGraph()->setLinkAttribute(id, name, type, value);
    }
}

    IVariable* getLinkAttribute(Link::ID id, const char* name)
    {
        // LOG("[API] getLinkAttribute(%lu, '%s')\n", id, name);
        return getActiveGraph()->getLinkAttribute(id, name);
    }

extern "C"
{
    // ----- Spheres -----

    Sphere::ID addSphere(const char* label)
    {
        // LOG("[API] addSphere('%s')\n", label);
        return getActiveGraph()->addSphere(label);
    }
}
    // ----- Helpers -----

    std::pair<Node::ID, Link::ID> addNeighbor(const char* label, Node::ID neighbor)
    {
        // LOG("[API] addNeighbor('%s', %lu)\n", label, neighbor);
        return getActiveGraph()->addNeighbor(label, neighbor);
    }

extern "C"
{
    unsigned long countSelectedNodes()
    {
        // LOG("[API] countSelectedNodes()\n");
        return getActiveGraph()->countSelectedNodes();
    }

    Node::ID getSelectedNode(unsigned int index)
    {
        // LOG("[API] getSelectedNode(%u)\n", index);
        return getActiveGraph()->getSelectedNode(index);
    }
}

    // ----- Commands -----

    Sequence::ID sendCommand(Timecode timecode, const char* name, const Variables& variables)
    {
        Sequence* command = NULL;
        std::string sname = std::string(name);

        // LOG("[API] sendCommand(%lu, '%s', %p)\n", timecode, name, &variables);

        GraphEntity* graph = getActiveGraph();

        if (sname == "graph:set_attribute")
            command = GraphCommandFactory::SetAttribute(graph, variables);
        else if (sname == "graph:add_node")
            command = GraphCommandFactory::AddNode(graph, variables);
        else if (sname == "graph:remove_node")
            command = GraphCommandFactory::RemoveNode(graph, variables);
        else if (sname == "graph:set_node_attribute")
            command = GraphCommandFactory::SetNodeAttribute(graph, variables);
        else if (sname == "graph:add_link")
            command = GraphCommandFactory::AddLink(graph, variables);
        else if (sname == "graph:remove_link")
            command = GraphCommandFactory::RemoveLink(graph, variables);
        else if (sname == "graph:set_link_attribute")
            command = GraphCommandFactory::SetLinkAttribute(graph, variables);
        else
            LOG("[API] Unknown command type '%s'!\n", name);

        if (command == NULL)
        {
            LOG("[API] Couldn't create command.\n");
            throw;
        }

        graph->context()->sequencer().track("command")->insert(command, Track::Event::ONCE, timecode);
        // TODO : Find a way to not overload the scheduler when we load a lot of commands at once
        graph->context()->messages().push(new SequencerMessage("command", "update"));
        return command->id();
    }
}
}
