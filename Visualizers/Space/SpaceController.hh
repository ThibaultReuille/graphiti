#pragma once

#include <raindance/Core/Headers.hh>
#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/Camera/Controllers.hh>
#include <raindance/Core/Camera/Sequences.hh>
#include <raindance/Core/Controller.hh>
#include <raindance/Core/Variables.hh>

#include <graphiti/Entities/MVC.hh>
#include <graphiti/Entities/Graph/GraphModel.hh>
#include <graphiti/Visualizers/Space/SpaceWidgets.hh>

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
		m_HasPick = false;
		m_IsDragging = false;

		m_Menu = new SpaceMenu();
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

		m_CameraController.bind(context, m_GraphView->camera());

		m_Menu->bind(view);
        m_Menu->getTimeline()->bindTrack(m_GraphContext->sequencer().track("command"));
        m_Menu->getClock()->bind(&m_GraphContext->sequencer().track("command")->clock());
	}

	void draw() override
	{
		m_Menu->draw(m_GraphContext);
	}

	void idle() override
	{
		updateSelection();

		m_CameraController.updateCamera();

        g_SpaceResources->ShowNodeLOD = m_Menu->getCheckBox1()->value();
        g_SpaceResources->ShowEdgeLOD = m_Menu->getCheckBox2()->value();
        g_SpaceResources->ShowNodeActivity = m_Menu->getCheckBox3()->value();
        g_SpaceResources->ShowEdgeActivity = m_Menu->getCheckBox4()->value();
	}

	void updateSelection()
	{
		// NOTE : Has selected node been removed ?
		if (m_HasSelection && !m_GraphView->getNodeMap().containsLocalID(m_SelectedNode))
		{
			m_HasSelection = false;
			m_IsDragging = false;
			m_HasPick = false;
		}
	}

	void onResize(const Viewport& viewport) override
	{
		if (g_SpaceResources->m_Wallpaper)
			g_SpaceResources->m_Wallpaper->onResize(viewport);

		m_Menu->onResize(viewport);

		m_CameraController.onResize(viewport);
	}

	void onKey(int key, int scancode, int action, int mods) override
	{
		if (action == GLFW_PRESS && key == GLFW_KEY_SPACE && mods == GLFW_MOD_ALT)
		{
		    if (m_GraphContext->sequencer().clock().state() == Clock::STARTED)
		        m_GraphContext->sequencer().clock().pause();
		    else
		        m_GraphContext->sequencer().clock().start();
		    return;
		}
		
		m_CameraController.onKey(key, scancode, action, mods);
	}
	
	void onScroll(double xoffset, double yoffset) override
	{
		m_CameraController.onScroll(xoffset, yoffset);
	}
	
	void onMouseMove(const glm::vec2& pos, const glm::vec2& dpos) override
	{
		updateSelection();

		if (m_IsDragging)
		{
			dragVertex(m_SelectedNode, (int)pos.x, (int)pos.y);
		}
		else
		{
			m_HasPick = m_GraphView->pickNode((int)pos.x, (int)pos.y, &m_PickNode);

			if (m_HasPick && m_PickNode == m_SelectedNode && dpos.x < 5 && dpos.y < 5)
			{
				m_IsDragging = true;
				dragVertex(m_SelectedNode, (int)pos.x, (int)pos.y);
			}
			else
			{
				m_CameraController.onMouseMove(pos, dpos);
			}
		}
	}

	void onMouseDown(const glm::vec2& pos) override
	{
		m_IsDragging = false;
		m_CameraController.onMouseDown(pos);
	}

	void onMouseClick(const glm::vec2& pos) override
	{
		m_IsDragging = false;

		updateSelection();

		IWidget* pickWidget = m_Menu->pickWidget(pos);
		if (pickWidget != NULL)
		{
			pickWidget->onMouseClick(m_GraphContext->messages(), pos);
		}
		else
		{
			m_HasPick = m_GraphView->pickNode((int)pos.x, (int)pos.y, &m_PickNode);
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

					int marker = m_Menu->getMarkerWidget()->marker();
					int mark = node->getMark() != marker ? marker : 0;
					node->setMark(mark);

					std::ostringstream smark;
					smark << mark;
					m_GraphView->onSetNodeAttribute(id, "space:mark", RD_INT, smark.str());
				}
			}
			else
			{
				if (m_HasSelection)
					m_GraphModel->unselectNode(m_GraphView->getNodeMap().getRemoteID(m_SelectedNode));
				m_HasSelection = false;

				m_CameraController.select(CameraController::FIRST_PERSON);
			}

			m_CameraController.onMouseClick(pos);
		}
	}

	void onMouseDoubleClick(const glm::vec2& pos) override
	{
		updateSelection();

		if (m_HasSelection)
		{
			m_GraphContext->messages().push(new GraphTargetNodeMessage(m_SelectedNode));
			m_CameraController.select(CameraController::SPHERICAL);
		}

		m_CameraController.onMouseDoubleClick(pos);
	}

	void onMouseTripleClick(const glm::vec2& pos) override
	{
		updateSelection();

		m_CameraController.onMouseTripleClick(pos);
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

	void notify(IMessage* message) override
	{
		IMessage::Type type = message->type();
		if (type == TARGET_NODE)
		{
			GraphTargetNodeMessage* msg = static_cast<GraphTargetNodeMessage*>(message);

			Scene::Node* node = m_GraphView->getNodes()[msg->ID];

			// NOTE : Calculate the zoom distance according to the node size so that it always has the same screen size.
			float zoomAngle = M_PI / 20;
			float zoomDistance = static_cast<SpaceNode*>(node)->getScreenSize() / (2 * tan(zoomAngle / 2));

			m_CameraController.zoom(node->getPosition(), zoomDistance, 250);
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
				float value = 5.0f * m_Menu->getSlider1()->getValue();
				m_GraphView->setNodeSize(value);
			}
            else if (msg->Name == "slider2" && msg->Message == "update")
            {
                float value = 3.0f * m_Menu->getSlider2()->getValue();
                m_GraphView->setEdgeSize(value);
            }
            else if (msg->Name == "slider3" && msg->Message == "update")
            {
                float x = m_Menu->getSlider3()->getValue();
                float value = 20.0f * x * x;
                m_GraphView->setTemperature(value);
            }
            else if (msg->Name == "slider4" && msg->Message == "update")
            {
                float value = m_Menu->getSlider4()->getValue();
                g_SpaceResources->LODSlice[0] = value;
            }
            else if (msg->Name == "slider5" && msg->Message == "update")
            {
                float value = m_Menu->getSlider5()->getValue();
                g_SpaceResources->LODSlice[1] = value;
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
			{
				g_SpaceResources->ShowNodeShapes = SpaceResources::ALL;
 				g_SpaceResources->ShowNodeLabels = true;
				m_Menu->getNodeTextWidget()->text().set("All", m_Menu->getFont());
			}
			else if (msg->Message == "space:nodes:shapes+labels")
			{
				g_SpaceResources->ShowNodeShapes = SpaceResources::COLORS;
 				g_SpaceResources->ShowNodeLabels = true;
				m_Menu->getNodeTextWidget()->text().set("Shapes + Labels", m_Menu->getFont());
			}
			else if (msg->Message == "space:nodes:marks+labels")
			{	
				g_SpaceResources->ShowNodeShapes = SpaceResources::MARKS;
 				g_SpaceResources->ShowNodeLabels = true;
				m_Menu->getNodeTextWidget()->text().set("Marks + Labels", m_Menu->getFont());
			}
			else if (msg->Message == "space:nodes:shapes+marks")
			{
				g_SpaceResources->ShowNodeShapes = SpaceResources::ALL;
 				g_SpaceResources->ShowNodeLabels = false;
				m_Menu->getNodeTextWidget()->text().set("Shapes + Marks", m_Menu->getFont());
			}
			else if (msg->Message == "space:nodes:shapes")
			{
				g_SpaceResources->ShowNodeShapes = SpaceResources::COLORS;
 				g_SpaceResources->ShowNodeLabels = false;
				m_Menu->getNodeTextWidget()->text().set("Shapes", m_Menu->getFont());
			}
			else if (msg->Message == "space:nodes:marks")
			{
				g_SpaceResources->ShowNodeShapes = SpaceResources::MARKS;
 				g_SpaceResources->ShowNodeLabels = false;
				m_Menu->getNodeTextWidget()->text().set("Marks", m_Menu->getFont());
			}
			else if (msg->Message == "space:nodes:labels")
			{
				g_SpaceResources->ShowNodeShapes = SpaceResources::NONE;
 				g_SpaceResources->ShowNodeLabels = true;
				m_Menu->getNodeTextWidget()->text().set("Labels", m_Menu->getFont());
			}
			else if (msg->Message == "space:nodes:off")
			{
				g_SpaceResources->ShowNodeShapes = SpaceResources::NONE;
 				g_SpaceResources->ShowNodeLabels = false;
				m_Menu->getNodeTextWidget()->text().set("Off", m_Menu->getFont());
			}
			
		    else if (msg->Message == "space:edges:widelines")
		    {
		    	g_SpaceResources->m_EdgeMode = SpaceResources::WIDE_LINES;
		     	m_Menu->getEdgeTextWidget()->text().set("Wide Lines", m_Menu->getFont());
		    }
			else if (msg->Message == "space:edges:lines")
			{
				g_SpaceResources->m_EdgeMode = SpaceResources::LINES;
				m_Menu->getEdgeTextWidget()->text().set("Lines", m_Menu->getFont());
		    }
		    else if (msg->Message == "space:edges:off")
		    {
		    	g_SpaceResources->m_EdgeMode = SpaceResources::OFF;
		        m_Menu->getEdgeTextWidget()->text().set("Off", m_Menu->getFont());
		    }

			else if (msg->Message == "show spheres")
			{
				g_SpaceResources->ShowSpheres = true;
				m_Menu->getSpheresTextWidget()->text().set("On", m_Menu->getFont());
			}
			else if (msg->Message == "hide spheres")
			{
				g_SpaceResources->ShowSpheres = false;
				m_Menu->getSpheresTextWidget()->text().set("Off", m_Menu->getFont());
			}

			else if (msg->Message == "show debug")
 				g_SpaceResources->ShowDebug = true;
 			else if (msg->Message == "hide debug")
 				g_SpaceResources->ShowDebug = false;

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

    void onSetAttribute(const std::string& name, VariableType type, const std::string& value) override
    {
        StringVariable vstring;

        if (name == "space:title" && type == RD_STRING)
        {
            vstring.set(value);
            m_Menu->getTitle()->text().set(vstring.value().c_str(), m_Menu->getFont());
        }
    }

private:
	GraphContext* m_GraphContext;
	GraphModel* m_GraphModel;
	SpaceView* m_GraphView;

	SpaceMenu* m_Menu;

	CameraController m_CameraController;

	ToolMode m_ToolMode;
	bool m_HasSelection;
	bool m_HasTarget;
	bool m_HasPick;
	bool m_IsDragging;
	SpaceNode::ID m_PickNode;
	SpaceNode::ID m_SelectedNode;

	DemoMode m_DemoMode;
};

