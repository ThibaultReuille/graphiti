#pragma once

#include <raindance/Core/HUD/Widgets/Widget.hh>

class ViewWidget : public IWidget
{
public:
	ViewWidget(IWidget* parent, glm::vec3 pos, glm::vec2 dimension)
	: IWidget("view", parent, pos, dimension)
	{
		m_Icon = new Icon(glm::vec2(1.0, 1.0), glm::vec2(0.5, -0.5));
		m_Icon->load("eye", Resources_Hud_eye_png, sizeof(Resources_Hud_eye_png));
		m_State = 0;
	}
	virtual ~ViewWidget()
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
		m_State = (m_State + 1) % 7;
		switch(m_State)
		{
		case 0:
			messages.push(static_cast<IMessage*>(new WidgetMessage(m_Name.c_str(), "perspective")));
			break;
		case 1:
			messages.push(static_cast<IMessage*>(new WidgetMessage(m_Name.c_str(), "front")));
			break;
		case 2:
			messages.push(static_cast<IMessage*>(new WidgetMessage(m_Name.c_str(), "top")));
			break;
		case 3:
			messages.push(static_cast<IMessage*>(new WidgetMessage(m_Name.c_str(), "left")));
			break;
		case 4:
			messages.push(static_cast<IMessage*>(new WidgetMessage(m_Name.c_str(), "back")));
			break;
		case 5:
			messages.push(static_cast<IMessage*>(new WidgetMessage(m_Name.c_str(), "bottom")));
			break;
		case 6:
			messages.push(static_cast<IMessage*>(new WidgetMessage(m_Name.c_str(), "right")));
			break;
		default:
			break;
		}
	}
private:
	unsigned int m_State;
};
