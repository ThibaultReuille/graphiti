#pragma once

#include <raindance/Core/Context.hh>
#include <raindance/Core/Controller.hh>
#include <raindance/Core/Variables.hh>
#include <raindance/Core/GUI/View.hh>
#include <raindance/Core/Manager.hh>
#include <raindance/Core/Interface/Document.hh>

// TODO : This should probably all be moved into the Raindance engine

// ------------------------

class Entity;

// ------------------------

class EntityBase
{
public:
    inline Variables& attributes() { return m_Attributes; }

protected:
    Variables m_Attributes;
};


// ------------------------

class EntityContext : public Context
{
public:
};

// ------------------------

class EntityModel : public EntityBase
{
public:
    virtual ~EntityModel() = 0;
};

EntityModel::~EntityModel() {}

// ------------------------

class EntityView : public View, public EntityBase
{
public:
    virtual ~EntityView() = 0;
    virtual const char* name() const = 0;
    virtual IVariable* getAttribute(const std::string& name) = 0;
};

EntityView::~EntityView() {}

// ------------------------

class EntityController : public EntityBase, public Controller
{
public:
    virtual ~EntityController() = 0;
    virtual void draw() {}
    virtual void idle() {}
};

EntityController::~EntityController() {}

// ------------------------

class EntityVisualizer : public EntityBase, public Document
{
public:
    virtual ~EntityVisualizer() {}
    virtual bool bind(const Viewport& viewport, Entity* entity) = 0;
    virtual EntityView* view() { return m_EntityView; }
    virtual EntityController* controller() { return m_EntityController; }

    void draw() override
    {
        auto position = this->position() + glm::vec3(
            this->margin().left() + this->border().left() + this->padding().left(),
            this->margin().bottom() + this->border().bottom() + this->padding().bottom(),
            0);
   
        glEnable(GL_SCISSOR_TEST);
        glViewport(position.x, position.y, this->content().getWidth(), this->content().getHeight());
        glScissor(position.x, position.y, this->content().getWidth(), this->content().getHeight());

        if (view() != NULL)
            view()->draw();
        if (controller() != NULL)
            controller()->draw();   

        glDisable(GL_SCISSOR_TEST);
    }

    void idle() override
    {
        if (view() != NULL)
            view()->idle();
        if (controller() != NULL)
            controller()->idle();  
    }

    void onResize(const Viewport& viewport) override
    {
        if (controller())
           controller()->onResize(viewport);
    } 

    void onKey(int key, int scancode, int action, int mods) override
    {
        if (controller() != NULL)
            controller()->onKey(key, scancode, action, mods);
    }

    void onMouseDown(const glm::vec2& pos) override
    {
        if (controller() != NULL)
            controller()->onMouseDown(pos);
    }

    void onMouseClick(const glm::vec2& pos) override
    { 
        if (controller() != NULL)
            controller()->onMouseClick(pos);
    }

    void onMouseDoubleClick(const glm::vec2& pos) override
    {
        if (controller() != NULL)
            controller()->onMouseDoubleClick(pos);
    }

    void onMouseTripleClick(const glm::vec2& pos) override
    { 
        if (controller() != NULL)
            controller()->onMouseTripleClick(pos);
    }

    void onMouseMove(const glm::vec2& pos, const glm::vec2& dpos) override
    {
        if (controller() != NULL)
            controller()->onMouseMove(pos, dpos);
    }

    void onScroll(double xoffset, double yoffset) override
    {
        // TODO: Route scroll to the active element
        if (controller() != NULL)
            controller()->onScroll(xoffset, yoffset);
    }



protected:
    void set(EntityView* view) { m_EntityView = view; }
    void set(EntityController* controller) { m_EntityController = controller; }

private:
    EntityView* m_EntityView;
    EntityController* m_EntityController;
};

class EntityVisualizerManager : public Manager<EntityVisualizer>
{
public:
    virtual ~EntityVisualizerManager() {}
};

// ------------------------

class EntityListener : public EntityBase, public IListener
{
public:
    virtual ~EntityListener() = 0;
    virtual void onSetAttribute(const std::string& name, VariableType type, const std::string& value)
    { (void) name; (void) type; (void) value; }

};
EntityListener::~EntityListener() {}

// ------------------------

class Entity : public EntityBase
{
public:
    typedef unsigned long ID;

    enum Type
    {
        GRAPH,
        TIME_SERIES,
        CONSOLE
    };

    Entity(Type type) : m_Type(type) {};

    virtual ~Entity()
    {
        for (auto c : controllers())
            delete c;

        for (auto v : views())
            delete v;

        // TODO : Why is that crashing ?
        // for (auto l : listeners())
        //    delete l;
    }

    virtual void send(const Variables& input, Variables& output) = 0;

    void setAttribute(const std::string& name, const std::string& type, const std::string& value)
    {
        VariableType vtype;

        if (type == "float")
            vtype = RD_FLOAT;
        else if (type == "string")
            vtype = RD_STRING;
        else if (type == "int")
            vtype = RD_INT;
        else if (type == "bool")
            vtype = RD_BOOLEAN;
        else if (type == "vec2")
            vtype = RD_VEC2;
        else if (type == "vec3")
            vtype = RD_VEC3;
        else if (type == "vec4")
            vtype = RD_VEC4;
        else
        {
            std::cout << "Unknown attribute type \"" << type << "\" !" << std::endl;
            return;
        }

        unsigned long pos = name.find(":");
        std::string category = name.substr (0, pos);
        std::string sname = name;

        // TODO : Remove 'raindance' attribute namespace whenever possible.
        if (category == "raindance" || category == "graphiti" || category == "og")
        {
            sname = sname.substr(pos + 1);
        }
        else
        {
            model()->attributes().set(sname, vtype, value);
        }

        for (auto l : listeners())
            l->onSetAttribute(sname, vtype, value);
    }

    IVariable* getAttribute(const std::string& name)
    {
        unsigned long pos1 = name.find(":");
        std::string category = name.substr (0, pos1);

        if (category == "graphiti" || category == "og")
        {
            std::string rest = name.substr(pos1 + 1);
            unsigned long pos2 = rest.find(":");
            std::string view = rest.substr(0, pos2);
            rest = rest.substr(pos2 + 1);

            for (auto v : views())
            {
                if (view == std::string(v->name()))
                    return v->getAttribute(rest);
            }
            return NULL;
        }
        else
        {
            IVariable* attribute = model()->attributes().get(name);
            if (attribute)
                return attribute->duplicate();
            else
                return NULL;
        }
    }

    virtual void notifyListeners(IMessage* message) 
    {
        for (auto l : m_Listeners)
            l->notify(message);
    }

    virtual EntityContext* context() = 0;
    virtual EntityModel* model() = 0;
    inline std::list<EntityView*>& views() { return m_Views; }
    inline std::list<EntityController*>& controllers() { return m_Controllers; }
    inline std::vector<EntityListener*>& listeners() { return m_Listeners; }

    inline Type type() const { return m_Type; };

private:
    Type m_Type;
    std::list<EntityView*> m_Views;
    std::list<EntityController*> m_Controllers;
    std::vector<EntityListener*> m_Listeners;
};

#include "Graph/GraphEntity.hh"
#include "TimeSeries/TimeSeriesEntity.hh"

class EntityManager : public Manager<Entity>
{
public:
    virtual ~EntityManager() {}

    virtual ID create(const char* type)
    {
        Entity* entity = NULL;

        std::string stype(type);

        if (stype == "graph")
            entity = new GraphEntity();
        else if (stype == "time_series")
            entity = new TimeSeriesEntity();
        else
        {
            LOG("[ENTITY] '%s' : Unknown entity type!\n", type);
            throw;
        }

        return add(entity);
    }
};

// ------------------------


