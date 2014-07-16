#pragma once

#include <raindance/Core/Headers.hh>
#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/Camera/Controllers.hh>
#include <raindance/Core/Camera/Sequences.hh>
#include <raindance/Core/HUD/Hud.hh>
#include <raindance/Core/Controller.hh>
#include <raindance/Core/Variables.hh>

#include "Entities/MVC.hh"

#include "Entities/Graph/GraphModel.hh"

#include "Views/Space/SpaceWidgets.hh"

class SpaceController : public GraphController
{
public:
	enum ToolMode { POINTER, MARKER };
	enum DemoMode { START, RUN, STOP };

	SpaceController()
	{
	    m_GraphContext = NULL;

		m_ToolMode = POINTER;
		m_HasSelection = false;
		m_HasTarget = false;
		m_HasPick = false;
		m_IsDragging = false;

		m_Menu = new SpaceMenu();
		m_ShowMenu = true;
	}

	virtual ~SpaceController()
	{
		delete m_Menu;
	}

	void bind(GraphContext* context, GraphModel* model, SpaceView* view)
	{
		m_GraphContext = context;
		m_GraphModel = model;
		m_GraphView = view;

		m_FirstPersonCameraController.bind(context, m_GraphView->camera());
		m_FirstPersonCameraController.updateCamera();
		m_SphericalCameraController.bind(context, m_GraphView->camera());
		m_SphericalCameraController.updateCamera();

        m_Menu->getTimeline()->bindTrack(m_GraphContext->sequencer().track("command"));
        m_Menu->getClock()->bind(&m_GraphContext->sequencer().track("command")->clock());
	}

	virtual void reshape(int width, int height)
	{
		m_Menu->reshape(width, height);

		if (m_HasTarget)
			m_SphericalCameraController.reshape(width, height);
		else
			m_FirstPersonCameraController.reshape(width, height);
	}

	virtual void idle()
	{
		updateSelection();

		if (m_HasTarget)
			m_SphericalCameraController.updateCamera();
		else
			m_FirstPersonCameraController.updateCamera();

        g_SpaceResources->ShowNodeLOD = m_Menu->getCheckBox1()->value();
        g_SpaceResources->ShowLinkLOD = m_Menu->getCheckBox2()->value();
        g_SpaceResources->ShowNodeActivity = m_Menu->getCheckBox3()->value();
        g_SpaceResources->ShowEdgeActivity = m_Menu->getCheckBox4()->value();
	}

	virtual void draw()
	{
		if (m_ShowMenu)
			m_Menu->draw(m_GraphContext);
	}

	virtual void onKeyboard(unsigned char key, Controller::KeyEvent event)
	{
		if (event == Controller::KEY_DOWN && key == 9) // TAB
		{
			m_ShowMenu = !m_ShowMenu;
			return;
		}
		else if (event == Controller::KEY_DOWN && key == ' ')
		{
		    if (m_GraphContext->sequencer().clock().state() == Clock::STARTED)
		        m_GraphContext->sequencer().clock().pause();
		    else
		        m_GraphContext->sequencer().clock().start();
		    return;
		}

		if (m_HasTarget)
			m_SphericalCameraController.onKeyboard(key, event);
		else
			m_FirstPersonCameraController.onKeyboard(key, event);
	}

	virtual void onMouseDown(int x, int y)
	{
		m_IsDragging = false;

		if (m_HasTarget)
			m_SphericalCameraController.onMouseDown(x, y);
		else
			m_FirstPersonCameraController.onMouseDown(x, y);
	}

	void updateSelection()
	{
		// NOTE : Has selected node been removed ?
		if (m_HasSelection && !m_GraphView->getNodeMap().containsLocalID(m_SelectedNode))
		{
			m_HasSelection = false;
			m_IsDragging = false;
			m_HasTarget = false;
			m_HasPick = false;
			LOG("Node selection lost!\n");
		}
	}

	virtual void onMouseClick(int x, int y)
	{
		m_IsDragging = false;

		updateSelection();

		IWidget* pickWidget = m_Menu->pickWidget(x, y);
		if (pickWidget != NULL)
		{
			pickWidget->onMouseClick(m_GraphContext->messages(), x, y);
		}
		else
		{
			m_HasPick = m_GraphView->pickNode(x, y, &m_PickNode);
			if (m_HasPick)
			{
				if (m_ToolMode == POINTER)
				{
					if (m_HasSelection)
						m_GraphModel->unselectNode(m_GraphView->getNodeMap().getRemoteID(m_SelectedNode));
					m_GraphModel->selectNode(m_GraphView->getNodeMap().getRemoteID(m_PickNode));
					m_HasSelection = true;
					m_SelectedNode = m_PickNode;
				}
				else if (m_ToolMode == MARKER)
				{
				    SpaceNode::ID id = m_GraphView->getNodeMap().getRemoteID(m_PickNode);
				    SpaceNode* node = static_cast<SpaceNode*>(m_GraphView->getNodes()[id]);

					unsigned int marker = m_Menu->getMarkerWidget()->marker();
					unsigned int mark = node->getMark() != marker ? marker : 0;
					node->setMark(mark);

					std::ostringstream smark;
					smark << mark;
					m_GraphView->onSetNodeAttribute(id, "space:mark", INT, smark.str());
				}

				m_SphericalCameraController.onMouseClick(x, y);
			}
			else
			{
				if (m_HasSelection)
					m_GraphModel->unselectNode(m_GraphView->getNodeMap().getRemoteID(m_SelectedNode));
				m_HasSelection = false;
				m_HasTarget = false;
				m_FirstPersonCameraController.onMouseClick(x, y);
			}
		}
	}

	virtual void onMouseDoubleClick(int x, int y)
	{
		updateSelection();

		if (m_HasSelection)
		{
			m_HasTarget = true;
			m_GraphContext->messages().push(new GraphTargetNodeMessage(m_SelectedNode));

			m_SphericalCameraController.onMouseDoubleClick(x, y);
		}
		else
			m_FirstPersonCameraController.onMouseDoubleClick(x, y);
	}

	virtual void onMouseTripleClick(int x, int y)
	{
		updateSelection();

		if (m_HasTarget)
			m_SphericalCameraController.onMouseTripleClick(x, y);
		else
			m_FirstPersonCameraController.onMouseTripleClick(x, y);
	}

	void dragVertex(Node::ID, int screenX, int screenY)
	{
		updateSelection();

		Ray ray = m_GraphView->camera()->createRay(screenX, screenY);

		glm::vec3 pos = m_GraphView->getNodes()[m_SelectedNode]->getPosition();

		glm::vec3 direction = pos - m_GraphView->camera()->getPosition();
		float length = glm::length(direction);

		m_GraphView->getNodes()[m_SelectedNode]->setPosition(ray.position() + length * ray.direction());

		// NOTE : I don't think we need to force octree update here since we can only drag nodes within the vision field.
	}

	virtual void onMouseMove(int x, int y, int dx, int dy)
	{
		updateSelection();

		if (m_IsDragging)
		{
			dragVertex(m_SelectedNode, x, y);
		}
		else
		{
			m_HasPick = m_GraphView->pickNode(x, y, &m_PickNode);

			if (m_HasPick && m_PickNode == m_SelectedNode && dx < 5 && dy < 5)
			{
				m_IsDragging = true;
				dragVertex(m_SelectedNode, x, y);
			}
			else if (m_HasTarget)
				m_SphericalCameraController.onMouseMove(x, y, dx, dy);
			else
				m_FirstPersonCameraController.onMouseMove(x, y, dx, dy);
		}
	}

	virtual void onSpecial(int key, Controller::KeyEvent event)
	{
		if (event == Controller::KEY_DOWN)
		{
			if (key == GLUT_KEY_F11)
				glutFullScreen();
		}

		if (m_HasTarget)
			m_SphericalCameraController.onSpecial(key, event);
		else
			m_FirstPersonCameraController.onSpecial(key, event);
	}

	void notify(IMessage* message)
	{
		IMessage::Type type = message->type();
		if (type == TARGET_NODE)
		{
			GraphTargetNodeMessage* msg = static_cast<GraphTargetNodeMessage*>(message);

			Scene::Node* node = m_GraphView->getNodes()[msg->ID];

			// NOTE : Find the zoom distance according to the node size so that it always has the same screen size.
			float zoomAngle = M_PI / 20;
			float zoomDistance = static_cast<SpaceNode*>(node)->getScreenSize() / (2 * tan(zoomAngle / 2));

			if (m_HasTarget)
				m_SphericalCameraController.playZoomSequence(node->getPosition(), zoomDistance, 250);
			else
				m_FirstPersonCameraController.sequence(node->getPosition(), zoomDistance, 250);
		}
		else if (type == IMessage::SCRIPT)
		{
			m_HasTarget = false;
		}
		else if (type == IMessage::WIDGET)
		{
			WidgetMessage* msg = static_cast<WidgetMessage*>(message);

			if (msg->Name == "slider1" && msg->Message == "update")
			{
				float value = m_Menu->getSlider1()->value();
				m_GraphView->setNodeSize(5.0f * value);
			}
            else if (msg->Name == "slider2" && msg->Message == "update")
            {
                float value = m_Menu->getSlider2()->value();
                m_GraphView->setEdgeSize(3.0f * value);
            }
            else if (msg->Name == "slider3" && msg->Message == "update")
            {
                float value = m_Menu->getSlider3()->value();
                m_GraphView->setTemperature(20.0f * value * value);
            }
			else if (msg->Message == "pointer")
				m_ToolMode = POINTER;
			else if (msg->Message == "marker")
			{
				if (m_ToolMode == MARKER)
					m_Menu->getMarkerWidget()->next();
				else
					m_ToolMode = MARKER;
			}
			else if (msg->Message == "space:nodes:all")
				m_Menu->getNodeTextWidget()->text().set("All", m_Menu->getFont());
			else if (msg->Message == "space:nodes:shapes+labels")
				m_Menu->getNodeTextWidget()->text().set("Shapes + Labels", m_Menu->getFont());
			else if (msg->Message == "space:nodes:marks+labels")
				m_Menu->getNodeTextWidget()->text().set("Marks + Labels", m_Menu->getFont());
			else if (msg->Message == "space:nodes:shapes+marks")
				m_Menu->getNodeTextWidget()->text().set("Shapes + Marks", m_Menu->getFont());
			else if (msg->Message == "space:nodes:shapes")
				m_Menu->getNodeTextWidget()->text().set("Shapes", m_Menu->getFont());
			else if (msg->Message == "space:nodes:marks")
				m_Menu->getNodeTextWidget()->text().set("Marks", m_Menu->getFont());
			else if (msg->Message == "space:nodes:labels")
				m_Menu->getNodeTextWidget()->text().set("Labels", m_Menu->getFont());
			else if (msg->Message == "space:nodes:off")
				m_Menu->getNodeTextWidget()->text().set("Off", m_Menu->getFont());

			else if (msg->Message == "space:edges:lines")
				m_Menu->getEdgeTextWidget()->text().set("Lines", m_Menu->getFont());
		    else if (msg->Message == "space:edges:widelines")
		        m_Menu->getEdgeTextWidget()->text().set("Wide Lines", m_Menu->getFont());
		    else if (msg->Message == "space:edges:off")
		        m_Menu->getEdgeTextWidget()->text().set("Off", m_Menu->getFont());

			else if (msg->Message == "show spheres")
				m_Menu->getSpheresTextWidget()->text().set("On", m_Menu->getFont());
			else if (msg->Message == "hide spheres")
				m_Menu->getSpheresTextWidget()->text().set("Off", m_Menu->getFont());

			else if (msg->Message == "play")
				m_DemoMode = START;
			else if (msg->Message == "pause")
				m_DemoMode = STOP;
		}
		else if (type == IMessage::SEQUENCER)
		{
		    SequencerMessage* msg = static_cast<SequencerMessage*>(message);

		    if (msg->Message == "update")
		        m_Menu->getTimeline()->setDirty();
		}
	}

	// GraphListener Interface

    virtual void onSetAttribute(const std::string& name, VariableType type, const std::string& value)
    {
        StringVariable vstring;

        if (name == "space:title" && type == STRING)
        {
            vstring.set(value);
            m_Menu->getTitle()->text().set(vstring.value().c_str(), m_Menu->getFont());
        }
    }

    virtual void onAddNode(Node::ID id, const char* label)
    {
        (void) id;
        (void) label;
    }

    virtual void onRemoveNode(Node::ID id)
    {
        (void) id;
    }

    virtual void onSetNodeAttribute(Node::ID id, const std::string& name, VariableType type, const std::string& value)
    {
        (void) id;
        (void) name;
        (void) type;
        (void) value;
    }

    virtual void onSetNodeLabel(Node::ID id, const char* label)
    {
        (void) id;
        (void) label;
    }

    virtual void onTagNode(Node::ID node, Sphere::ID sphere)
    {
        (void) node;
        (void) sphere;
    }

    virtual void onAddLink(Link::ID uid, Node::ID uid1, Node::ID uid2)
    {
        (void) uid;
        (void) uid1;
        (void) uid2;
    }

    virtual void onRemoveLink(Link::ID id)
    {
        (void) id;
    }

    virtual void onSetLinkAttribute(Link::ID id, const std::string& name, VariableType type, const std::string& value)
    {
        (void) id;
        (void) name;
        (void) type;
        (void) value;
    }

    virtual void onAddNeighbor(const std::pair<Node::ID, Link::ID>& element, const char* label, Node::ID neighbor)
    {
        (void) element;
        (void) label;
        (void) neighbor;
    }

    virtual void onAddSphere(Sphere::ID id, const char* label)
    {
        (void) id;
        (void) label;
    }

private:
	GraphContext* m_GraphContext;
	GraphModel* m_GraphModel;
	SpaceView* m_GraphView;

	SpaceMenu* m_Menu;
	bool m_ShowMenu;

	SphericalCameraController m_SphericalCameraController;
	FirstPersonCameraController m_FirstPersonCameraController;

	ToolMode m_ToolMode;
	bool m_HasSelection;
	bool m_HasTarget;
	bool m_HasPick;
	bool m_IsDragging;
	SpaceNode::ID m_PickNode;
	SpaceNode::ID m_SelectedNode;

	DemoMode m_DemoMode;
};

