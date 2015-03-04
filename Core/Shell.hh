#pragma once

#include <raindance/Core/Console/Shell.hh>

#include <graphiti/Entities/MVC.hh>
#include <graphiti/Core/Console.hh>

class GraphitiShell : public Shell, EntityListener
{
public:
	GraphitiShell()
	: Shell(), EntityListener()
	{
		m_Console = NULL;
	}

	virtual void bind(GraphitiConsole* console)
	{
		m_Console = console;
		m_Console->listeners().push_back(this);
	}

	virtual void notify(IMessage* message)
	{
		if (message->type() != IMessage::CONSOLE)
			return;

		auto msg = static_cast<ConsoleMessage*>(message);
		this->print(msg->Message);
	}

	void execute(const std::string& command) override
	{
		if (m_Console == NULL)
			print(command);
		else
			m_Console->execute(command);
	}

private:
	GraphitiConsole* m_Console;
};