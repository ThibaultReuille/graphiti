#pragma once

#include <raindance/Raindance.hh>
#include <raindance/Core/Debug.hh>
#include <raindance/Core/Canvas.hh> // TODO : Should disappear (Move window logic to RainDance.hh)
#include <raindance/Core/Wallpaper.hh>

#include "Core/Console.hh"

#include "Entities/MVC.hh"
#include "Entities/Graph/GraphCommands.hh"

#include "Views/Space/SpaceView.hh"
#include "Views/Space/SpaceController.hh"

#ifndef EMSCRIPTEN

# include "Views/World/WorldView.hh"
# include "Views/World/WorldController.hh"

# include "Views/Cloud/CloudView.hh"
# include "Views/Cloud/CloudController.hh"

# include "Views/Mesh/MeshView.hh"
# include "Views/Mesh/MeshController.hh"

# include "Views/Particles/ParticleView.hh"
# include "Views/Particles/ParticleController.hh"

#endif

class Gaia : public RainDance
{
public:
	Gaia()
	{
		m_GraphConsole = NULL;

		m_HUD = NULL;

		m_Started = false;
		m_Screenshot = false;
		m_FullScreen = false;

		m_FrameBuffer = NULL;
		m_PostEffect1 = Canvas::NORMAL;
		m_PostEffect2 = Canvas::NORMAL;
	}

	virtual ~Gaia()
	{
	}

	virtual void create(int argc, char** argv)
	{
	    RainDance::create(argc, argv);

		if (m_EntityManager.active() == NULL)
		{
		    auto id = m_EntityManager.create("graph");
		    m_EntityManager.bind(id);
		    auto graph = static_cast<GraphEntity*>(m_EntityManager.entity(id));

		    auto t1 = new Track("command");
		    t1->setExternalClock(&graph->context()->sequencer().clock());
		    t1->setSynchronization(Track::EXTERNAL);
		    graph->context()->sequencer().add(t1);

		    auto t2 = new Track("animation");
		    t2->setSynchronization(Track::INTERNAL);
		    graph->context()->sequencer().add(t2);
		}

		if (m_GraphConsole == NULL)
		{
		    auto graph = static_cast<GraphEntity*>(m_EntityManager.active());

			m_GraphConsole = new GraphConsole(argc, argv);
			graph->context()->messages().addListener(m_GraphConsole);
		}
	}

	virtual void destroy()
	{
		SAFE_DELETE(m_HUD);
		SAFE_DELETE(m_FrameBuffer);

		SAFE_DELETE(m_GraphConsole);

        RainDance::destroy();

        ResourceManager::getInstance().dump();
	}

	virtual void initialize()
	{
		m_GraphConsole->initialize();
	}

	virtual void start(const char* view)
	{
		// addWindow("Gaia : Data Visualization Engine", 1024, 728);
		addWindow("Gaia : Data Visualization Engine", 1920, 1080); // TODO : Make resolution adjustable
		auto entity = static_cast<GraphEntity*>(m_EntityManager.active());

		m_HUD = new HUD();
		m_HUD->buildScriptWidgets(m_GraphConsole);
		m_HUD->bind(entity->context());

		addView(view);

		// Initialize clocks
		entity->context()->clock().reset();
		entity->context()->sequencer().clock().reset();
		entity->context()->sequencer().clock().pause();

		entity->context()->messages().process();

		m_FrameBuffer = new Canvas(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		m_FrameBuffer->bind();

		m_TextureVector.add(new Texture("Color 0", glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 4));
		m_TextureVector.add(new Texture("Color 1", glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 4));
		m_TextureVector.add(new Texture("Color 2", glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 4));

		run();
	}

	void addView(const char* view)
	{
	    auto entity = m_EntityManager.active();

	    // TODO : Remove this
	    auto graph = static_cast<GraphEntity*>(entity);

        if (entity != NULL)
        {
            std::string name = std::string(view);

            if (name == "space")
            {
                SpaceView* view = new SpaceView();
                if (!view->bind(entity))
                {
                    SAFE_DELETE(view);
                    return;
                }
                entity->context()->messages().addListener(view);

                SpaceController* controller = new SpaceController();
                controller->bind(static_cast<GraphContext*>(graph->context()), graph->model(), view);
                entity->controllers().push_back(controller);
                entity->listeners().push_back(controller);
                entity->context()->messages().addListener(controller);
            }
#ifndef EMSCRIPTEN
            else if (name == "world")
            {
                WorldView* view = new WorldView();
                if (!view->bind(entity))
                {
                    SAFE_DELETE(view);
                    return;
                }
                entity->context()->messages().addListener(view);

                WorldController* controller = new WorldController();
                controller->bind(static_cast<GraphContext*>(graph->context()), view);
                entity->controllers().push_back(controller);
                entity->context()->messages().addListener(controller);
            }
            else if (name == "cloud")
            {
                CloudView* view = new CloudView();
                if (!view->bind(entity))
                {
                    SAFE_DELETE(view);
                    return;
                }
                entity->context()->messages().addListener(view);

                CloudController* controller = new CloudController();
                controller->bind(static_cast<GraphContext*>(graph->context()), view);
                entity->controllers().push_back(controller);
                entity->context()->messages().addListener(controller);
            }
            else if (name == "mesh")
            {
                MeshView* view = new MeshView();
                if (!view->bind(entity))
                {
                    SAFE_DELETE(view);
                    return;
                }
                entity->context()->messages().addListener(view);

                MeshController* controller = new MeshController();
                controller->bind(entity->context(), view);
                entity->controllers().push_back(controller);
                entity->context()->messages().addListener(controller);
            }
            else if (name == "particles")
            {
                ParticleView* view = new ParticleView();
                if (!view->bind(entity))
                {
                    SAFE_DELETE(view);
                    return;
                }
                entity->context()->messages().addListener(view);

                ParticleController* controller = new ParticleController();
                controller->bind(static_cast<GraphContext*>(graph->context()), graph->model(), view);
                entity->controllers().push_back(controller);
                entity->context()->messages().addListener(controller);
            }
#endif
            else
                LOG("[GAIA] Unknown view name '%s' !\n", view);
        }
	}

	void screenshot(const char* filename)
	{
		m_Screenshot = true;
		m_ScreenshotFilename = std::string(filename);
	}

	// ---------- Window Events ----------

	void reshape(int width, int height)
	{
		m_FrameBuffer->reshape(width, height);
		m_TextureVector.reshape(width, height);

        for (auto e : m_EntityManager.entities())
        for (auto c : e.second->controllers())
            c->reshape(width, height);

		if (m_HUD != NULL)
			m_HUD->reshape(width, height);
	}

	void draw()
	{
	    auto entity = static_cast<GraphEntity*>(m_EntityManager.active());

	    Geometry::beginFrame();

		if (m_Screenshot)
		{
			unsigned int factor = 4;
			m_FrameBuffer->reshape(glutGet(GLUT_WINDOW_WIDTH) * factor, glutGet(GLUT_WINDOW_HEIGHT) * factor);
			m_TextureVector.reshape(glutGet(GLUT_WINDOW_WIDTH) * factor, glutGet(GLUT_WINDOW_HEIGHT) * factor);
		}

		m_FrameBuffer->bind();
		m_FrameBuffer->setConvolutionEffect(Canvas::NORMAL);
		m_FrameBuffer->bindColorTexture(m_TextureVector[0]);

		{
		    glClearColor(0.0, 0.0, 0.0, 0.0);
		    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		    for (auto e : m_EntityManager.entities())
		    for (auto v : e.second->views())
		        v->draw();
		}

		if (m_Screenshot)
		{
			m_FrameBuffer->dump("hd-shot.tga", m_TextureVector[0]);
			m_FrameBuffer->reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
			m_TextureVector.reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
			m_Screenshot = false;
		}

		// Final render to screen
		m_FrameBuffer->unbind();
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_FrameBuffer->draw(entity->context(), m_TextureVector[0]);

		for (auto e : m_EntityManager.entities())
		for (auto c : e.second->controllers())
            c->draw();

		if (!m_Screenshot)
			m_HUD->draw(entity->context());

		finish();

		Geometry::endFrame();
	}

	void idle()
	{
		if (!m_Started)
		{
			if (m_GraphConsole)
			{
				IScript* script = m_GraphConsole->getScript("#started");
				if (script)
					m_GraphConsole->execute(script);
			}

			m_Started = true;
		}

		auto entity = static_cast<GraphEntity*>(m_EntityManager.active());

		entity->context()->sequencer().play();

		m_HUD->idle();

		for (auto e : m_EntityManager.entities())
		{
		    for (auto c : e.second->controllers())
		        c->idle();
		    for (auto v : e.second->views())
		        v->idle();
		}

        entity->context()->messages().process();

		glutPostRedisplay();
	}

	void keyboard(unsigned char key, int x, int y)
	{
		(void) x;
		(void) y;

		m_HUD->onKeyboard(key, Controller::KEY_DOWN);

		if (key == 'f')
		{
		    m_Window->fullscreen();
		}
		else if (key == 'm')
		{
		    Geometry::getMetrics().dump();
		    Geometry::getMetrics().reset();
		    ResourceManager::getInstance().dump();
		}
	#ifndef EMSCRIPTEN
		else if (key == 's')
				m_Screenshot = true;
	#endif
		else
		{
	        for (auto e : m_EntityManager.entities())
	        for (auto c : e.second->controllers())
	            c->onKeyboard(key, Controller::KEY_DOWN);
		}
	}

	void keyboardUp(unsigned char key, int x, int y)
	{
        (void) x;
        (void) y;

        m_HUD->onKeyboard(key, Controller::KEY_UP);

        for (auto e : m_EntityManager.entities())
        for (auto c : e.second->controllers())
            c->onKeyboard(key, Controller::KEY_UP);
	}

	void special(int key, int x, int y)
	{
		(void) x;
		(void) y;

		m_HUD->onSpecial(key, Controller::KEY_DOWN);

		for (auto e : m_EntityManager.entities())
		for (auto c : e.second->controllers())
		    c->onSpecial(key, Controller::KEY_DOWN);
	}

	void specialUp(int key, int x, int y)
	{
	    (void) x;
	    (void) y;

		m_HUD->onSpecial(key, Controller::KEY_UP);

		for (auto e : m_EntityManager.entities())
		for (auto c : e.second->controllers())
		    c->onSpecial(key, Controller::KEY_UP);
	}

	void mouse(int button, int state, int x, int y)
	{
		m_HUD->mouse(button, state, x, y);

		if (m_HUD->getWidgetPick() == NULL)
		{
		    for (auto e : m_EntityManager.entities())
            for (auto c : e.second->controllers())
                c->mouse(button, state, x, y);
		}
	}

	void motion(int x, int y)
	{
		m_HUD->motion(x, y);

		for (auto e : m_EntityManager.entities())
		for (auto c : e.second->controllers())
                c->motion(x, y);
	}

	void registerScript(const char* name, const char* source)
	{
		m_GraphConsole->registerScript(new StaticScript(std::string(name), std::string(source)));

		if (m_HUD != NULL)
		{
			m_HUD->buildScriptWidgets(m_GraphConsole);
			// NOTE : Hack ! script menu needs to be reshaped
			m_HUD->reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		}
	}

	void unregisterScript(const char* name)
	{
	    IScript* script = m_GraphConsole->getScript(name);
	    if (script == NULL)
	    {
	        LOG("[API] Script <%s> not found!\n", name);
	        return;
	    }

		m_GraphConsole->unregisterScript(script);

		if (m_HUD != NULL)
		{
			m_HUD->buildScriptWidgets(m_GraphConsole);
			// NOTE : Hack ! Script menu needs to be reshaped
			m_HUD->reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		}
	}

	// Accessors

	inline EntityManager& getEntityManager() { return m_EntityManager; }

private:
	GraphConsole* m_GraphConsole;

	EntityManager m_EntityManager;

	HUD* m_HUD;

	Canvas* m_FrameBuffer;
	TextureVector m_TextureVector;
	Canvas::ConvolutionEffect m_PostEffect1;
	Canvas::ConvolutionEffect m_PostEffect2;

	bool m_Started;
	bool m_Screenshot;
	std::string m_ScreenshotFilename;
	bool m_FullScreen;
};
