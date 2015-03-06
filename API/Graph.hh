#pragma once

#include <graphiti/Entities/Graph/GraphCommands.hh>

namespace API {
namespace Graph {

inline GraphEntity* getActiveGraph()
{
    auto entity = g_Graphiti->entities().active();
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

    // ---- Edges -----

    Edge::ID addEdge(Node::ID uid1, Node::ID uid2)
    {
        // LOG("[API] addEdge(%lu, %lu)\n", uid1, uid2);
        return getActiveGraph()->addEdge(uid1, uid2);
    }

    void removeEdge(Edge::ID id)
    {
        // LOG("[API] removeEdge(%lu)\n", id);
        return getActiveGraph()->removeEdge(id);
    }

    unsigned long countEdges()
    {
        // LOG("[API] countEdges()\n");
        return getActiveGraph()->countEdges();
    }

    Edge::ID getEdgeID(unsigned int i)
    {
        // LOG("[API] getEdgeID(%lu)\n", i);
        return getActiveGraph()->getEdgeID(i);
    }
}

    std::vector<Edge::ID> getEdgeIDs()
    {
        // LOG("[API] getEdgeIDs()\n");
        return getActiveGraph()->getEdgeIDs();
    }

extern "C"
{
    Node::ID getEdgeNode1(Edge::ID id)
    {
        // LOG("[API] getEdgeNode1(%lu)", id);
        return getActiveGraph()->getEdgeNode1(id);
    }

    Node::ID getEdgeNode2(Edge::ID id)
    {
        // LOG("[API] getEdgeNode2(%lu)", id);
        return getActiveGraph()->getEdgeNode2(id);
    }

    void setEdgeAttribute(Edge::ID id, const char* name, const char* type, const char* value)
    {
        // LOG("[API] setEdgeAttribute(%lu, '%s', '%s', '%s')\n", id, name, type, value);
        return getActiveGraph()->setEdgeAttribute(id, name, type, value);
    }
}

    IVariable* getEdgeAttribute(Edge::ID id, const char* name)
    {
        // LOG("[API] getEdgeAttribute(%lu, '%s')\n", id, name);
        return getActiveGraph()->getEdgeAttribute(id, name);
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

    std::pair<Node::ID, Edge::ID> addNeighbor(const char* label, Node::ID neighbor)
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
        else if (sname == "graph:add_edge")
            command = GraphCommandFactory::AddEdge(graph, variables);
        else if (sname == "graph:remove_edge")
            command = GraphCommandFactory::RemoveEdge(graph, variables);
        else if (sname == "graph:set_edge_attribute")
            command = GraphCommandFactory::SetEdgeAttribute(graph, variables);
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
