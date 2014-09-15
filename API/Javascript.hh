#pragma once

#include <emscripten/bind.h>

#include "API/C.hh"
#include "API/Graph.hh"

using namespace emscripten;

namespace Javascript
{
	// NOTE : Emscripten can't bind char* prototypes, we need to use std::string ones instead.

	void createWindow(const std::string& title, int width, int height) { API::createWindow(title.c_str(), width, height); }

	void createView(const std::string& view) { API::createView(view.c_str()); }

	void createEntity(const std::string& type) { API::createEntity(type.c_str()); }

	void setAttribute(const std::string& name, const std::string& type, const std::string& value) { API::setAttribute(name.c_str(), type.c_str(), value.c_str()); }

	void registerScript(const std::string& name, const std::string& source) { API::registerScript(name.c_str(), source.c_str()); }

	void unregisterScript(const std::string& name) { API::unregisterScript(name.c_str()); }

	namespace Graph
	{
		Node::ID addNode(const std::string& label) { return API::Graph::addNode(label.c_str()); }

		void setNodeAttribute(Node::ID id, const std::string& name, const std::string& type, const std::string& value) { API::Graph::setNodeAttribute(id, name.c_str(), type.c_str(), value.c_str()); }

		void setEdgeAttribute(Link::ID id, const std::string& name, const std::string& type, const std::string& value) { API::Graph::setLinkAttribute(id, name.c_str(), type.c_str(), value.c_str()); }

		std::pair<Node::ID, Link::ID> addNeighbor(const std::string& label, Node::ID neighbor) { return API::Graph::addNeighbor(label.c_str(), neighbor); }
	}
}

EMSCRIPTEN_BINDINGS(my_module)
{
	function("create", &API::create);
	function("initialize", &API::initialize);
	function("createWindow", &Javascript::createWindow, allow_raw_pointers());
	function("createView", &Javascript::createView, allow_raw_pointers());
	function("start", &API::start);
	function("destroy", &API::destroy);
	// function("screenshot", &API::screenshot);

	// ----- Entities -----

	function("createEntity", &Javascript::createEntity, allow_raw_pointers());
	function("bindEntity", &API::bindEntity);
	// function("send");
	function("destroyEntity", &API::destroyEntity);
	
	// ----- Attributes -----

    function("setAttribute", &Javascript::setAttribute, allow_raw_pointers());
    // TODO : function("getAttribute");
    
	// ----- Scripts -----

	function("registerScript", &Javascript::registerScript, allow_raw_pointers());
	function("unregisterScript", &Javascript::unregisterScript, allow_raw_pointers());
	function("addJob", &Javascript::addJob, allow_raw_pointers());
	function("removeJob", &Javascript::removeJob);

	// ----- Commands -----

	// TODO : function("sendCommand");
	
	// ----- Graph Entity -----

	function("addNode", &Javascript::Graph::addNode, allow_raw_pointers());
	function("removeNode", &API::Graph::removeNode);
	function("countNodes", &API::Graph::countNodes);
	function("getNodeID", &API::Graph::getNodeID);
	function("getNodeIDs", &API::Graph::getNodeIDs);
	function("setNodeAttribute", &Javascript::Graph::setNodeAttribute, allow_raw_pointers());
	// TODO : function("getNodeAttribute")
	function("countSelectedNodes", &API::Graph::countSelectedNodes);
	function("getSelectedNode", &API::Graph::getSelectedNode);

	function("addEdge", &API::Graph::addLink);
	function("removeEdge", &API::Graph::removeLink);
	function("countEdges", &API::Graph::countLinks);
	function("getEdgeID", &API::Graph::getLinkID);
	function("getEdgeIDs", &API::Graph::getLinkIDs);
	function("getEdgeNode1", &API::Graph::getLinkNode1);
	function("getEdgeNode2", &API::Graph::getLinkNode2);
	function("setEdgeAttribute", &Javascript::Graph::setEdgeAttribute, allow_raw_pointers());
	// TODO : function("getEdgeAttribute")
}
