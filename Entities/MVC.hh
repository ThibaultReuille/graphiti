#pragma once

#include <raindance/Core/Context.hh>
#include <raindance/Core/Controller.hh>
#include <raindance/Core/Variables.hh>

// Forward declarations
class Entity;

// Entity Context

class EntityContext : public Context
{
public:
};

// ----- Entity View -----

class EntityView
{
public:
    virtual ~EntityView() = 0;
    virtual const char* name() const = 0;
    virtual void draw() = 0;
    virtual void idle() = 0;
    virtual bool bind(Entity* entity) = 0;
};

EntityView::~EntityView() {}

// ----- Entity Controller -----

class EntityController : public Controller
{
public:
    virtual ~EntityController() = 0;
    virtual void draw() = 0;
    virtual void idle() = 0;
};

EntityController::~EntityController() {}

// ----- Entity Listener -----

class EntityListener : public IListener
{
public:
    virtual ~EntityListener() = 0;

};
EntityListener::~EntityListener() {}

// ----- Entity -----

class Entity
{
public:
    typedef unsigned long ID;

    enum Type
    {
        GRAPH
    };

    Entity(Type type)
    {
        m_Type = type;
    }

    virtual ~Entity()
    {
        for (auto c : controllers())
            delete c;

        for (auto v : views())
            delete v;

        // TODO : Why does it crash ?
        // for (auto l : listeners())
        //    delete l;
    }

    virtual void send(const Variables& input, Variables& output) = 0;

    virtual EntityContext* context() = 0;
    inline std::list<EntityView*>& views() { return m_Views; }
    inline std::list<EntityController*>& controllers() { return m_Controllers; }
    inline std::vector<EntityListener*>& listeners() { return m_Listeners; }

    inline Type type() { return m_Type; }

private:
    Type m_Type;

    std::list<EntityView*> m_Views;
    std::list<EntityController*> m_Controllers;
    std::vector<EntityListener*> m_Listeners;
};

#include "Graph/GraphEntity.hh"

class EntityManager
{
public:
    EntityManager()
    {
        m_NextEntityID = 0;
        m_ActiveEntity = NULL;
    }

    virtual ~EntityManager()
    {
        for (auto e : m_Entities)
            delete e.second;
    }

    Entity::ID create(const char* type)
    {
        std::string stype(type);

        Entity::ID id = m_NextEntityID;

        if (stype == "graph")
            m_Entities[id] = new GraphEntity();
        else
        {
            LOG("[ENTITY] '%s' : Unknown entity type!\n", type);
            throw;
        }

        m_NextEntityID++;
        return id;
    }

    void destroy(Entity::ID id)
    {
        Entity* e = m_Entities[id];
        if (m_ActiveEntity == e)
            m_ActiveEntity = NULL;
        m_Entities.erase(id);
        delete e;
    }

    void bind(Entity::ID id)
    {
        m_ActiveEntity = entity(id);
    }

    // Accessors
    inline Entity* entity(Entity::ID id) { return m_Entities[id]; }
    inline Entity* active() { return m_ActiveEntity; }
    inline std::unordered_map<Entity::ID, Entity*>& entities() { return m_Entities; }

private:
    Entity::ID m_NextEntityID;
    Entity* m_ActiveEntity;
    std::unordered_map<Entity::ID, Entity*> m_Entities;
};
