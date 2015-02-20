#pragma once

#include <raindance/Core/Context.hh>
#include <raindance/Core/Console/Shell.hh>

#if EMSCRIPTEN
# include <emscripten.h>
# include <raindance/Core/Console/JavascriptConsole.hh>
#else
# include <Python.h>
# include <raindance/Core/Console/PythonConsole.hh>
#endif

class GraphitiConsole :
#ifdef EMSCRIPTEN
	public JavascriptConsole, public IListener
{
public:
	GraphitiConsole(int argc, char** argv) : JavascriptConsole(argc, argv) {}
#else
# ifndef NO_PYTHON
	public PythonConsole, public IListener
	{
	public:
		GraphitiConsole(int argc, char** argv) : PythonConsole(argc, argv) {}
# else
	public Console, public IListener
	{
	public:
		GraphitiConsole(int argc, char** argv) { (void) argc; (void) argv; }

		void initialize() {}

		bool execute(IScript* script)
		{
			LOG("[CONSOLE] Couldn't execute script \"%s\" !\n", script->name().c_str());
			return false;
		}
# endif
#endif
	virtual void notify(IMessage* message)
	{
		IMessage::Type type = message->type();
		if (type != IMessage::SCRIPT)
			return;

		auto msg = static_cast<ScriptMessage*>(message);
		IScript* script = getScript(msg->Name);
		if (script == NULL)
		{
			LOG("[CONSOLE] Couldn't find script \"%s\" !", msg->Name.c_str());
			return;
		}

		execute(script);
	}
};
