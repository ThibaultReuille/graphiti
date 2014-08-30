#pragma once

#include <raindance/Core/Context.hh>

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

		bool execute(IScript* script) { std::cout << "Console : Couldn't execute script \"" << script->name() << "\" !" << std::endl; return false; }
# endif
#endif
	void notify(IMessage* message)
	{
		IMessage::Type type = message->type();

		if (type != IMessage::SCRIPT)
			return;

		ScriptMessage* msg = static_cast<ScriptMessage*>(message);

		IScript* script = getScript(msg->Name);
		if (script == NULL)
		{
			std::cout << "Console : Couldn't find script \"" << msg->Name << "\" !" << std::endl;
			return;
		}
		else
			execute(script);
	}
};
