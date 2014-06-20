#pragma once

#include "Core/Scheduler.hh"
#include "Core/Variables.hh"

enum GraphMessage
{
	// Graph
	NODE_FIRST = IMessage::CUSTOM + 1,

	NODE_SELECTED,
	NODE_UNSELECTED,

	// Navigation
	TARGET_NODE,
};

struct GraphNodeSelectedMessage : public IMessage
{
	GraphNodeSelectedMessage(unsigned long id) : ID(id) {}
	unsigned int type() { return NODE_SELECTED; }
	unsigned long ID;
};
struct GraphNodeUnselectedMessage : public IMessage
{
	GraphNodeUnselectedMessage(unsigned long id) : ID(id) {}
	unsigned int type() { return NODE_UNSELECTED; }
	unsigned long ID;
};

struct GraphTargetNodeMessage : public IMessage
{
	GraphTargetNodeMessage(unsigned long id) : ID(id) {}
	unsigned int type() { return TARGET_NODE; }
	unsigned long ID;
};

