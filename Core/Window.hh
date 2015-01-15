#pragma once

#include <raindance/Core/GUI/Window.hh>
#include <raindance/Core/GUI/HUD.hh>

#include <graphiti/Entities/MVC.hh>

class GLWindow : public Window
{
public:
    GLWindow(const char* title, const int width, const int height, Root* parent = NULL)
    : Window(title, width, height)
    {
        m_ActiveVisualizer = 0;

        m_HUD = new HUD(getViewport());
        m_HUD->bind(parent->context());

        m_Parent = parent;
    }

    virtual ~GLWindow()
    {
        SAFE_DELETE(m_HUD);
    }

    virtual void draw(Context* context)
    {
        auto visualizer = getActiveVisualizer();
        if (visualizer)
        {
            if (visualizer->view() != NULL)
                visualizer->view()->draw();
            if (visualizer->controller() != NULL)
                visualizer->controller()->draw();
        }

        m_HUD->draw(context);
    }

    virtual void idle(Context* context)
    {
        (void) context;
        
        m_HUD->idle();

        for (auto visualizer : m_Visualizers)
        {
            if (visualizer->view() != NULL)
                visualizer->view()->idle();
            if (visualizer->controller() != NULL)
                visualizer->controller()->idle();
        }
    }

     // ----- TODO : VisualizerManager -----

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

    // ----- Window Events -----

    void onWindowSize(int width, int height) override
    {
        m_HUD->reshape(getViewport());

        for (auto visualizer : m_Visualizers)
            if (visualizer->controller())
                visualizer->controller()->onWindowSize(width, height);
    }

    void onSetFramebufferSize(int width, int height) override
    {
        glViewport(0, 0, width, height);

        m_HUD->reshape(getViewport());

        for (auto visualizer : m_Visualizers)
            if (visualizer->controller())
                visualizer->controller()->onSetFramebufferSize(width, height);
    }

    void onCursorPos(double xpos, double ypos) override
    {
        m_HUD->onCursorPos(xpos, ypos);

        auto visualizer = getActiveVisualizer();
        if (visualizer)
            visualizer->controller()->onCursorPos(xpos, ypos);
    }

    void onMouseButton(int button, int action, int mods) override
    {
        m_HUD->onMouseButton(button, action, mods);

        if (m_HUD->getWidgetPick() == NULL)
        {
            auto visualizer = getActiveVisualizer();
            if (visualizer)
                visualizer->controller()->onMouseButton(button, action, mods);   
        }
    }

    void onKey(int key, int scancode, int action, int mods) override
    {     
         m_HUD->onKey(key, scancode, action, mods);
           
        if (key == GLFW_KEY_N && action == 1 /* DOWN */)
            selectNextVisualizer();
        else if (key == GLFW_KEY_M && action == 1 /* DOWN */)
        {
            Geometry::getMetrics().dump();
            Geometry::getMetrics().reset();
            ResourceManager::getInstance().dump();
        }
        else
        {
            auto visualizer = getActiveVisualizer();
            if (visualizer)
                visualizer->controller()->onKey(key, scancode, action, mods);
        }
    }

    void onScroll(double xoffset, double yoffset) override
    {
        auto visualizer = getActiveVisualizer();
        if (visualizer)
            visualizer->controller()->onScroll(xoffset, yoffset);
    }

    inline HUD* hud() { return m_HUD; }

    inline Root* getParent() { return m_Parent; }

private:
    Root* m_Parent;
    std::vector<EntityVisualizer*> m_Visualizers;
    int m_ActiveVisualizer;
    HUD* m_HUD;
};
