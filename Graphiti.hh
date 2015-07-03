#pragma once

#include <raindance/Raindance.hh>
#include <raindance/Core/Debug.hh>

#include <graphiti/Pack.hh>

#include <graphiti/Entities/MVC.hh>

#include <graphiti/Core/Console.hh>
#include <graphiti/Core/Window.hh>
#include <graphiti/Core/Logo.hh>
#include <graphiti/Core/Shell.hh>

#include <raindance/Core/Interface/Documents/Timeline.hh>

#include <graphiti/Visualizers/Space/SpaceVisualizer.hh>
#include <graphiti/Visualizers/Network/NetworkVisualizer.hh>
#include <graphiti/Visualizers/World/WorldVisualizer.hh>

#ifndef EMSCRIPTEN
# include <graphiti/Visualizers/Stream/StreamVisualizer.hh>
# include <graphiti/Visualizers/Cloud/CloudVisualizer.hh>
#endif

#include <graphiti/Documents/WorldMap.hh>
#include <graphiti/Documents/Globe.hh>
#include <graphiti/Documents/TimeSeries.hh>

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

    virtual void createShell()
    {
        auto window = static_cast<GLWindow*>(windows().active());

        auto textarea = new TextArea();
        textarea->style().Align = Document::Style::CENTER;
        textarea->style().Top = Document::Length(Document::Length::PIXELS, -44);
        textarea->style().Near = Document::Length(Document::Length::PIXELS, 1.0);  
        textarea->style().Width = Document::Length(Document::Length::PERCENTS, 0.60);
        textarea->style().Height = Document::Length(Document::Length::PERCENTS, 0.15);
        textarea->style().BackgroundColor = glm::vec4(BLACK, 0.0);
        textarea->style().Pickable = false;
        window->body().addElement(textarea);

        auto shell = new GraphitiShell();
        shell->bind(m_Console);
        shell->attach(textarea);
        shell->style().Align = Document::Style::CENTER;
        shell->style().Top = Document::Length(Document::Length::PIXELS, -10);  
        shell->style().Near = Document::Length(Document::Length::PIXELS, 1.0);  
        shell->style().Width = Document::Length(Document::Length::PERCENTS, 0.60);
        shell->style().Height = Document::Length(Document::Length::PIXELS, 34);
        shell->style().BackgroundColor = glm::vec4(HEX_COLOR(0x2B3856), 0.75);
        window->body().addElement(shell);
        window->focus(shell);
    }

    virtual void createLogo()
    {
        auto window = static_cast<GLWindow*>(windows().active());

        auto logo = new Logo();
        logo->style().Align = Document::Style::RIGHT;
        logo->style().Left = Document::Length(Document::Length::PIXELS, -10);
        logo->style().Top = Document::Length(Document::Length::PIXELS, -10);
        logo->style().Near = Document::Length(Document::Length::PIXELS, 1.0);  
        logo->style().Width = Document::Length(Document::Length::PIXELS, 64);
        logo->style().Height = Document::Length(Document::Length::PIXELS, 64);
        logo->style().BackgroundColor = glm::vec4(WHITE, 0.5);
        window->body().addElement(logo);
    }

    virtual void createTimeline()
    {
        auto window = static_cast<GLWindow*>(windows().active());

        auto doc = new Timeline();
        doc->style().Align = Document::Style::RIGHT;
        doc->style().Left = Document::Length(Document::Length::PIXELS, -10);
        doc->style().Top = Document::Length(Document::Length::PIXELS, -300);
        doc->style().Near = Document::Length(Document::Length::PIXELS, 1.0);  
        doc->style().Width = Document::Length(Document::Length::PIXELS, 300);
        doc->style().Height = Document::Length(Document::Length::PIXELS, 100);
        doc->style().BackgroundColor = glm::vec4(BLACK, 0.1);
        
        auto entity = m_EntityManager.active();
        auto sequencer = entity->context()->sequencer();

        // doc->bindTrack(sequencer.track("command"));
        // doc->bind(sequencer.track("command")->clock());
        // context()->clock().bind(&sequencer.track("command")->clock());

        window->body().addElement(doc);
    }

    virtual void createGlobe()
    {
        auto window = static_cast<GLWindow*>(windows().active());

        auto doc = new Globe();
        doc->style().Align = Document::Style::RIGHT;
        doc->style().Near = Document::Length(Document::Length::PIXELS, 1.0);  
        doc->style().Left = Document::Length(Document::Length::PIXELS, -10);
        doc->style().Top = Document::Length(Document::Length::PIXELS, -10 - 64 - 10 - 220 -10);
        doc->style().Width = Document::Length(Document::Length::PIXELS, 300);
        doc->style().Height = Document::Length(Document::Length::PIXELS, 300);
        doc->style().BackgroundColor = glm::vec4(HEX_COLOR(0x111111), 0.90);
        
        window->body().addElement(doc);
    }

    virtual void createTimeSeries()
    {
        auto window = static_cast<GLWindow*>(windows().active());

        auto doc = new TimeSeries();
        doc->style().Align = Document::Style::RIGHT;
        doc->style().Left = Document::Length(Document::Length::PIXELS, -10);
        doc->style().Width = Document::Length(Document::Length::PIXELS, 300);
        doc->style().Height = Document::Length(Document::Length::PIXELS, 100);
        doc->style().Near = Document::Length(Document::Length::PIXELS, 1.0);
        doc->style().Top = Document::Length(Document::Length::PIXELS, -10 - 64 - 10 - 220 -10 - 300 - 10);
        doc->style().BackgroundColor = glm::vec4(HEX_COLOR(0x111111), 0.90);

        window->body().addElement(doc);
    }

    virtual EntityVisualizer* createSpaceVisualizer()
    {
        auto window = static_cast<GLWindow*>(windows().active());

        auto visualizer = new SpaceVisualizer();
        visualizer->style().Width = Document::Length(Document::Length::PERCENTS, 1.0);
        visualizer->style().Height = Document::Length(Document::Length::PERCENTS, 1.0);
        window->body().addElement(visualizer);

        return visualizer;
    }

    virtual EntityVisualizer* createNetworkVisualizer()
    {
        auto window = static_cast<GLWindow*>(windows().active());

        auto visualizer = new NetworkVisualizer();
        visualizer->style().Width = Document::Length(Document::Length::PERCENTS, 1.0);
        visualizer->style().Height = Document::Length(Document::Length::PERCENTS, 1.0);
        window->body().addElement(visualizer);

        return visualizer;
    }

    virtual EntityVisualizer* createWorldVisualizer()
    {
        auto window = static_cast<GLWindow*>(windows().active());

        auto doc = new WorldMap();
        doc->style().Align = Document::Style::RIGHT;
        doc->style().Left = Document::Length(Document::Length::PIXELS, -10);
        doc->style().Top = Document::Length(Document::Length::PIXELS, -10 - 64 - 10);
        doc->style().Near = Document::Length(Document::Length::PIXELS, 1.0);
        doc->style().Width = Document::Length(Document::Length::PIXELS, 300);
        doc->style().Height = Document::Length(Document::Length::PIXELS, 220);
        doc->style().BackgroundColor = glm::vec4(1.0, 1.0, 1.0, 0.75);
        window->body().addElement(doc);

        return NULL;
    }

    virtual EntityVisualizer* createCloudVisualizer()
    {
        auto window = static_cast<GLWindow*>(windows().active());

        auto visualizer = new CloudVisualizer();
        visualizer->style().Width = Document::Length(Document::Length::PERCENTS, 1.0);
        visualizer->style().Height = Document::Length(Document::Length::PERCENTS, 1.0);
        window->body().addElement(visualizer);

        return visualizer;
    }

    virtual EntityVisualizer* createStreamVisualizer()
    {
        auto window = static_cast<GLWindow*>(windows().active());

        auto visualizer = new StreamVisualizer();
        visualizer->style().Align = Document::Style::RIGHT;
        visualizer->style().Left = Document::Length(Document::Length::PIXELS, -10);
        visualizer->style().Width = Document::Length(Document::Length::PERCENTS, .33);
        visualizer->style().Height = Document::Length(Document::Length::PERCENTS, .33);
        window->body().addElement(visualizer);

        return visualizer;
    }

    virtual bool createVisualizer(const char* name)
    {
        Entity* entity = m_EntityManager.active();
        if (entity == NULL)
            return false;

        std::string sname = name;

        EntityVisualizer* visualizer = NULL;

        if (sname == "logo")
        {
            createLogo();
            return true;
        }
        else if (sname == "shell")
        {
            createShell();
            return true;
        }
        else if (sname == "timeline")
        {
            createTimeline();
            return true;
        }
        else if (sname == "timeseries")
        {
            createTimeSeries();
            return true;
        }
        else if (sname == "globe")
        {
            createGlobe();
            return true;
        }
        else if (sname == "space")
            visualizer = createSpaceVisualizer();
        else if (sname == "network")
            visualizer = createNetworkVisualizer();
        else if (sname == "world")
            visualizer = createWorldVisualizer();
        else if (sname == "cloud")
            visualizer = createCloudVisualizer();
        else if (sname == "stream")
            visualizer = createStreamVisualizer();

        auto window = static_cast<GLWindow*>(windows().active());
        auto viewport = window->getViewport();

        // TODO: Rename "bind" to "attach"
        if (visualizer != NULL && visualizer->bind(viewport, entity))
            return true;
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

    virtual void request(EntityManager::ID id, const Variables& input, Variables& output)
    {
         auto window = static_cast<GLWindow*>(windows().active());

         window->body().getElements()[id]->request(input, output);
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
