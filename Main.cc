#define GRAPHITI_VERSION "Alpha"

#ifndef EMSCRIPTEN

	#include "Graphiti.hh"
	#include "API/Python.hh"

	int main(int argc, char** argv)
	{
		LOG("--- Welcome in OpenGraphiti (" GRAPHITI_VERSION ") ---\n");

		// Create gaia context
		API::create_with_args(argc, argv);

		// Initialize python bindings
        API::Python::initialize();

        // Launch starting script
        API::initialize();

        // Destroy everything
		API::destroy();

		return 0;
	}

#else

	#include "Graphiti.hh"
    #include "API/Javascript.hh"

#endif
