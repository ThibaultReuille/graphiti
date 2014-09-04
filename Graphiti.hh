#pragma once

#include <raindance/Raindance.hh>
#include <raindance/Core/Debug.hh>
#include <raindance/Core/Canvas.hh> // TODO : Should disappear (Move window logic to RainDance.hh)
#include <raindance/Core/Wallpaper.hh>

#include "Pack.hh"

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

# include "Views/Stream/StreamView.hh"
# include "Views/Stream/StreamController.hh"

#endif

class Graphiti : public RainDance
{
public:
    Graphiti()
	{
		m_HUD = NULL;

		m_Started = false;
		m_Screenshot = false;
		m_FullScreen = false;

		m_FrameBuffer = NULL;
		m_PostEffect1 = Canvas::NORMAL;
		m_PostEffect2 = Canvas::NORMAL;
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
		SAFE_DELETE(m_FrameBuffer);

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
		addWindow(title, width, height);
	}

	Entity::ID createEntity(const char* type)
	{
		std::string stype = type;

	    auto id = m_EntityManager.create(type);
	    m_EntityManager.bind(id);

		if (stype == "graph")
		{
		    auto entity = m_EntityManager.entity(id);

		    auto t1 = new Track("command");
		    t1->setExternalClock(&entity->context()->sequencer().clock());
		    t1->setSynchronization(Track::EXTERNAL);
		    entity->context()->sequencer().add(t1);

		    auto t2 = new Track("animation");
		    t2->setSynchronization(Track::INTERNAL);
		    entity->context()->sequencer().add(t2);
				
			entity->context()->messages().addListener(m_Console);
		}

		return id;
	}

	bool createView(const char* view)
	{
	    if (m_EntityManager.active() == NULL)
	    	return false;

	    std::string name = view;

	    if (m_EntityManager.active()->type() == Entity::GRAPH)
	    {
	    	auto entity = static_cast<GraphEntity*>(m_EntityManager.active());

	    	if (name == "space")
	    	{
		    	auto view = new SpaceView();
	            if (!view->bind(entity))
	            {
	                SAFE_DELETE(view);
	                return false;
	            }
	            entity->context()->messages().addListener(view);

	            SpaceController* controller = new SpaceController();
	            controller->bind(static_cast<GraphContext*>(entity->context()), entity->model(), view);
	            entity->controllers().push_back(controller);
	            entity->listeners().push_back(controller);
	            entity->context()->messages().addListener(controller);

	            return true;
	        }
#ifndef EMSCRIPTEN
            else if (name == "world")
            {
                auto view = new WorldView();
                if (!view->bind(entity))
                {
                    SAFE_DELETE(view);
                    return false;
                }
                entity->context()->messages().addListener(view);

                WorldController* controller = new WorldController();
                controller->bind(static_cast<GraphContext*>(entity->context()), view);
                entity->controllers().push_back(controller);
                entity->context()->messages().addListener(controller);

                return true;
            }
            else if (name == "cloud")
            {
                auto view = new CloudView();
                if (!view->bind(entity))
                {
                    SAFE_DELETE(view);
                    return false;
                }
                entity->context()->messages().addListener(view);

                CloudController* controller = new CloudController();
                controller->bind(static_cast<GraphContext*>(entity->context()), view);
                entity->controllers().push_back(controller);
                entity->context()->messages().addListener(controller);

                return true;
            }
            else if (name == "mesh")
            {
                auto view = new MeshView();
                if (!view->bind(entity))
                {
                    SAFE_DELETE(view);
                    return false;
                }
                entity->context()->messages().addListener(view);

                MeshController* controller = new MeshController();
                controller->bind(entity->context(), view);
                entity->controllers().push_back(controller);
                entity->context()->messages().addListener(controller);

                return true;
            }
            else if (name == "particles")
            {
                auto view = new ParticleView();
                if (!view->bind(entity))
                {
                    SAFE_DELETE(view);
                    return false;
                }
                entity->context()->messages().addListener(view);

                ParticleController* controller = new ParticleController();
                controller->bind(static_cast<GraphContext*>(entity->context()), entity->model(), view);
                entity->controllers().push_back(controller);
                entity->context()->messages().addListener(controller);

                return true;
            }
            
#endif
	    }

#ifndef EMSCRIPTEN
	    else if (m_EntityManager.active()->type() == Entity::TIME_SERIES)
	    {
		    auto entity = static_cast<TimeSeriesEntity*>(m_EntityManager.active());
		    
		    if (name == "stream")
		    {
	            auto view = new StreamView();
		        if (!view->bind(entity))
		        {
		            SAFE_DELETE(view);
		            return false;
		        }
		        entity->context()->messages().addListener(view);

		        auto controller = new StreamController();
		        controller->bind(static_cast<TimeSeriesContext*>(entity->context()), view);
		        entity->controllers().push_back(controller);
		        entity->context()->messages().addListener(controller);

		        return true;
	    	}
	    }
#endif
	    
        LOG("[GRAPHITI] Couldn't create view named '%s' !\n", view);
        return false;
	}

	virtual void start()
	{
		auto entity = m_EntityManager.active();
		if (entity == NULL)
		{
			LOG("[GRAPHITI] No active entity!\n");
			return;
		}

		m_HUD = new HUD();
		m_HUD->buildScriptWidgets(m_Console);
		m_HUD->bind(entity->context());

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

#ifndef EMSCRIPTEN
		m_FrameBuffer->bind();
		m_FrameBuffer->setConvolutionEffect(Canvas::NORMAL);
		m_FrameBuffer->bindColorTexture(m_TextureVector[0]);
#endif
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

#ifndef EMSCRIPTEN
		// Final render to screen
		m_FrameBuffer->unbind();
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_FrameBuffer->draw(entity->context(), m_TextureVector[0]);
#endif
		
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
			if (m_Console)
			{
				IScript* script = m_Console->getScript("#started");
				if (script)
					m_Console->execute(script);
			}

			m_Started = true;
		}

		if (m_EntityManager.active() != NULL)
			m_EntityManager.active()->context()->sequencer().play();

		m_HUD->idle();
		
		for (auto e : m_EntityManager.entities())
		{
		    for (auto c : e.second->controllers())
		        c->idle();
		    for (auto v : e.second->views())
		        v->idle();
		}

		if (m_EntityManager.active() != NULL)
        	m_EntityManager.active()->context()->messages().process();

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

	// Accessors

	inline EntityManager& getEntityManager() { return m_EntityManager; }

private:
	GraphitiConsole* m_Console;

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
