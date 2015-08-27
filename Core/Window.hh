#pragma once

#include <raindance/Core/Interface/Window.hh>
#include <graphiti/Entities/MVC.hh>
#include <graphiti/Entities/Root.hh>

class GLWindow : public rd::Window
{
public:
    struct Settings : rd::Window::Settings
    {
        // TODO: Insert GLWindow specific members here
    };

    GLWindow(Settings* settings, Root* parent = NULL)
    : rd::Window(settings)
    {
        m_ActiveVisualizer = 0;
        m_Parent = parent;
    }

    virtual ~GLWindow()
    {
    }

    virtual void draw(Context* context)
    {
        glClearColor(0.3, 0.3, 0.35, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        body().draw(context);
    }

    virtual void idle(Context* context)
    {
        body().idle(context);
    }

    // ----- Window Events -----

    void onKey(int key, int scancode, int action, int mods) override
    {     
        if (key == GLFW_KEY_M && action == GLFW_PRESS && mods == GLFW_MOD_ALT)
        {
            Geometry::getMetrics().dump();
            Geometry::getMetrics().reset();
            ResourceManager::getInstance().dump();
        }
        else if (key == GLFW_KEY_ENTER && action == GLFW_RELEASE)
        {
            print();
        }
        else
        { 
            body().onKey(key, scancode, action, mods);
        }
    }

    void onChar(unsigned codepoint) override
    {
        body().onChar(codepoint);
    }

    void onScroll(double xoffset, double yoffset) override
    {
        body().onScroll(xoffset, yoffset);
    }

    inline Root* getParent() { return m_Parent; }

private:
    Root* m_Parent;
    std::vector<EntityVisualizer*> m_Visualizers;
    int m_ActiveVisualizer;
};
