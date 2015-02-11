#pragma once

#include <raindance/Raindance.hh>
#include <raindance/Core/Debug.hh>

#include "Pack.hh"

#include "Entities/MVC.hh"

#include "Core/Console.hh"
#include "Core/Window.hh"

#include "Visualizers/Space/SpaceVisualizer.hh"
#include "Visualizers/Network/NetworkVisualizer.hh"
#include "Visualizers/World/WorldVisualizer.hh"

#ifndef EMSCRIPTEN
# include "Visualizers/Particles/ParticleVisualizer.hh"
# include "Visualizers/Stream/StreamVisualizer.hh"
# include "Visualizers/Cloud/CloudVisualizer.hh"

#endif

class Graphiti : public Raindance, public Root
{
public:
    Graphiti(int argc, char** argv)
    : Raindance(argc, argv), m_Console(NULL)
    {
        SAFE_DELETE(m_Console);
        m_Console = new GraphitiConsole(argc, argv);
    }

    virtual void initialize()
    {
        m_Console->initialize();
    }

    virtual ~Graphiti()
    {
        SAFE_DELETE(m_Console);
    }

    virtual void createWindow(const char* title, int width, int height)
    {
        if (width == 0 || height == 0)
        {
            // TODO : Fullscreen mode
            width = 1024;
            height = 728;
        }

        auto window = new GLWindow(title, width, height, this);
        add(window);
    }

    virtual EntityManager::ID createEntity(const char* type)
    {
        std::string stype = type;

        auto id = m_EntityManager.create(type);

        // TODO : Move this to GraphEntity ?
        if (stype == "graph")
        {
            auto entity = m_EntityManager.element(id);

            auto t1 = new Track("command");
            t1->setExternalClock(&entity->context()->sequencer().clock());
            t1->setSynchronization(Track::EXTERNAL);
            entity->context()->sequencer().add(t1);

            auto t2 = new Track("animation");
            t2->setSynchronization(Track::INTERNAL);
            entity->context()->sequencer().add(t2);

            entity->context()->messages().addListener(m_Console);
        }

        m_EntityManager.bind(id);

        return id;
    }

    virtual bool createVisualizer(const char* name)
    {
        Entity* entity = m_EntityManager.active();
        if (entity == NULL)
            return false;

        std::string sname = name;

        auto window = static_cast<GLWindow*>(windows().active());

        EntityVisualizer* visualizer = NULL;
        if (sname == "space")
            visualizer = new SpaceVisualizer();
        else if (sname == "network")
            visualizer = new NetworkVisualizer();
        else if (sname == "world")
            visualizer = new WorldVisualizer();
#ifndef EMSCRIPTEN
        else if (sname == "particles")
            visualizer = new ParticleVisualizer();
        else if (sname == "cloud")
            visualizer = new CloudVisualizer();
        else if (sname == "stream")
            visualizer = new StreamVisualizer();
#endif

        if (visualizer != NULL && visualizer->bind(window->getViewport(), entity))
        {
            m_VisualizerManager.add(visualizer);
            window->addVisualizer(visualizer);
            return true;
        }
        else
        {
            SAFE_DELETE(visualizer);
            LOG("[GRAPHITI] Couldn't create visualizer named '%s' !\n", name);
            return false;
        }
    }

    virtual void start()
    {
        context()->messages().addListener(m_Console);

        for (auto it : m_EntityManager.elements())
        {
            // Initialize clocks & process messages
            it.second->context()->clock().reset();
            it.second->context()->sequencer().clock().reset();
            it.second->context()->sequencer().clock().pause();
            it.second->context()->messages().process();
        }

        // NOTE : This releases the Global Interpreter Lock and allows the python threads to run.
        m_Console->begin();

        IScript* script = m_Console->getScript("#started");
        if (script)
            m_Console->execute(script);

        refreshHUD();

        run();

        m_Console->end();
    }

    void idle() override
    {
        m_Console->idle(context()->clock());

        Raindance::idle();

        if (m_EntityManager.active() != NULL)
            m_EntityManager.active()->context()->sequencer().play();

        for (auto e : m_EntityManager.elements())
        {
            for (auto c : e.second->controllers())
                c->idle();
            for (auto v : e.second->views())
                v->idle();
        }

        m_Context->messages().process();

        // TODO : We shoud align every message on Graphiti.context()
        if (m_EntityManager.active() != NULL)
            m_EntityManager.active()->context()->messages().process();
    }

    void registerScript(const char* name, const char* source)
    {
        auto script = new StaticScript(std::string(name), std::string(source));
        m_Console->registerScript(script);
        refreshHUD();
    }

    void unregisterScript(const char* name)
    {
        IScript* script = m_Console->getScript(name);
        if (script == NULL)
            return;

        m_Console->unregisterScript(script);
        refreshHUD();
    }

    void refreshHUD()
    {
        if (windows().active() != NULL)
        {
            auto window = static_cast<GLWindow*>(windows().active());
            window->hud()->buildScriptWidgets(console());
            window->hud()->reshape(window->getViewport());
        }    
    }

    // ----- Properties -----

    virtual Context* context() { return m_Context; }
    virtual GraphitiConsole* console() { return m_Console; }
    virtual EntityManager& entities() { return m_EntityManager; }
    virtual EntityVisualizerManager& visualizers() { return m_VisualizerManager; }

private:
    GraphitiConsole* m_Console;
    EntityManager m_EntityManager;
    EntityVisualizerManager m_VisualizerManager;
};