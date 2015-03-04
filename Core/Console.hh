#pragma once

#include <raindance/Core/Context.hh>
#include <raindance/Core/Scheduler.hh>

#if EMSCRIPTEN
# include <emscripten.h>
# include <raindance/Core/Console/JavascriptConsole.hh>
#else
# include <Python.h>
# include <raindance/Core/Console/PythonConsole.hh>
#endif

#include <graphiti/Entities/MVC.hh>

class GraphitiConsoleBase : public Entity
{
public:
	GraphitiConsoleBase() : Entity(Entity::CONSOLE) {}

	virtual void send(const Variables& input, Variables& output)
	{
		std::string logtext;
		if (input.getString("log", logtext))
			notifyListeners(new ConsoleMessage(logtext));
	}

	virtual EntityContext* context() { return NULL; }
	virtual EntityModel* model() { return NULL; }
};

#ifdef EMSCRIPTEN

	class GraphitiConsole : public GraphitiConsoleBase, public JavascriptConsole
	{
	public:
		GraphitiConsole(int argc, char** argv) : JavascriptConsole(argc, argv) {}
	};

#else
# ifndef NO_PYTHON

	class GraphitiConsole : public GraphitiConsoleBase, public PythonConsole
	{
	public:
		GraphitiConsole(int argc, char** argv) : PythonConsole(argc, argv) {}
	};

# else

	class GraphitiConsole : public GraphitiConsoleBase
	{
	public:
		GraphitiConsole(int argc, char** argv) { (void) argc; (void) argv; }

		void initialize() {}

		bool execute(IScript* script)
		{
			LOG("[CONSOLE] Couldn't execute script \"%s\" !\n", script->name().c_str());
			return false;
		}
	};

# endif
#endif
