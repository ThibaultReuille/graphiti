#pragma once

#include "Graphiti.hh"
#include "Entities/MVC.hh"

Graphiti* g_Graphiti = NULL;

namespace API {
extern "C" {

	void create(int argc, char** argv)
    {
		LOG("[API] create(%d, %p)\n", argc, argv);
		g_Graphiti = new Graphiti(argc, argv);
	}

	void initialize()
	{
	    LOG("[API] initialize()\n");
		g_Graphiti->initialize();
	}

	void createWindow(const char* title, int width, int height)
	{
		LOG("[API] createWindow('%s', %i, %i);\n", title, width, height);
		g_Graphiti->createWindow(title, width, height);
	}

	void createVisualizer(const char* visualizer)
	{
		LOG("[API] createVisualizer('%s')\n", visualizer);
		g_Graphiti->createVisualizer(visualizer);
	}

	void start()
	{
	    LOG("[API] start()\n");
		g_Graphiti->start();
	}

	void destroy()
	{
	    LOG("[API] destroy()\n");
		SAFE_DELETE(g_Graphiti);
	}

	void screenshot(const char* filename)
	{
	    // LOG("[API] screenshot(%s)\n", filename);
		// TODO : g_Graphiti->screenshot(filename);
	}

	// ----- Entities -----

	EntityManager::ID createEntity(const char* type)
	{
	    LOG("[API] createEntity('%s')\n", type);
	    return g_Graphiti->createEntity(type);
	}

	void bindEntity(EntityManager::ID id)
	{
	    // LOG("[API] bindEntitiy(%lu)\n", id);
	    g_Graphiti->entities().bind(id);
	}

	void send(EntityManager::ID id, const Variables& input, Variables& output)
	{
	    // LOG("[API] send(%p, %p)\n", &input, &output);
	    g_Graphiti->entities().element(id)->send(input, output);
	}

	void destroyEntity(EntityManager::ID id)
	{
	    LOG("[API] destroyEntity('%lu')\n", id);
	    return g_Graphiti->entities().destroy(id);
	}

	// ----- Attributes -----

    void setAttribute(const char* name, const char* type, const char* value)
    {
        // LOG("[API] setAttribute('%s', '%s', '%s')\n", name, type, value);
        return g_Graphiti->entities().active()->setAttribute(name, type, value);
    }

    IVariable* getAttribute(const char* name)
    {
        // LOG("[API] getAttribute('%s')\n", name);
        return g_Graphiti->entities().active()->getAttribute(name);
    }

	// ----- Scripts -----

	void registerScript(const char* name, const char* source)
	{
	    // LOG("[API] registerScript('%s', '%s')\n", name, source);
		g_Graphiti->registerScript(name, source);
	}

	void unregisterScript(const char* name)
	{
	    // LOG("[API] unregisterScript('%s')\n", name);
		g_Graphiti->unregisterScript(name);
	}

	Job::ID addJob(const char* name, float period)
	{
        IScript* script = g_Graphiti->console()->getScript(name);
        if (script == NULL)
        {
            LOG("[API] Can't create job, script <%s> not found!\n", name);
            return 0;
        }

        return g_Graphiti->console()->addJob(script, period);
	}

	void removeJob(Job::ID id)
	{
		g_Graphiti->console()->removeJob(id);
	}
}
}
