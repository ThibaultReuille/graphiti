#pragma once

#include <Python.h>

#include "API/C.hh"
#include "API/Graph.hh"

#define PROTECT_PARSE(code)                      \
    if (!(code))                                 \
    {                                            \
        LOG("[PYTHON] Erroneous API call !\n");  \
        return Py_BuildValue("");                \
    }                                            \

static PyObject* convertVariableToPyObject(IVariable* attribute)
{
	PyObject* result = NULL;
	PyObject* item = NULL;

	switch(attribute->type())
	{

	case RD_STRING:
		result = PyBytes_FromString(static_cast<StringVariable*>(attribute)->value().c_str());
		break;

	case RD_INT:
		result = PyLong_FromLong(static_cast<IntVariable*>(attribute)->value());
		break;

	case RD_FLOAT:
		result = PyFloat_FromDouble((double)static_cast<FloatVariable*>(attribute)->value());
		break;

	case RD_BOOLEAN:
		result = PyBool_FromLong(static_cast<BooleanVariable*>(attribute)->value() ? 1 : 0);
		break;

	case RD_VEC2:
		result = PyList_New(2);
		if (result)
		{
			glm::vec2 value = static_cast<Vec2Variable*>(attribute)->value();
			item = PyFloat_FromDouble(value[0]);
			PyList_SET_ITEM(result, 0, item);
			item = PyFloat_FromDouble(value[1]);
			PyList_SET_ITEM(result, 1, item);
		}
		break;

    case RD_VEC3:
        result = PyList_New(3);
        if (result)
        {
            glm::vec3 value = static_cast<Vec3Variable*>(attribute)->value();
            item = PyFloat_FromDouble(value[0]);
            PyList_SET_ITEM(result, 0, item);
            item = PyFloat_FromDouble(value[1]);
            PyList_SET_ITEM(result, 1, item);
            item = PyFloat_FromDouble(value[2]);
            PyList_SET_ITEM(result, 2, item);
        }
        break;

    case RD_VEC4:
        result = PyList_New(4);
        if (result)
        {
            glm::vec4 value = static_cast<Vec4Variable*>(attribute)->value();
            item = PyFloat_FromDouble(value[0]);
            PyList_SET_ITEM(result, 0, item);
            item = PyFloat_FromDouble(value[1]);
            PyList_SET_ITEM(result, 1, item);
            item = PyFloat_FromDouble(value[2]);
            PyList_SET_ITEM(result, 2, item);
            item = PyFloat_FromDouble(value[3]);
            PyList_SET_ITEM(result, 3, item);
        }
        break;

	default:
		std::cerr << "Unknown attribute type !" << std::endl;
		result = Py_BuildValue("");
	}

	return result;
}

static PyObject* convertVariablesToPyDict(Variables* variables)
{
    PyObject* result = PyDict_New();

    for (auto vit : variables->symbols())
    {
        PyObject* key = PyString_FromString(vit.first.c_str());
        PyObject* value = convertVariableToPyObject(vit.second);
        PyDict_SetItem(result, key, value);
    }

    return result;
}

static Variables* convertPyDictToVariables(PyObject* dict)
{
    if (!PyDict_Check(dict))
        return NULL;

    Variables* variables = new Variables();

    PyObject *key, *value;
    Py_ssize_t pos = 0;

    while (PyDict_Next(dict, &pos, &key, &value))
    {
		auto name = std::string(PyString_AsString(key));
        
		IVariable* variable = NULL;

        if (PyString_Check(value) || PyUnicode_Check(value))
        {
            auto var = new StringVariable();
    		var->set(PyString_AsString(value));
    		variable = var;
        }
        else if (PyInt_Check(value) || PyLong_Check(value))
        {
            auto var = new IntVariable();
            var->set(PyLong_AsLong(value));
            variable = var;
        }
        else if (PyBool_Check(value))
        {
            auto var = new BooleanVariable();
            var->set(value == Py_True);
            variable = var;
        }
        else if (PyFloat_Check(value))
        {
            auto var = new FloatVariable();
            var->set(PyFloat_AsDouble(value));
            variable = var;
        }
        // TODO : Implement the other types here
        else
        {
            LOG("[PYTHON] Item '%s' ignored!\n", name.c_str());
            continue;
        }

        variables->set(name, variable);
    }

    return variables;
}

namespace API {
namespace Python {

static PyObject* start(PyObject* self, PyObject* args)
{
	(void) self;
	(void) args;

	API::start();

	return Py_BuildValue("");
}

static PyObject* createWindow(PyObject* self, PyObject* args)
{
	(void) self;

	char* title = NULL;
	int width, height;

	PROTECT_PARSE(PyArg_ParseTuple(args, "sii", &title, &width, &height));

	API::createWindow(title, width, height);
	
	return Py_BuildValue("");
}

static PyObject* createVisualizer(PyObject* self, PyObject* args)
{
	(void) self;

	char* name = NULL;

	PROTECT_PARSE(PyArg_ParseTuple(args, "s", &name));

	API::createVisualizer(name);
	
	return Py_BuildValue("");
}

static PyObject* screenshot(PyObject* self, PyObject* args)
{
	char* filename = NULL;

	(void) self;

	PROTECT_PARSE(PyArg_ParseTuple(args, "s", &filename))

	API::screenshot(filename);

	return Py_BuildValue("");
}

static PyObject* console(PyObject* self, PyObject* args)
{
    (void) self;

    PyObject* dict = NULL;

    PROTECT_PARSE(PyArg_ParseTuple(args, "O", &dict))

    Variables* input = convertPyDictToVariables(dict);
    if (input == NULL)
        return Py_BuildValue("");
    Variables* output = new Variables();

    API::console(*input, *output);

    PyObject* pyOutput = convertVariablesToPyDict(output);

    delete input;
    delete output;

    return pyOutput;
}

// ----- Entities -----

static PyObject* createEntity(PyObject* self, PyObject* args)
{
    (void) self;

    char* type = NULL;

    PROTECT_PARSE(PyArg_ParseTuple(args, "s", &type))

    return PyLong_FromLong(API::createEntity(type));
}

static PyObject* bindEntity(PyObject* self, PyObject* args)
{
    (void) self;

    unsigned long id;

    PROTECT_PARSE(PyArg_ParseTuple(args, "k", &id))

    API::bindEntity(id);

    return Py_BuildValue("");
}

static PyObject* send(PyObject* self, PyObject* args)
{
    (void) self;

    unsigned long id;
    PyObject* dict = NULL;

    PROTECT_PARSE(PyArg_ParseTuple(args, "kO", &id, &dict))

    Variables* input = convertPyDictToVariables(dict);
    if (input == NULL)
        return Py_BuildValue("");
    Variables* output = new Variables();

    API::send(id, *input, *output);

    PyObject* pyOutput = convertVariablesToPyDict(output);

    delete input;
    delete output;

    return pyOutput;
}

static PyObject* destroyEntity(PyObject* self, PyObject* args)
{
    (void) self;

    unsigned long id;

    PROTECT_PARSE(PyArg_ParseTuple(args, "k", &id))

    API::destroyEntity(id);

    return Py_BuildValue("");
}

static PyObject* setAttribute(PyObject* self, PyObject* args)
{
	char* name = NULL;
	char* type = NULL;
	char* value = NULL;

	(void) self;

	PROTECT_PARSE(PyArg_ParseTuple(args, "sss", &name, &type, &value))

	API::setAttribute(name, type, value);

	return Py_BuildValue("");
}

static PyObject* getAttribute(PyObject* self, PyObject* args)
{
    char* key = NULL;

    (void) self;

    PROTECT_PARSE(PyArg_ParseTuple(args, "s", &key))

    IVariable* attribute = API::getAttribute(key);

    if (attribute == NULL)
        return Py_BuildValue("");
    else
    {
        PyObject* result = convertVariableToPyObject(attribute);

        delete attribute;
        //Py_DECREF(result);
        return result;
    }
}

namespace Graph {

// ----- Nodes -----

static PyObject* addNode(PyObject* self, PyObject* args)
{
	char* label = NULL;

	(void)self;

	PROTECT_PARSE(PyArg_ParseTuple(args, "s", &label))

	return PyLong_FromLong(API::Graph::addNode(label));
}
static PyObject* removeNode(PyObject* self, PyObject* args)
{
	Node::ID id;

	(void)self;

	PROTECT_PARSE(PyArg_ParseTuple(args, "k", &id))

	API::Graph::removeNode(id);

	return Py_BuildValue("");
}
static PyObject* tagNode(PyObject* self, PyObject* args)
{
	Node::ID node;
	Node::ID sphere;

	(void)self;

	PROTECT_PARSE(PyArg_ParseTuple(args, "kk", &node, &sphere))

	API::Graph::tagNode(node, sphere);

	return Py_BuildValue("");
}
static PyObject* countNodes(PyObject* self, PyObject* args)
{
	(void)self;
	(void)args;
	return PyLong_FromLong(API::Graph::countNodes());
}
static PyObject* getNodeIDs(PyObject* self, PyObject* args)
{
	PyObject* result = NULL;
	PyObject* item = NULL;

	(void)self;
	(void)args;

	std::vector<Node::ID> ids = API::Graph::getNodeIDs();

	result = PyList_New(ids.size());
	if (result)
	{
		std::vector<Node::ID>::iterator it;
		unsigned int count = 0;
		for (it = ids.begin(); it != ids.end(); ++it)
		{
			item = PyLong_FromLong(*it);
			PyList_SET_ITEM(result, count++, item);
		}
	}

	return result;
}
static PyObject* setNodeLabel(PyObject* self, PyObject* args)
{
	Node::ID id;
	char* label;

	(void)self;

	PROTECT_PARSE(PyArg_ParseTuple(args, "ks", &id, &label))

	API::Graph::setNodeLabel(id, label);

	return Py_BuildValue("");
}
static PyObject* getNodeLabel(PyObject* self, PyObject* args)
{
	Node::ID id;

	(void)self;

	PROTECT_PARSE(PyArg_ParseTuple(args, "k", &id))

	return PyString_FromString(API::Graph::getNodeLabel(id));
}
static PyObject* setNodeAttribute(PyObject* self, PyObject* args)
{
	Node::ID id;
	char* name = NULL;
	char* type = NULL;
	char* value = NULL;

	(void) self;

	PROTECT_PARSE(PyArg_ParseTuple(args, "ksss", &id, &name, &type, &value))

	API::Graph::setNodeAttribute(id, name, type, value);

	return Py_BuildValue("");
}
static PyObject* getNodeAttribute(PyObject* self, PyObject* args)
{
	Node::ID id;
	char* key = NULL;

	(void) self;

	PROTECT_PARSE(PyArg_ParseTuple(args, "ks", &id, &key))

	IVariable* attribute = API::Graph::getNodeAttribute(id, key);

	if (attribute == NULL)
		return Py_BuildValue("");
	else
	{
		PyObject* result = convertVariableToPyObject(attribute);

		delete attribute;
		//Py_DECREF(result);
		return result;
	}
}

// ----- Edges -----

static PyObject* addEdge(PyObject* self, PyObject* args)
{
	Node::ID id1;
	Node::ID id2;

	(void)self;

	PROTECT_PARSE(PyArg_ParseTuple(args, "kk", &id1, &id2))

	return PyLong_FromLong(API::Graph::addEdge(id1, id2));
}
static PyObject* removeEdge(PyObject* self, PyObject* args)
{
	Edge::ID id;

	(void)self;

	PROTECT_PARSE(PyArg_ParseTuple(args, "k", &id))

	API::Graph::removeEdge(id);
	return Py_BuildValue("");
}
static PyObject* countEdges(PyObject* self, PyObject* args)
{
	(void)self;
	(void)args;
	return PyLong_FromLong(API::Graph::countEdges());
}
static PyObject* getEdgeIDs(PyObject* self, PyObject* args)
{
	PyObject* result = NULL;
	PyObject* item = NULL;

	(void)self;
	(void)args;

	std::vector<Edge::ID> ids = API::Graph::getEdgeIDs();

	result = PyList_New(ids.size());
	if (result)
	{
		std::vector<Node::ID>::iterator it;
		unsigned int count = 0;
		for (it = ids.begin(); it != ids.end(); ++it)
		{
			item = PyLong_FromLong(*it);
			PyList_SET_ITEM(result, count++, item);
		}
	}

	return result;
}
static PyObject* getEdgeNode1(PyObject* self, PyObject* args)
{
	Edge::ID id;

	(void)self;

	PROTECT_PARSE(PyArg_ParseTuple(args, "k", &id))

	return PyLong_FromLong(API::Graph::getEdgeNode1(id));
}
static PyObject* getEdgeNode2(PyObject* self, PyObject* args)
{
	Edge::ID id;

	(void)self;

	PROTECT_PARSE(PyArg_ParseTuple(args, "k", &id))

	return PyLong_FromLong(API::Graph::getEdgeNode2(id));
}
static PyObject* setEdgeAttribute(PyObject* self, PyObject* args)
{
	Edge::ID id;
	char* name = NULL;
	char* type = NULL;
	char* value = NULL;

	(void) self;

	PROTECT_PARSE(PyArg_ParseTuple(args, "ksss", &id, &name, &type, &value))

	API::Graph::setEdgeAttribute(id, name, type, value);

	return Py_BuildValue("");
}
static PyObject* getEdgeAttribute(PyObject* self, PyObject* args)
{
	Edge::ID id;
	char* key = NULL;

	(void) self;

	PROTECT_PARSE(PyArg_ParseTuple(args, "ks", &id, &key))

	IVariable* attribute = API::Graph::getEdgeAttribute(id, key);

	if (attribute == NULL)
		return Py_BuildValue("");
	else
	{
		PyObject* result = convertVariableToPyObject(attribute);

		delete attribute;
		//Py_DECREF(result);
		return result;
	}
}

// ----- Spheres -----

static PyObject* addSphere(PyObject* self, PyObject* args)
{
	char* label = NULL;

	(void)self;

	PROTECT_PARSE(PyArg_ParseTuple(args, "s", &label))

	return PyLong_FromLong(API::Graph::addSphere(label));
}

// ----- Helpers -----

static PyObject* addNeighbor(PyObject* self, PyObject* args)
{
	const char* label;
	Node::ID neighbor;

	(void)self;

	PROTECT_PARSE(PyArg_ParseTuple(args, "sk", &label, &neighbor))

	return PyLong_FromLong(API::Graph::addNeighbor(label, neighbor).first);
}

static PyObject* countSelectedNodes(PyObject* self, PyObject* args)
{
	(void) self;
	(void) args;
	return PyLong_FromLong(API::Graph::countSelectedNodes());
}

static PyObject* getSelectedNode(PyObject* self, PyObject* args)
{
	(void) self;
	(void) args;

	unsigned int index;

	PROTECT_PARSE(PyArg_ParseTuple(args, "I", &index))

	return PyLong_FromLong(API::Graph::getSelectedNode(index));
}

// ----- Commands -----

static PyObject* sendCommand(PyObject* self, PyObject* args)
{
    (void) self;
    PyObject* cid;

    char* name = NULL;
    PyObject* dict;
    Timecode timecode;

    PROTECT_PARSE(PyArg_ParseTuple(args, "ksO", &timecode, &name, &dict))

    Variables* vars = convertPyDictToVariables(dict);
    if (vars == NULL)
        return Py_BuildValue("");

    try
    {
        cid = PyLong_FromLong(API::Graph::sendCommand(timecode, name, *vars));
    }
    catch (std::exception& e)
    {
        cid = Py_BuildValue("");
    }

    delete vars;

    return cid;
}

}

// ----- Scripts -----

static PyObject* registerScript(PyObject* self, PyObject* args)
{
    char* name = NULL;
    char* source = NULL;

    (void)self;

    PROTECT_PARSE(PyArg_ParseTuple(args, "ss", &name, &source))

    API::registerScript(name, source);

    return Py_BuildValue("");
}

static PyObject* unregisterScript(PyObject* self, PyObject* args)
{
    char* name = NULL;

    (void)self;

    PROTECT_PARSE(PyArg_ParseTuple(args, "s", &name))

    API::unregisterScript(name);

    return Py_BuildValue("");
}

static PyObject* addJob(PyObject* self, PyObject* args)
{
    (void)self;

    char* name = NULL;
    float period = 0;

    PROTECT_PARSE(PyArg_ParseTuple(args, "sf", &name, &period))

    return PyLong_FromLong(API::addJob(name, period));
}

static PyObject* removeJob(PyObject* self, PyObject* args)
{
    (void)self;
    
    unsigned long id;

    PROTECT_PARSE(PyArg_ParseTuple(args, "k", &id))

    API::removeJob(id);

    return Py_BuildValue("");
}

static PyMethodDef g_Module[] =
{
	{"start",                 API::Python::start,               METH_VARARGS, "Start engine"},
	{"create_window",         API::Python::createWindow,        METH_VARARGS, "Create window"},
	{"create_visualizer",     API::Python::createVisualizer,    METH_VARARGS, "Create visualizer"},
	{"screenshot",            API::Python::screenshot,          METH_VARARGS, "Take a screenshot"},
	{"console",               API::Python::console,             METH_VARARGS, "Send data to console"},
    // ----- Entities -----
    {"create_entity",         API::Python::createEntity,        METH_VARARGS, "Create an entity"},
    {"bind_entity",           API::Python::bindEntity,          METH_VARARGS, "Create an entity"},
    {"send",                  API::Python::send,                METH_VARARGS, "Send message to an entity"},
    {"destroy_entity",        API::Python::destroyEntity,       METH_VARARGS, "Destroy an entity"},
    // ----- Attributes -----
    {"set_attribute",         API::Python::setAttribute,        METH_VARARGS, "Set entity attribute"},
    {"get_attribute",         API::Python::getAttribute,        METH_VARARGS, "Get entity attribute"},
    // ----- Scripts -----
    {"register_script",       API::Python::registerScript,      METH_VARARGS, "Register a script"},
    {"unregister_script",     API::Python::unregisterScript,    METH_VARARGS, "Unregister a script"},
    {"add_job",               API::Python::addJob,              METH_VARARGS, "Add a job"},
    {"remove_job",            API::Python::removeJob,           METH_VARARGS, "Remove a job"},

	// ----- Graph -----

        // ----- Nodes -----
        {"add_node",              API::Python::Graph::addNode,             METH_VARARGS, "Add node to the graph"},
        {"remove_node",           API::Python::Graph::removeNode,          METH_VARARGS, "Remove node from the graph"},
        {"tag_node",              API::Python::Graph::tagNode,             METH_VARARGS, "Tag a node"},
        {"count_nodes",           API::Python::Graph::countNodes,          METH_VARARGS, "Count nodes"},
        {"get_node_ids",          API::Python::Graph::getNodeIDs,          METH_VARARGS, "Get node IDs" },
        {"set_node_label",        API::Python::Graph::setNodeLabel,        METH_VARARGS, "Set node label" },
        {"get_node_label",        API::Python::Graph::getNodeLabel,        METH_VARARGS, "Get node label" },
        {"set_node_attribute",    API::Python::Graph::setNodeAttribute,    METH_VARARGS, "Set node attribute"},
        {"get_node_attribute",    API::Python::Graph::getNodeAttribute,    METH_VARARGS, "Get node attribute"},
        // ----- Edges -----
        {"add_edge",              API::Python::Graph::addEdge,             METH_VARARGS, "Add edge to the graph"},
        {"remove_edge",           API::Python::Graph::removeEdge,          METH_VARARGS, "Remove edge from the graph"},
        {"count_edges",           API::Python::Graph::countEdges,          METH_VARARGS, "Count edges"},
        {"get_edge_ids",          API::Python::Graph::getEdgeIDs,          METH_VARARGS, "Get edge IDs" },
        {"get_edge_node1",        API::Python::Graph::getEdgeNode1,        METH_VARARGS, "Get the edge node #1" },
        {"get_edge_node2",        API::Python::Graph::getEdgeNode2,        METH_VARARGS, "Get the edge node #2" },
        {"set_edge_attribute",    API::Python::Graph::setEdgeAttribute,    METH_VARARGS, "Set edge attribute"},
        {"get_edge_attribute",    API::Python::Graph::getEdgeAttribute,    METH_VARARGS, "Get edge attribute"},
        // ----- Spheres -----
        {"add_sphere",            API::Python::Graph::addSphere,           METH_VARARGS, "Add sphere to the graph"},
        // ----- Helpers -----
        {"add_neighbor",          API::Python::Graph::addNeighbor,         METH_VARARGS, "Add neighbor to a graph node"},
        {"count_selected_nodes",  API::Python::Graph::countSelectedNodes,  METH_VARARGS, "Count selected nodes"},
        {"get_selected_node",     API::Python::Graph::getSelectedNode,     METH_VARARGS, "Get a selected node"},
        // ----- Commands -----
        {"send_command",          API::Python::Graph::sendCommand,         METH_VARARGS, "Send a command"},

	{NULL, NULL, 0, NULL}
};

static void initialize()
{
	Py_InitModule("graphiti", g_Module);
}

}
}
