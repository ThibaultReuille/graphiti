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
    #include "API/C.hh"
    #include "API/Graph.hh"

    int main(void)
    {
    	API::create();
    	API::initialize();

        int canvasWidth, canvasHeight, isFullscreen;
        emscripten_run_script("for (i = 0; i < 5; i++) { document.body.children[i].style.display = 'None'; }");
        emscripten_run_script("document.getElementById('output').style.display ='None'");
        emscripten_run_script("document.getElementById('canvas').style.display ='block'");
        canvasWidth = emscripten_run_script_int("window.innerWidth - 10");
        canvasHeight = emscripten_run_script_int("window.innerHeight - 10");
        API::createWindow("OpenGraphiti : Data Visualization Engine", canvasWidth, canvasHeight);

    	API::start("space");
    	API::destroy();
    }

#endif
