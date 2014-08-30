#pragma once

#include "Graphiti.hh"

Graphiti g_Graphiti;

namespace API {
extern "C" {

	void create_with_args(int argc, char** argv)
    {
		LOG("[API] create(%d, %p)\n", argc, argv);
		g_Graphiti.create(argc, argv);
	}

    void create()
    {
        // LOG("[API] create()\n");
        g_Graphiti.create(0, NULL);
    }

	void initialize()
	{
	    // LOG("[API] initialize()\n");
		g_Graphiti.initialize();
	}

	void createWindow(const char* title, int width, int height)
	{
		// LOG("[API] createWindow('%s', %i, %i);\n", title, width, height);
		g_Graphiti.createWindow(title, width, height);
	}

	void createView(const char* view)
	{
		// LOG("[API] createView('%s')\n", view);
		g_Graphiti.createView(view);
	}

	void start()
	{
	    // LOG("[API] start()\n");
		g_Graphiti.start();
	}

	void destroy()
	{
	    // LOG("[API] destroy()\n");
		g_Graphiti.destroy();
	}

	void screenshot(const char* filename)
	{
	    // LOG("[API] screenshot(%s)\n", filename);
		g_Graphiti.screenshot(filename);
	}

	// ----- Entities -----

	Entity::ID createEntity(const char* type)
	{
	    // LOG("[API] createEntity('%s')\n", type);
	    return g_Graphiti.createEntity(type);
	}

	void bindEntity(Entity::ID id)
	{
	    // LOG("[API] bindEntitiy(%lu)\n", id);
	    g_Graphiti.getEntityManager().bind(id);
	}

	void send(Entity::ID id, const Variables& input, Variables& output)
	{
	    // LOG("[API] send(%p, %p)\n", &input, &output);
	    g_Graphiti.getEntityManager().entity(id)->send(input, output);
	}

	void destroyEntity(Entity::ID id)
	{
	    // LOG("[API] destroyEntity('%lu')\n", id);
	    return g_Graphiti.getEntityManager().destroy(id);
	}

	// ----- Attributes -----

    void setAttribute(const char* name, const char* type, const char* value)
    {
        // LOG("[API] setAttribute('%s', '%s', '%s')\n", name, type, value);

        return g_Graphiti.getEntityManager().active()->setAttribute(name, type, value);
    }

    IVariable* getAttribute(const char* name)
    {
        // LOG("[API] getAttribute('%s')\n", name);

        return g_Graphiti.getEntityManager().active()->getAttribute(name);
    }

	// ----- Scripts -----

	void registerScript(const char* name, const char* source)
	{
	    // LOG("[API] registerScript('%s', '%s')\n", name, source);
		g_Graphiti.registerScript(name, source);
	}

	void unregisterScript(const char* name)
	{
	    // LOG("[API] unregisterScript('%s')\n", name);
		g_Graphiti.unregisterScript(name);
	}
}
}
