#pragma once

#include <raindance/Core/HUD/Widgets/Widget.hh>
#include <raindance/Core/HUD/Widgets/TextWidget.hh>
#include <raindance/Core/HUD/Widgets/SliderWidget.hh>
#include <raindance/Core/HUD/Widgets/CheckBoxWidget.hh>
#include <raindance/Core/HUD/Widgets/TimelineWidget.hh>
#include <raindance/Core/HUD/Widgets/ClockWidget.hh>

class PointerWidget : public IWidget
{
public:
	PointerWidget(IWidget* parent, glm::vec3 pos, glm::vec2 dimension)
	: IWidget("pointer", parent, pos, dimension)
	{
		m_Icon = new Icon(glm::vec2(1.0, 1.0), glm::vec2(0.5, -0.5));
		m_Icon->load("pointer", Resources_Hud_pointer_png, sizeof(Resources_Hud_pointer_png));
	}
	virtual ~PointerWidget()
	{
		delete m_Icon;
	}
	virtual void draw(Context* context, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
	{
		m_Icon->draw(context, projection * view * glm::scale(model, glm::vec3(m_Dimension, 1.0)), glm::vec4(1.0, 1.0, 1.0, 1.0), 0);
	}
	virtual void onMouseClick(MessageQueue& messages, int x, int y)
	{
		(void) x;
		(void) y;
		messages.push(static_cast<IMessage*>(new WidgetMessage("pointer")));
	}
};

class PlayPauseWidget : public IWidget
{
public:
	enum State
	{
		PLAY = 0,
		PAUSE = 1
	};
	PlayPauseWidget(IWidget* parent, glm::vec3 pos, glm::vec2 dimension)
	: IWidget("playpause", parent, pos, dimension)
	{
		m_Icon = new Icon(glm::vec2(1.0, 1.0), glm::vec2(0.5, -0.5));
		m_Icon->load("play", Resources_Hud_play_png, sizeof(Resources_Hud_play_png));
		m_Icon->load("pause", Resources_Hud_pause_png, sizeof(Resources_Hud_pause_png));
		m_State = PAUSE;
	}
	virtual ~PlayPauseWidget()
	{
		delete m_Icon;
	}
	virtual void draw(Context* context, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
	{
	    switch(m_State)
		{
		case PLAY:
	        m_Icon->draw(context, projection * view * glm::scale(model, glm::vec3(m_Dimension, 1.0)), glm::vec4(1.0, 1.0, 1.0, 1.0), 1);
			break;
		case PAUSE:
	        m_Icon->draw(context, projection * view * glm::scale(model, glm::vec3(m_Dimension, 1.0)), glm::vec4(1.0, 1.0, 1.0, 1.0), 0);
			break;
		}
	}
	virtual void onMouseClick(MessageQueue& messages, int x, int y)
	{
		(void) x;
		(void) y;
		m_State = (State) ((m_State + 1) % 2);
		messages.push(static_cast<IMessage*>(new WidgetMessage(m_State == PLAY ? "play" : "pause")));
	}

private:
	State m_State;
};

class ExpandWidget : public IWidget
{
public:
	ExpandWidget(IWidget* parent, glm::vec3 pos, glm::vec2 dimension)
	: IWidget("expand", parent, pos, dimension)
	{
		m_Icon = new Icon(glm::vec2(1.0, 1.0), glm::vec2(0.5, -0.5));
		m_Icon->load("expand", Resources_Hud_expand_png, sizeof(Resources_Hud_expand_png));
	}
	virtual ~ExpandWidget()
	{
		delete m_Icon;
	}
	virtual void draw(Context* context, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
	{
        m_Icon->draw(context, projection * view * glm::scale(model, glm::vec3(m_Dimension, 1.0)), glm::vec4(1.0, 1.0, 1.0, 1.0), 0);
	}
	virtual void onMouseClick(MessageQueue& messages, int x, int y)
	{
		(void) x;
		(void) y;
		messages.push(static_cast<IMessage*>(new WidgetMessage("expand")));
	}
};

class EdgeWidget : public IWidget
{
public:
	EdgeWidget(IWidget* parent, glm::vec3 pos, glm::vec2 dimension)
	: IWidget("edge", parent, pos, dimension)
	{
		m_Icon = new Icon(glm::vec2(1.0, 1.0), glm::vec2(0.5, -0.5));
		m_Icon->load("edge", Resources_Hud_graph_png, sizeof(Resources_Hud_graph_png));
		m_State = 0;
	}
	virtual ~EdgeWidget()
	{
		delete m_Icon;
	}
    virtual void draw(Context* context, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
    {
        m_Icon->draw(context, projection * view * glm::scale(model, glm::vec3(m_Dimension, 1.0)), m_State != 2 ? glm::vec4(1.0, 1.0, 1.0, 1.0) : glm::vec4(0.5, 0.5, 0.5, 1.0), 0);
    }
    virtual void onMouseClick(MessageQueue& messages, int x, int y)
    {
        (void) x;
        (void) y;
        m_State = (m_State + 1) % 3;
        switch(m_State)
        {
        case 0:
            messages.push(static_cast<IMessage*>(new WidgetMessage("space:edges:lines")));
            break;
        case 1:
            messages.push(static_cast<IMessage*>(new WidgetMessage("space:edges:widelines")));
            break;
        case 2:
            messages.push(static_cast<IMessage*>(new WidgetMessage("space:edges:off")));
            break;
        default:
            break;
        }
    }
private:
    unsigned int m_State;
};

class LabelWidget : public IWidget
{
public:
	LabelWidget(IWidget* parent, glm::vec3 pos, glm::vec2 dimension)
	: IWidget("label", parent, pos, dimension)
	{
		m_Icon = new Icon(glm::vec2(1.0, 1.0), glm::vec2(0.5, -0.5));
		m_Icon->load("label", Resources_Hud_label_png, sizeof(Resources_Hud_label_png));
		m_State = true;
	}
	virtual ~LabelWidget()
	{
		delete m_Icon;
	}
	virtual void draw(Context* context, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
	{
	    m_Icon->draw(context, projection * view * glm::scale(model, glm::vec3(m_Dimension, 1.0)), m_State ? glm::vec4(1.0, 1.0, 1.0, 1.0) : glm::vec4(0.5, 0.5, 0.5, 1.0), 0);
	}
	virtual void onMouseClick(MessageQueue& messages, int x, int y)
	{
		(void) x;
		(void) y;
		m_State = !m_State;
		messages.push(static_cast<IMessage*>(new WidgetMessage(m_State ? "show labels" : "hide labels")));
	}
private:
	bool m_State;
};

class SpheresWidget : public IWidget
{
public:
	SpheresWidget(IWidget* parent, glm::vec3 pos, glm::vec2 dimension)
	: IWidget("spheres", parent, pos, dimension)
	{
		m_Icon = new Icon(glm::vec2(1.0, 1.0), glm::vec2(0.5, -0.5));
		m_Icon->load("spheres", Resources_Hud_spheres_png, sizeof(Resources_Hud_spheres_png));
		m_State = true;
	}
	virtual ~SpheresWidget()
	{
		delete m_Icon;
	}
	virtual void draw(Context* context, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
	{
	    m_Icon->draw(context, projection * view * glm::scale(model, glm::vec3(m_Dimension, 1.0)), m_State ? glm::vec4(1.0, 1.0, 1.0, 1.0) : glm::vec4(0.5, 0.5, 0.5, 1.0), 0);
	}
	virtual void onMouseClick(MessageQueue& messages, int x, int y)
	{
		(void) x;
		(void) y;
		m_State = !m_State;
		messages.push(static_cast<IMessage*>(new WidgetMessage(m_State ? "show spheres" : "hide spheres")));
	}
private:
	bool m_State;
};

class NodeWidget : public IWidget
{
public:
	NodeWidget(IWidget* parent, glm::vec3 pos, glm::vec2 dimension)
	: IWidget("node", parent, pos, dimension)
	{
		m_Icon = new Icon(glm::vec2(1.0, 1.0), glm::vec2(0.5, -0.5));
		m_Icon->load("node", Resources_Hud_node_png, sizeof(Resources_Hud_node_png));
		m_State = 0;
	}
	virtual ~NodeWidget()
	{
		delete m_Icon;
	}
	virtual void draw(Context* context, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
	{
	    m_Icon->draw(context, projection * view * glm::scale(model, glm::vec3(m_Dimension, 1.0)), m_State != 7 ? glm::vec4(1.0, 1.0, 1.0, 1.0) : glm::vec4(0.5, 0.5, 0.5, 1.0), 0);
	}
	virtual void onMouseClick(MessageQueue& messages, int x, int y)
	{
		(void) x;
		(void) y;
		m_State = (m_State + 1) % 8;
		switch(m_State)
		{
		case 0:
			messages.push(static_cast<IMessage*>(new WidgetMessage("space:nodes:all")));
			break;
		case 1:
			messages.push(static_cast<IMessage*>(new WidgetMessage("space:nodes:shapes+marks")));
			break;
		case 2:
			messages.push(static_cast<IMessage*>(new WidgetMessage("space:nodes:shapes+labels")));
			break;
		case 3:
			messages.push(static_cast<IMessage*>(new WidgetMessage("space:nodes:shapes")));
			break;
		case 4:
			messages.push(static_cast<IMessage*>(new WidgetMessage("space:nodes:marks+labels")));
			break;
		case 5:
			messages.push(static_cast<IMessage*>(new WidgetMessage("space:nodes:marks")));
			break;
		case 6:
			messages.push(static_cast<IMessage*>(new WidgetMessage("space:nodes:labels")));
			break;
		case 7:
			messages.push(static_cast<IMessage*>(new WidgetMessage("space:nodes:off")));
			break;
		default:
			break;
		}
	}
private:
	unsigned int m_State;
};

class LoupeWidget : public IWidget
{
public:
	LoupeWidget(IWidget* parent, glm::vec3 pos, glm::vec2 dimension)
	: IWidget("loupe", parent, pos, dimension)
	{
		m_Icon = new Icon(glm::vec2(1.0, 1.0), glm::vec2(0.5, -0.5));
		m_Icon->load("loupe", Resources_Hud_loupe_png, sizeof(Resources_Hud_loupe_png));
	}
	virtual ~LoupeWidget()
	{
		delete m_Icon;
	}
	virtual void draw(Context* context, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
	{
	    m_Icon->draw(context, projection * view * glm::scale(model, glm::vec3(m_Dimension, 1.0)), glm::vec4(1.0, 1.0, 1.0, 1.0), 0);
	}
	virtual void onMouseClick(MessageQueue& messages, int x, int y)
	{
		(void) messages;
		(void) x;
		(void) y;
	}
};

class MarkerWidget : public IWidget
{
public:
	enum MarkerColor
	{
		WHITE = 0,    GRAY = 1,
		RED = 2,      LOW_RED = 3,
		GREEN = 4,    LOW_GREEN = 5,
		BLUE = 6,     LOW_BLUE = 7,
		MAGENTA = 8,  LOW_MAGENTA = 9,
		YELLOW = 10,  LOW_YELLOW = 11,
		CYAN = 12,    LOW_CYAN = 13,
		BLACK
	};
	MarkerWidget(IWidget* parent, glm::vec3 pos, glm::vec2 dimension)
	: IWidget("marker", parent, pos, dimension)
	{
		m_Icon = new Icon(glm::vec2(1.0, 1.0), glm::vec2(0.5, -0.5));
		m_Icon->load("marker", Resources_Hud_marker_png, sizeof(Resources_Hud_marker_png));
		m_Marker = 1;
	}
	virtual ~MarkerWidget()
	{
		delete m_Icon;
	}
	virtual void draw(Context* context, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
	{
	    m_Icon->draw(context, projection * view * glm::scale(model, glm::vec3(m_Dimension, 1.0)), color(m_Marker), 0);
	}
	virtual void onMouseClick(MessageQueue& messages, int x, int y)
	{
		(void) x;
		(void) y;
		messages.push(static_cast<IMessage*>(new WidgetMessage("marker")));
	}

	static glm::vec4 color(unsigned int marker)
	{
		MarkerColor color = (MarkerColor) marker;

		switch(color)
		{
		case WHITE:
			return glm::vec4(1.0, 1.0, 1.0, 1.0);
		case GRAY:
			return glm::vec4(0.5, 0.5, 0.5, 1.0);

		case RED:
			return glm::vec4(1.0, 0.0, 0.0, 1.0);
		case LOW_RED:
			return glm::vec4(0.5, 0.0, 0.0, 1.0);

		case GREEN:
			return glm::vec4(0.0, 1.0, 0.0, 1.0);
		case LOW_GREEN:
			return glm::vec4(0.0, 0.5, 0.0, 1.0);

		case BLUE:
			return glm::vec4(0.0, 0.0, 1.0, 1.0);
		case LOW_BLUE:
			return glm::vec4(0.0, 0.0, 0.5, 1.0);

		case MAGENTA:
			return glm::vec4(1.0, 0.0, 1.0, 1.0);
		case LOW_MAGENTA:
			return glm::vec4(0.5, 0.0, 0.5, 1.0);

		case YELLOW:
			return glm::vec4(1.0, 1.0, 0.0, 1.0);
		case LOW_YELLOW:
			return glm::vec4(0.5, 0.5, 0.0, 1.0);

		case CYAN:
			return glm::vec4(0.0, 1.0, 1.0, 1.0);
		case LOW_CYAN:
			return glm::vec4(0.0, 0.5, 0.5, 1.0);

		case BLACK:
		default:
			return glm::vec4(0.0, 0.0, 0.0, 1.0);
		}
	}

	void next()
	{
		m_Marker = 1 + m_Marker % BLACK;
	}

	unsigned int marker()
	{
		return m_Marker;
	}

private:
	unsigned int m_Marker;
};

class BugWidget : public IWidget
{
public:
	BugWidget(IWidget* parent, glm::vec3 pos, glm::vec2 dimension)
	: IWidget("bug", parent, pos, dimension)
	{
		m_Icon = new Icon(glm::vec2(1.0, 1.0), glm::vec2(0.5, -0.5));
		m_Icon->load("bug", Resources_Hud_bug_png, sizeof(Resources_Hud_bug_png));
		m_State = false;
	}
	virtual ~BugWidget()
	{
		delete m_Icon;
	}
	virtual void draw(Context* context, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
	{
	    m_Icon->draw(context, projection * view * glm::scale(model, glm::vec3(m_Dimension, 1.0)), m_State ? glm::vec4(1.0, 1.0, 1.0, 1.0) : glm::vec4(0.5, 0.5, 0.5, 1.0), 0);
	}
	virtual void onMouseClick(MessageQueue& messages, int x, int y)
	{
		(void) x;
		(void) y;
		m_State = !m_State;
		messages.push(static_cast<IMessage*>(new WidgetMessage(m_State ? "show debug" : "hide debug")));
	}
private:
	bool m_State;
};

class SpaceMenu //  : public WidgetGroup
{
public:
	SpaceMenu()
	{
		m_WindowWidth = glutGet(GLUT_WINDOW_WIDTH);
		m_WindowHeight = glutGet(GLUT_WINDOW_HEIGHT);
		m_Camera.setOrthographicProjection(0.0f, (float)m_WindowWidth, 0.0f, (float)m_WindowHeight, 0.001f, 100.f);
		m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_Font = new Font();
		m_WidgetDimension = glm::vec2(24, 24);
		m_WidgetSpacing = 10;
		float textWidgetWidth = 60;

		// ----- Graph Menu -----
		{
		    // ----- Top Left Part -----

			m_TopLeftWidgetGroup = new WidgetGroup("top left", WidgetGroup::TOP_LEFT);

			glm::vec3 tl = glm::vec3(m_WidgetSpacing, -m_WidgetSpacing, 0);

			m_TopLeftWidgetGroup->add(m_PointerWidget = new PointerWidget(NULL, tl, m_WidgetDimension));
			tl.x += m_WidgetDimension.x + m_WidgetSpacing;
			m_TopLeftWidgetGroup->add(m_MarkerWidget = new MarkerWidget(NULL, tl, m_WidgetDimension));
			tl.x -= m_WidgetDimension.x + m_WidgetSpacing;
			tl.y -= m_WidgetDimension.y;

	        tl.y -= m_WidgetDimension.y;

			TextWidget* nodeTextWidget = new TextWidget("node text", NULL, tl, glm::vec2(textWidgetWidth, m_WidgetDimension.y));
            m_TopLeftWidgetGroup->add(nodeTextWidget);
            tl -= glm::vec3(0, m_WidgetDimension.y, 0);
            nodeTextWidget->text().set("Nodes", m_Font);

            tl.x += m_WidgetDimension.x;

            m_TopLeftWidgetGroup->add(m_NodeWidget = new NodeWidget(NULL, tl, m_WidgetDimension));
            tl.x += m_WidgetSpacing + m_WidgetDimension.x;
            m_TopLeftWidgetGroup->add(m_NodeTextWidget = new TextWidget("nodes text", NULL, tl, glm::vec2(textWidgetWidth, m_WidgetDimension.y)));
            tl -= glm::vec3(m_WidgetSpacing + m_WidgetDimension.x, m_WidgetSpacing + m_WidgetDimension.y, 0);
            m_NodeTextWidget->text().set("All", m_Font);

            m_TopLeftWidgetGroup->add(m_Slider1 = new SliderWidget("slider1", NULL, tl, glm::vec2(m_WidgetDimension.x * 3, m_WidgetDimension.y / 2)));
            tl.x += m_WidgetDimension.x * 3 + m_WidgetSpacing;
            tl.y += 2;
            TextWidget* nodeSizeWidget = new TextWidget("node size text", NULL, tl, glm::vec2(textWidgetWidth, m_WidgetDimension.y));
            m_TopLeftWidgetGroup->add(nodeSizeWidget);
            nodeSizeWidget->text().set("Size", m_Font);
            tl.x -= m_WidgetDimension.x * 3 + m_WidgetSpacing;
            tl.y -= m_WidgetDimension.y + 2;

            m_TopLeftWidgetGroup->add(m_CheckBox3 = new CheckBoxWidget("checkbox 3", NULL, tl, glm::vec2(m_WidgetDimension.x * 0.66, m_WidgetDimension.y * 0.66)));
            tl.x += m_WidgetDimension.x * 0.66 + m_WidgetSpacing;
            TextWidget* checkbox3TextWidget = new TextWidget("checkbox3 text", NULL, tl, glm::vec2(textWidgetWidth, m_WidgetDimension.y));
            m_TopLeftWidgetGroup->add(checkbox3TextWidget);
            checkbox3TextWidget->text().set("Activity", m_Font);
            tl -= glm::vec3(m_WidgetDimension.x * 0.66 + m_WidgetSpacing, m_WidgetDimension.y, 0);

            tl.x -= m_WidgetDimension.x;
            tl.y -= m_WidgetDimension.y;

            TextWidget* edgeTextWidget = new TextWidget("edge text", NULL, tl, glm::vec2(textWidgetWidth, m_WidgetDimension.y));
            m_TopLeftWidgetGroup->add(edgeTextWidget);
            tl -= glm::vec3(0, m_WidgetDimension.y, 0);
            edgeTextWidget->text().set("Edges", m_Font);

            tl.x += m_WidgetDimension.x;

            m_TopLeftWidgetGroup->add(m_EdgeWidget = new EdgeWidget(NULL, tl, m_WidgetDimension));
            tl.x += m_WidgetSpacing + m_WidgetDimension.x;
            m_TopLeftWidgetGroup->add(m_EdgeTextWidget = new TextWidget("edges text", NULL, tl, glm::vec2(textWidgetWidth, m_WidgetDimension.y)));
            tl -= glm::vec3(m_WidgetSpacing + m_WidgetDimension.x, m_WidgetSpacing + m_WidgetDimension.y, 0);
            m_EdgeTextWidget->text().set("Lines", m_Font);

            m_TopLeftWidgetGroup->add(m_Slider2 = new SliderWidget("slider2", NULL, tl, glm::vec2(m_WidgetDimension.x * 3, m_WidgetDimension.y / 2)));
            tl.x += m_WidgetDimension.x * 3 + m_WidgetSpacing;
            tl.y += 2;
            TextWidget* edgeSizeWidget = new TextWidget("edge size text", NULL, tl, glm::vec2(textWidgetWidth, m_WidgetDimension.y));
            m_TopLeftWidgetGroup->add(edgeSizeWidget);
            edgeSizeWidget->text().set("Size", m_Font);
            tl.x -= m_WidgetDimension.x * 3 + m_WidgetSpacing;
            tl.y -= m_WidgetDimension.y + 2;

            m_TopLeftWidgetGroup->add(m_CheckBox4 = new CheckBoxWidget("checkbox 4", NULL, tl, glm::vec2(m_WidgetDimension.x * 0.66, m_WidgetDimension.y * 0.66)));
            tl.x += m_WidgetDimension.x * 0.66 + m_WidgetSpacing;
            TextWidget* checkbox4TextWidget = new TextWidget("checkbox4 text", NULL, tl, glm::vec2(textWidgetWidth, m_WidgetDimension.y));
            m_TopLeftWidgetGroup->add(checkbox4TextWidget);
            checkbox4TextWidget->text().set("Activity", m_Font);
            tl -= glm::vec3(m_WidgetDimension.x * 0.66 + m_WidgetSpacing, m_WidgetDimension.y, 0);

            tl.x -= m_WidgetDimension.x;
            tl.y -= m_WidgetDimension.y;

            /*
            m_TopLeftWidgetGroup->add(m_SpheresWidget = new SpheresWidget(NULL, tl, m_WidgetDimension));
            tl.x += m_WidgetSpacing + m_WidgetDimension.x;
            m_TopLeftWidgetGroup->add(m_SpheresTextWidget = new TextWidget("spheres text", NULL, tl, glm::vec2(textWidgetWidth, m_WidgetDimension.y)));
            tl -= glm::vec3(m_WidgetSpacing + m_WidgetDimension.x, m_WidgetSpacing + m_WidgetDimension.y, 0);
            m_SpheresTextWidget->text().set("On", m_Font);
            tl.y -= m_WidgetDimension.y;
            */
            // TODO : Integrate SphereWidget when possible
            m_SpheresWidget = NULL;

            TextWidget* physicsWidget = new TextWidget("physics text", NULL, tl, glm::vec2(textWidgetWidth, m_WidgetDimension.y));
            m_TopLeftWidgetGroup->add(physicsWidget);
            physicsWidget->text().set("Physics", m_Font);
            tl.y -= m_WidgetDimension.y + m_WidgetSpacing;

            tl.x += m_WidgetDimension.x;

            m_TopLeftWidgetGroup->add(m_Slider3 = new SliderWidget("slider3", NULL, tl, glm::vec2(m_WidgetDimension.x * 3, m_WidgetDimension.y / 2)));
            tl.x += m_WidgetDimension.x * 3 + m_WidgetSpacing;
            tl.y += 2;

            TextWidget* temperatureWidget = new TextWidget("temperature text", NULL, tl, glm::vec2(textWidgetWidth, m_WidgetDimension.y));
            m_TopLeftWidgetGroup->add(temperatureWidget);
            temperatureWidget->text().set("Temperature", m_Font);
            tl.x -= m_WidgetDimension.x * 3 + m_WidgetSpacing;
			tl.y -= m_WidgetDimension.y + 2;

            m_TopLeftWidgetGroup->add(m_PlayPauseWidget = new PlayPauseWidget(NULL, tl, m_WidgetDimension));
            tl.y -= m_WidgetDimension.y + m_WidgetSpacing;

            tl.x -= m_WidgetDimension.x;
            tl.y -= m_WidgetDimension.y;

            TextWidget* filtersWidget = new TextWidget("filters text", NULL, tl, glm::vec2(textWidgetWidth, m_WidgetDimension.y));
            m_TopLeftWidgetGroup->add(filtersWidget);
            filtersWidget->text().set("Filters", m_Font);
            tl.y -= m_WidgetDimension.y + m_WidgetSpacing;

            tl.x += m_WidgetDimension.x;

            TextWidget* lodtextWidget = new TextWidget("lod text", NULL, tl, glm::vec2(textWidgetWidth, m_WidgetDimension.y));
            m_TopLeftWidgetGroup->add(lodtextWidget);
            tl -= glm::vec3(0, m_WidgetDimension.y, 0);
            lodtextWidget->text().set("LOD Threshold", m_Font);
            m_TopLeftWidgetGroup->add(m_LODSlider = new SliderWidget("lod slider", NULL, tl, glm::vec2(m_WidgetDimension.x * 5, m_WidgetDimension.y / 2)));
            tl.y -= m_WidgetSpacing + m_WidgetDimension.y / 2;

            m_TopLeftWidgetGroup->add(m_CheckBox1 = new CheckBoxWidget("checkbox 1", NULL, tl, glm::vec2(m_WidgetDimension.x * 0.66, m_WidgetDimension.y * 0.66)));
            tl.x += m_WidgetDimension.x * 0.66 + m_WidgetSpacing;
            TextWidget* checkbox1TextWidget = new TextWidget("checkbox1 text", NULL, tl, glm::vec2(textWidgetWidth, m_WidgetDimension.y));
            m_TopLeftWidgetGroup->add(checkbox1TextWidget);
            checkbox1TextWidget->text().set("Node LOD", m_Font);
            tl -= glm::vec3(m_WidgetDimension.x * 0.66 + m_WidgetSpacing, m_WidgetDimension.y, 0);

            m_TopLeftWidgetGroup->add(m_CheckBox2 = new CheckBoxWidget("checkbox 2", NULL, tl, glm::vec2(m_WidgetDimension.x * 0.66, m_WidgetDimension.y * 0.66)));
            tl.x += m_WidgetDimension.x * 0.66 + m_WidgetSpacing;
            TextWidget* checkbox2TextWidget = new TextWidget("checkbox2 text", NULL, tl, glm::vec2(textWidgetWidth, m_WidgetDimension.y));
            m_TopLeftWidgetGroup->add(checkbox2TextWidget);
            checkbox2TextWidget->text().set("Edge LOD", m_Font);
            tl -= glm::vec3(m_WidgetDimension.x * 0.66 + m_WidgetSpacing, m_WidgetDimension.y, 0);

            // ----- Bottom left part -----

            m_BottomLeftWidgetGroup = new WidgetGroup("bottom left", WidgetGroup::BOTTOM_LEFT);

            float titleWidth = 200;
            float fontHeight = 18;
            float titleSize = 1.5;

            glm::vec3 bl = glm::vec3(m_WidgetSpacing, m_WidgetSpacing + fontHeight * titleSize, 0);

            m_BottomLeftWidgetGroup->add(m_TitleWidget = new TextWidget("title widget", NULL , bl, glm::vec2(titleWidth, m_WidgetDimension.y)));
            m_TitleWidget->text().set("", m_Font);
            m_TitleWidget->setSize(titleSize);

            // ----- Bottom right part -----

            m_BottomRightWidgetGroup = new WidgetGroup("bottom right", WidgetGroup::BOTTOM_RIGHT);

            glm::vec2 timelineDimension = glm::vec2(400, 32);

            float logoSize = 64;

            glm::vec3 br = glm::vec3(-m_WidgetSpacing - logoSize - timelineDimension.x - m_WidgetSpacing, m_WidgetSpacing + timelineDimension.y, 0);
            m_BottomRightWidgetGroup->add(m_TimelineWidget = new TimelineWidget("timeline", NULL, br, timelineDimension));

            br.y += m_WidgetSpacing + m_WidgetDimension.y - 5;

            m_BottomRightWidgetGroup->add(m_ClockWidget = new ClockWidget("clock", NULL, br, m_WidgetDimension));

		}
	}

	IWidget* pickWidget(int x, int y)
	{
		IWidget* pick = NULL;

        pick = m_TopLeftWidgetGroup->pickWidget(x, y);
        if (pick != NULL)
            return pick;
        pick = m_BottomLeftWidgetGroup->pickWidget(x, y);
        if (pick != NULL)
            return pick;
        pick = m_BottomRightWidgetGroup->pickWidget(x, y);
        if (pick != NULL)
            return pick;
		return NULL;
	}

	virtual ~SpaceMenu()
	{
		delete m_TopLeftWidgetGroup;
		delete m_BottomLeftWidgetGroup;
		delete m_BottomRightWidgetGroup;
		delete m_Font;
	}

	virtual void reshape(int width, int height)
	{
		m_WindowWidth = width;
		m_WindowHeight = height;

		m_Camera.reshape(width, height);
		m_Camera.setOrthographicProjection(0.0f, (float)width, 0.0f, (float)height, 0.001f, 100.f);
		m_Camera.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		m_TopLeftWidgetGroup->reshape(width, height);
		m_BottomLeftWidgetGroup->reshape(width, height);
		m_BottomRightWidgetGroup->reshape(width, height);
	}

	virtual void draw(Context* context)
	{
		glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
        m_TopLeftWidgetGroup->draw(context, glm::mat4(), m_Camera.getViewMatrix(), m_Camera.getProjectionMatrix());
        m_BottomLeftWidgetGroup->draw(context, glm::mat4(), m_Camera.getViewMatrix(), m_Camera.getProjectionMatrix());
        m_BottomRightWidgetGroup->draw(context, glm::mat4(), m_Camera.getViewMatrix(), m_Camera.getProjectionMatrix());
	}

	inline Font* getFont() { return m_Font; }
	inline MarkerWidget* getMarkerWidget() { return m_MarkerWidget; }
	inline TextWidget* getNodeTextWidget() { return m_NodeTextWidget; }
	inline TextWidget* getEdgeTextWidget() { return m_EdgeTextWidget; }
	inline TextWidget* getSpheresTextWidget() { return m_SpheresTextWidget; }
    inline SliderWidget* getSlider1() { return m_Slider1; }
    inline SliderWidget* getSlider2() { return m_Slider2; }
    inline SliderWidget* getSlider3() { return m_Slider3; }
    inline CheckBoxWidget* getCheckBox1() { return m_CheckBox1; }
    inline CheckBoxWidget* getCheckBox2() { return m_CheckBox2; }
    inline CheckBoxWidget* getCheckBox3() { return m_CheckBox3; }
    inline CheckBoxWidget* getCheckBox4() { return m_CheckBox4; }
    inline SliderWidget* getLODSlider() { return m_LODSlider; }
	inline TextWidget* getTitle() { return m_TitleWidget; }
    inline TimelineWidget* getTimeline() { return m_TimelineWidget; }
    inline ClockWidget* getClock() { return m_ClockWidget; }

private:
	int m_WindowHeight;
	int m_WindowWidth;
	Camera m_Camera;
	Font* m_Font;
	glm::vec2 m_WidgetDimension;
	float m_WidgetSpacing;
	WidgetGroup* m_TopLeftWidgetGroup;
    WidgetGroup* m_BottomLeftWidgetGroup;
    WidgetGroup* m_BottomRightWidgetGroup;

	PointerWidget* m_PointerWidget;
	MarkerWidget* m_MarkerWidget;
	PlayPauseWidget* m_PlayPauseWidget;
	NodeWidget* m_NodeWidget;
	TextWidget* m_NodeTextWidget;
	EdgeWidget* m_EdgeWidget;
	TextWidget* m_EdgeTextWidget;
	SpheresWidget* m_SpheresWidget;
	TextWidget* m_SpheresTextWidget;

	SliderWidget* m_Slider1;
    SliderWidget* m_Slider2;
    SliderWidget* m_Slider3;

    CheckBoxWidget* m_CheckBox1;
    CheckBoxWidget* m_CheckBox2;
    CheckBoxWidget* m_CheckBox3;
    CheckBoxWidget* m_CheckBox4;

    SliderWidget* m_LODSlider;

	TextWidget* m_TitleWidget;
	TimelineWidget* m_TimelineWidget;
	ClockWidget* m_ClockWidget;
};
