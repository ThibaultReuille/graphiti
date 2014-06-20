#pragma once

#include "Graphiti.hh"

Gaia g_Gaia;

namespace API {
extern "C" {

	void create_with_args(int argc, char** argv)
    {
		LOG("[API] create(%d, %p)\n", argc, argv);
		g_Gaia.create(argc, argv);
	}

    void create()
    {
        LOG("[API] create()\n");
        g_Gaia.create(0, NULL);
    }

	void initialize()
	{
		LOG("[API] initialize()\n");
		g_Gaia.initialize();
	}

	void start(const char* view)
	{
		LOG("[API] start('%s')\n", view);
		g_Gaia.start(view);
	}

	void destroy()
	{
		LOG("[API] destroy()\n");
		g_Gaia.destroy();
	}

	void screenshot(const char* filename)
	{
		LOG("[API] screenshot(%s)\n", filename);
		g_Gaia.screenshot(filename);
	}

	// ----- Entities -----

	Entity::ID createEntity(const char* type)
	{
	    LOG("[API] createEntity('%s')\n", type);
	    return g_Gaia.getEntityManager().create(type);
	}

	void bindEntity(Entity::ID id)
	{
	    LOG("[API] bindEntitiy(%lu)\n", id);
	    g_Gaia.getEntityManager().bind(id);
	}

	void send(Entity::ID id, const Variables& input, Variables& output)
	{
	    LOG("[API] send(%p, %p)\n", &input, &output);
	    g_Gaia.getEntityManager().entity(id)->send(input, output);
	}

	void destroyEntity(Entity::ID id)
	{
	    LOG("[API] destroyEntity('%lu')\n", id);
	    return g_Gaia.getEntityManager().destroy(id);
	}

	// ----- Scripts -----

	void registerScript(const char* name, const char* source)
	{
		LOG("[API] registerScript('%s', '%s')\n", name, source);
		g_Gaia.registerScript(name, source);
	}

	void unregisterScript(const char* name)
	{
		LOG("[API] unregisterScript('%s')\n", name);
		g_Gaia.unregisterScript(name);
	}
}
}
