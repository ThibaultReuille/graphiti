#pragma once

#include <raindance/Core/Sequencer/Sequencer.hh>

#include <graphiti/Entities/MVC.hh>

class GraphCommand : public Sequence
{
public:
    GraphCommand(GraphEntity* graph, const char* name)
    : Sequence(name)
    {
        m_Graph = graph;
    }

    virtual ~GraphCommand() {}

    virtual void start(Timecode timecode)
    {
        (void) timecode;
    }

    virtual void stop(Timecode timecode)
    {
        (void) timecode;
    }

protected:
    GraphEntity* m_Graph;
};

class GraphCommand_SetAttribute : public GraphCommand
{
public:
    GraphCommand_SetAttribute(GraphEntity* graph, const char* name, const char* type, const char* value)
    : GraphCommand(graph, "SetAttribute")
    {
        m_Input.Name = std::string(name);
        m_Input.Type = std::string(type);
        m_Input.Value = std::string(value);
    }

    virtual Sequence::Status play(Timecode timecode)
    {
        (void) timecode;
        m_Graph->setAttribute(m_Input.Name.c_str(), m_Input.Type.c_str(), m_Input.Value.c_str());
        LOG("[COMMAND] SetAttribute{ Input : (%s, %s, %s) }\n", m_Input.Name.c_str(), m_Input.Type.c_str(), m_Input.Value.c_str());
        return KILL;
    }

    struct Input
    {
        std::string Name;
        std::string Type;
        std::string Value;
    } m_Input;
};

class GraphCommand_AddNode : public GraphCommand
{
public:
    GraphCommand_AddNode(GraphEntity* graph, const char* label)
    : GraphCommand(graph, "AddNode")
    {
        m_Input.Label = std::string(label);
    }

    virtual Sequence::Status play(Timecode timecode)
    {
        (void) timecode;
        m_Output.ID = m_Graph->addNode(m_Input.Label.c_str());
        LOG("[COMMAND] AddNode { Input : (%s), Output : (%lu) }\n", m_Input.Label.c_str(), m_Output.ID);
        return KILL;
    }

    struct Input { std::string Label; } m_Input;
    struct Output { Node::ID ID; } m_Output;
};

class GraphCommand_RemoveNode : public GraphCommand
{
public:
    GraphCommand_RemoveNode(GraphEntity* graph, Node::ID id)
    : GraphCommand(graph, "RemoveNode")
    {
        m_Input.ID = id;
    }

    virtual Sequence::Status play(Timecode timecode)
    {
        (void) timecode;
        m_Graph->removeNode(m_Input.ID);
        LOG("[COMMAND] RemoveNode { Input : (%lu) }\n", m_Input.ID);
        return KILL;
    }

    struct Input { Node::ID ID; } m_Input;
};

class GraphCommand_SetNodeAttribute : public GraphCommand
{
public:
    GraphCommand_SetNodeAttribute(GraphEntity* graph, Node::ID uid, const char* name, const char* type, const char* value)
    : GraphCommand(graph, "SetNodeAttribute")
    {
        m_Input.UID = uid;
        m_Input.Name = std::string(name);
        m_Input.Type = std::string(type);
        m_Input.Value = std::string(value);
    }

    virtual Sequence::Status play(Timecode timecode)
    {
        (void) timecode;
        m_Graph->setNodeAttribute(m_Input.UID, m_Input.Name.c_str(), m_Input.Type.c_str(), m_Input.Value.c_str());
        LOG("[COMMAND] SetNodeAttribute { Input : (%lu, %s, %s, %s) }\n", m_Input.UID, m_Input.Name.c_str(), m_Input.Type.c_str(), m_Input.Value.c_str());
        return KILL;
    }

    struct Input
    {
        Node::ID UID;
        std::string Name;
        std::string Type;
        std::string Value;
    } m_Input;
};

class GraphCommand_AddEdge : public GraphCommand
{
public:
    GraphCommand_AddEdge(GraphEntity* graph, Node::ID uid1, Node::ID uid2)
    : GraphCommand(graph, "AddEdge")
    {
        m_Input.UID1 = uid1;
        m_Input.UID2 = uid2;
    }

    virtual Sequence::Status play(Timecode timecode)
    {
        (void) timecode;
        m_Output.UID = m_Graph->addEdge(m_Input.UID1, m_Input.UID2);
        LOG("[COMMAND] AddEdge { Input : (%lu, %lu), Output : (%lu) }\n", m_Input.UID1, m_Input.UID2, m_Output.UID);
        return KILL;
    }

    struct Input
    {
        Node::ID UID1;
        Node::ID UID2;
    } m_Input;

    struct Output { Edge::ID UID; } m_Output;
};

class GraphCommand_RemoveEdge : public GraphCommand
{
public:
    GraphCommand_RemoveEdge(GraphEntity* graph, Edge::ID id)
    : GraphCommand(graph, "RemoveEdge")
    {
        m_Input.UID = id;
    }

    virtual Sequence::Status play(Timecode timecode)
    {
        (void) timecode;
        m_Graph->removeEdge(m_Input.UID);
        LOG("[COMMAND] RemoveEdge { Input : (%lu) }\n", m_Input.UID);
        return KILL;
    }

    struct Input { Node::ID UID; } m_Input;
};

class GraphCommand_SetEdgeAttribute : public GraphCommand
{
public:
    GraphCommand_SetEdgeAttribute(GraphEntity* graph, Edge::ID uid, const char* name, const char* type, const char* value)
    : GraphCommand(graph, "SetEdgeAttribute")
    {
        m_Input.UID = uid;
        m_Input.Name = std::string(name);
        m_Input.Type = std::string(type);
        m_Input.Value = std::string(value);
    }

    virtual Sequence::Status play(Timecode timecode)
    {
        (void) timecode;
        m_Graph->setEdgeAttribute(m_Input.UID, m_Input.Name.c_str(), m_Input.Type.c_str(), m_Input.Value.c_str());
        LOG("[COMMAND] SetEdgeAttribute { Input : (%lu, %s, %s, %s) }\n", m_Input.UID, m_Input.Name.c_str(), m_Input.Type.c_str(), m_Input.Value.c_str());
        return KILL;
    }

    struct Input
    {
        Node::ID UID;
        std::string Name;
        std::string Type;
        std::string Value;
    } m_Input;
};

class GraphCommandFactory
{
public:
    // Graph Commands

    static GraphCommand_SetAttribute* SetAttribute(GraphEntity* graph, const Variables& variables)
    {
        IVariable* name = getVariable("name", RD_STRING, variables);
        IVariable* type = getVariable("type", RD_STRING, variables);
        IVariable* value = getVariable("value", RD_STRING, variables);
        if (name == NULL || type == NULL || value == NULL)
            return NULL;

        return new GraphCommand_SetAttribute(graph,
                static_cast<StringVariable*>(name)->value().c_str(),
                static_cast<StringVariable*>(type)->value().c_str(),
                static_cast<StringVariable*>(value)->value().c_str());
    }

    // Node Commands

    static GraphCommand_AddNode* AddNode(GraphEntity* graph, const Variables& variables)
    {
        IVariable* label = getVariable("label", RD_STRING, variables);
        if (label == NULL)
            return NULL;

        return new GraphCommand_AddNode(graph, static_cast<StringVariable*>(label)->value().c_str());
    }

    static GraphCommand_RemoveNode* RemoveNode(GraphEntity* graph, const Variables& variables)
    {
        IVariable* id = getVariable("id", RD_INT, variables);
        if (id == NULL)
            return NULL;

        return new GraphCommand_RemoveNode(graph, static_cast<IntVariable*>(id)->value());
    }

    static GraphCommand_SetNodeAttribute* SetNodeAttribute(GraphEntity* graph, const Variables& variables)
    {
        IVariable* id = getVariable("id", RD_INT, variables);
        IVariable* name = getVariable("name", RD_STRING, variables);
        IVariable* type = getVariable("type", RD_STRING, variables);
        IVariable* value = getVariable("value", RD_STRING, variables);
        if (id == NULL || name == NULL || type == NULL || value == NULL)
            return NULL;

        return new GraphCommand_SetNodeAttribute(graph,
                static_cast<IntVariable*>(id)->value(),
                static_cast<StringVariable*>(name)->value().c_str(),
                static_cast<StringVariable*>(type)->value().c_str(),
                static_cast<StringVariable*>(value)->value().c_str());
    }

    // Edge Commands

    static GraphCommand_AddEdge* AddEdge(GraphEntity* graph, const Variables& variables)
    {
        IVariable* src = getVariable("src", RD_INT, variables);
        IVariable* dst = getVariable("dst", RD_INT, variables);
        if (src == NULL || dst == NULL)
            return NULL;

        return new GraphCommand_AddEdge(graph,
                static_cast<IntVariable*>(src)->value(),
                static_cast<IntVariable*>(dst)->value());
    }

    static GraphCommand_RemoveEdge* RemoveEdge(GraphEntity* graph, const Variables& variables)
    {
        IVariable* id = getVariable("id", RD_INT, variables);
        if (id == NULL)
            return NULL;

        return new GraphCommand_RemoveEdge(graph, static_cast<IntVariable*>(id)->value());
    }

    static GraphCommand_SetEdgeAttribute* SetEdgeAttribute(GraphEntity* graph, const Variables& variables)
    {
        IVariable* id = getVariable("id", RD_INT, variables);
        IVariable* name = getVariable("name", RD_STRING, variables);
        IVariable* type = getVariable("type", RD_STRING, variables);
        IVariable* value = getVariable("value", RD_STRING, variables);
        if (id == NULL || name == NULL || type == NULL || value == NULL)
            return NULL;

        return new GraphCommand_SetEdgeAttribute(graph,
                static_cast<IntVariable*>(id)->value(),
                static_cast<StringVariable*>(name)->value().c_str(),
                static_cast<StringVariable*>(type)->value().c_str(),
                static_cast<StringVariable*>(value)->value().c_str());
    }

    static IVariable* getVariable(const char* name, VariableType type, const Variables& variables)
    {
        IVariable* var = variables.get(name);
        if (var == NULL || var->type() != type)
        {
            LOG("[COMMAND] Couldn't find required variable '%s'!\n", name);
            return NULL;
        }
        return var;
    }
};
