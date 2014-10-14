#pragma once

#include <raindance/Raindance.hh>
#include <raindance/Core/Debug.hh>

#include "Pack.hh"

#include "Core/Console.hh"
#include "Core/Window.hh"

#include "Entities/MVC.hh"

#include "Visualizers/Space/SpaceVisualizer.hh"
#include "Visualizers/World/WorldVisualizer.hh"

#ifndef EMSCRIPTEN
# include "Visualizers/Particles/ParticleVisualizer.hh"
# include "Visualizers/Stream/StreamVisualizer.hh"
# include "Visualizers/Cloud/CloudVisualizer.hh"
# include "Visualizers/Mesh/MeshVisualizer.hh"
#endif

class Graphiti : public RainDance
{
public:
    Graphiti()
    {
        m_HUD = NULL;
        m_Console = NULL;

        m_Started = false;
    }

    virtual ~Graphiti()
    {
    }

    virtual void create(int argc, char** argv)
    {
        RainDance::create(argc, argv);

        if (m_Console == NULL)
            m_Console = new GraphitiConsole(argc, argv);
    }

    virtual void destroy()
    {
        SAFE_DELETE(m_HUD);

        SAFE_DELETE(m_Console);

        RainDance::destroy();

        ResourceManager::getInstance().dump();
    }

    virtual void initialize()
    {
        m_Console->initialize();
    }

    void createWindow(const char* title, int width, int height)
    {
        if (width <= 0 || height <= 0)
        {
            width = glutGet(GLUT_SCREEN_WIDTH);
            height = glutGet(GLUT_SCREEN_HEIGHT);
        }

        auto id = m_WindowManager.add(new GraphitiWindow(title, width, height));
        m_WindowManager.bind(id);
    }

    EntityManager::ID createEntity(const char* type)
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

    bool createVisualizer(const char* name)
    {
        Entity* entity = m_EntityManager.active();
        if (entity == NULL)
            return false;

        std::string sname = name;

        auto window =  static_cast<GraphitiWindow*>(m_WindowManager.active());
        Viewport viewport(glm::vec2(0, 0), glm::vec2(window->width(), window->height()));

        EntityVisualizer* visualizer = NULL;

        if (sname == "space")
            visualizer = new SpaceVisualizer();
        else if (sname == "world")
            visualizer = new WorldVisualizer();
#ifndef EMSCRIPTEN
        else if (sname == "particles")
            visualizer = new ParticleVisualizer();
        else if (sname == "cloud")
            visualizer = new CloudVisualizer();
        else if (sname == "mesh")
            visualizer = new MeshVisualizer();
        else if (sname == "stream")
            visualizer = new StreamVisualizer();
#endif

        if (visualizer != NULL && visualizer->bind(viewport, entity))
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
        m_HUD = new HUD();
        m_HUD->buildScriptWidgets(m_Console);
        m_HUD->bind(m_EntityManager.active()->context());

        for (auto it : m_EntityManager.elements())
        {
            // Initialize clocks
            it.second->context()->clock().reset();
            it.second->context()->sequencer().clock().reset();
            it.second->context()->sequencer().clock().pause();

            it.second->context()->messages().process();
        }

        // NOTE : This releases the Global Interpreter Lock and allows the python threads to run.
        m_Console->begin();

        run();

        m_Console->end();
    }

    inline void screenshot(const char* filename)
    {
        m_WindowManager.active()->screenshot(std::string(filename), 1);
    }

    // ---------- Window Events ----------

    void reshape(int width, int height)
    {
        // TODO : Use glutGetWindow to forward to the right window.
        m_WindowManager.active()->reshape(width, height);

        for (auto e : m_EntityManager.elements())
        for (auto c : e.second->controllers())
            c->reshape(width, height);

        if (m_HUD != NULL)
            m_HUD->reshape(width, height);
    }

    void draw()
    {
        Geometry::beginFrame();

        m_WindowManager.active()->draw(&m_Context);

        if (m_WindowManager.active()->getScreenshotFactor() <= 0)
            m_HUD->draw(&m_Context);

        finish();

        Geometry::endFrame();
    }

    void idle()
    {
        if (!m_Started)
        {
            if (m_Console)
            {
                IScript* script = m_Console->getScript("#started");
                if (script)
                    m_Console->execute(script);
            }

            m_Started = true;
        }

        m_Console->idle(context().clock());

        if (m_EntityManager.active() != NULL)
            m_EntityManager.active()->context()->sequencer().play();

        m_HUD->idle();

        for (auto e : m_EntityManager.elements())
        {
            for (auto c : e.second->controllers())
                c->idle();
            for (auto v : e.second->views())
                v->idle();
        }

        if (m_EntityManager.active() != NULL)
            m_EntityManager.active()->context()->messages().process();

        postRedisplay();
    }

    void keyboard(unsigned char key, int x, int y)
    {
        // TODO : Use glutGetWindow to forward to the right window.
        (void) x;
        (void) y;

        m_HUD->onKeyboard(key, Controller::KEY_DOWN);

        if (key == 'f')
        {
            m_WindowManager.active()->fullscreen();
        }
        else if (key == 'n')
        {
            static_cast<GraphitiWindow*>(m_WindowManager.active())->selectNextVisualizer();
        }
        else if (key == 'm')
        {
            Geometry::getMetrics().dump();
            Geometry::getMetrics().reset();
            ResourceManager::getInstance().dump();
        }
    #ifndef EMSCRIPTEN
        else if (key == 's')
            m_WindowManager.active()->screenshot(std::string("hd-shot.tga"), 4);
    #endif
        else
        {
            auto window = static_cast<GraphitiWindow*>(m_WindowManager.active());
            window->getActiveVisualizer()->controller()->onKeyboard(key, Controller::KEY_DOWN);
        }
    }

    void keyboardUp(unsigned char key, int x, int y)
    {
        (void) x;
        (void) y;

        m_HUD->onKeyboard(key, Controller::KEY_UP);

        auto window = static_cast<GraphitiWindow*>(m_WindowManager.active());
        window->getActiveVisualizer()->controller()->onKeyboard(key, Controller::KEY_UP);
    }

    void special(int key, int x, int y)
    {
        (void) x;
        (void) y;

        m_HUD->onSpecial(key, Controller::KEY_DOWN);

        for (auto e : m_EntityManager.elements())
        for (auto c : e.second->controllers())
            c->onSpecial(key, Controller::KEY_DOWN);
    }

    void specialUp(int key, int x, int y)
    {
        (void) x;
        (void) y;

        m_HUD->onSpecial(key, Controller::KEY_UP);

        auto window = static_cast<GraphitiWindow*>(m_WindowManager.active());
        window->getActiveVisualizer()->controller()->onSpecial(key, Controller::KEY_UP);
    }

    void mouse(int button, int state, int x, int y)
    {
        m_HUD->mouse(button, state, x, y);

        if (m_HUD->getWidgetPick() == NULL)
        {
            auto window = static_cast<GraphitiWindow*>(m_WindowManager.active());
            window->getActiveVisualizer()->controller()->mouse(button, state, x, y);
        }
    }

    void motion(int x, int y)
    {
        m_HUD->motion(x, y);

        auto window = static_cast<GraphitiWindow*>(m_WindowManager.active());
        window->getActiveVisualizer()->controller()->motion(x, y);
    }

    void registerScript(const char* name, const char* source)
    {
        m_Console->registerScript(new StaticScript(std::string(name), std::string(source)));

        if (m_HUD != NULL)
        {
            m_HUD->buildScriptWidgets(m_Console);
            // NOTE : Hack ! script menu needs to be reshaped
            m_HUD->reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        }
    }

    void unregisterScript(const char* name)
    {
        IScript* script = m_Console->getScript(name);
        if (script == NULL)
        {
            LOG("[API] Script <%s> not found!\n", name);
            return;
        }

        m_Console->unregisterScript(script);

        if (m_HUD != NULL)
        {
            m_HUD->buildScriptWidgets(m_Console);
            // NOTE : Hack ! Script menu needs to be reshaped
            m_HUD->reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        }
    }

    Job::ID addJob(const char* name, float period)
    {
        IScript* script = m_Console->getScript(name);
        if (script == NULL)
        {
            LOG("[API] Can't create job, script <%s> not found!\n", name);
            return 0;
        }

        return m_Console->addJob(script, period);
    }

    void removeJob(Job::ID id)
    {
        m_Console->removeJob(id);
    }

    // Accessors

    inline EntityManager& entities() { return m_EntityManager; }
    inline EntityVisualizerManager& visualizers() { return m_VisualizerManager; }

private:
    GraphitiConsole* m_Console;

    EntityManager m_EntityManager;
    EntityVisualizerManager m_VisualizerManager;

    HUD* m_HUD;

    bool m_Started;
    std::string m_ScreenshotFilename;
};
