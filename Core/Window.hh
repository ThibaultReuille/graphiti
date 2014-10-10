#pragma once

#include <raindance/Core/GUI/Window.hh>
#include "Entities/MVC.hh"

class GraphitiWindow : public Window
{
public:
    GraphitiWindow(const char* title, const int width, const int height)
    : Window(title, width, height)
    {
        m_ActiveVisualizer = 0;
    }

    virtual ~GraphitiWindow()
    {
    }

    virtual void draw(Context* context)
    {
        clear();

        preDraw(context);

        m_Visualizers[m_ActiveVisualizer]->view()->draw();
        m_Visualizers[m_ActiveVisualizer]->controller()->draw();

        postDraw(context);
    }

    inline void addVisualizer(EntityVisualizer* visualizer)
    {
        m_Visualizers.push_back(visualizer);
    }

    inline EntityVisualizer* getActiveVisualizer()
    {
        if (m_Visualizers.empty())
            return NULL;
        return m_Visualizers[m_ActiveVisualizer];
    }

    inline void selectNextVisualizer()
    {
        if (!m_Visualizers.empty())
            m_ActiveVisualizer = (m_ActiveVisualizer + 1) % m_Visualizers.size();
    }

private:
    std::vector<EntityVisualizer*> m_Visualizers;
    int m_ActiveVisualizer;
};
