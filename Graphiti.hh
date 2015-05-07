#pragma once

#include <raindance/Raindance.hh>
#include <raindance/Core/Debug.hh>

#include <graphiti/Pack.hh>

#include <graphiti/Entities/MVC.hh>

#include <graphiti/Core/Console.hh>
#include <graphiti/Core/Window.hh>
#include <graphiti/Core/Logo.hh>
#include <graphiti/Core/Shell.hh>

#include <graphiti/Visualizers/Space/SpaceVisualizer.hh>
#include <graphiti/Visualizers/Network/NetworkVisualizer.hh>
#include <graphiti/Visualizers/World/WorldVisualizer.hh>

#ifndef EMSCRIPTEN
# include <graphiti/Visualizers/Stream/StreamVisualizer.hh>
# include <graphiti/Visualizers/Cloud/CloudVisualizer.hh>
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
        GLWindow::Settings settings;

        settings.Title = std::string(title);
        settings.Width = width;
        settings.Height = height;

        if (width == 0 || height == 0)
            settings.Fullscreen = true;

        #ifdef RD_OCULUS_RIFT
            settings.Fullscreen = true;
            settings.Monitor = context()->rift()->findMonitor();
        #endif
        
        auto window = new GLWindow(&settings, this);
        add(window);
    }

    virtual void createShell()
    {
        auto window = static_cast<GLWindow*>(windows().active());

        auto shell = new GraphitiShell();
        shell->bind(m_Console);
    
        shell->style().Align = Document::Style::LEFT;
        shell->style().Left = Document::Length(Document::Length::PIXELS, 10);
        shell->style().Top = Document::Length(Document::Length::PIXELS, -10);
        shell->style().Width = Document::Length(Document::Length::PERCENTS, 1.0);
        shell->style().Height = Document::Length(Document::Length::PERCENTS, 0.25);
        
        window->body().addElement(shell);
    }

    virtual void createLogo()
    {
        auto window = static_cast<GLWindow*>(windows().active());

        auto logo = new Logo();
    
        logo->style().Align = Document::Style::RIGHT;
        logo->style().Left = Document::Length(Document::Length::PIXELS, -10);
        logo->style().Top = Document::Length(Document::Length::PIXELS, -10);
        logo->style().Width = Document::Length(Document::Length::PIXELS, 64);
        logo->style().Height = Document::Length(Document::Length::PIXELS, 64);
        
        window->body().addElement(logo);
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
        else if (sname == "cloud")
            visualizer = new CloudVisualizer();
        else if (sname == "stream")
            visualizer = new StreamVisualizer();
#endif

        auto viewport = window->getViewport();
        auto framebuffer = viewport.getFramebuffer();

        // TODO: Rename "bind" to "attach"
        if (visualizer != NULL && visualizer->bind(viewport, entity))
        {
            //visualizer->border().top(10);
            //visualizer->border().color(glm::vec4(PINK, 1.0));

            visualizer->style().Width = Document::Length(Document::Length::PERCENTS, 1.0);
            visualizer->style().Height = Document::Length(Document::Length::PERCENTS, 1.0);

            window->body().addElement(visualizer);
            
            // TODO: Hack to put shell & logo in front
            createLogo();
            createShell();

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
            m_Console->execute(script->source());

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
                c->idle(m_Context);
            for (auto v : e.second->views())
                v->idle(m_Context);
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
    }

    void unregisterScript(const char* name)
    {
        IScript* script = m_Console->getScript(name);
        if (script == NULL)
            return;

        m_Console->unregisterScript(script);
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
