#pragma once

#include <raindance/Core/GUI/Window.hh>

#include <graphiti/Core/HUD.hh>
#include <graphiti/Entities/MVC.hh>
#include <graphiti/Entities/Root.hh>

class GLWindow : public rd::Window
{
public:
    struct Settings : rd::Window::Settings
    {
        //TODO: Insert GLWindow specific members here
    };

    GLWindow(Settings* settings, Root* parent = NULL)
    : rd::Window(settings)
    {
        m_ActiveVisualizer = 0;

        m_Viewport = getViewport();

        m_HUD = new GraphitiHUD(m_Viewport);
        m_HUD->bind(parent->context());

        m_Parent = parent;
    
        this->onResize(m_Viewport);
    }

    virtual ~GLWindow()
    {
        SAFE_DELETE(m_HUD);
    }

    virtual void draw(Context* context)
    {
        auto framebuffer = m_Viewport.getFramebuffer();

        glViewport(0, framebuffer.Width, 0, framebuffer.Height);

        body().draw();

        m_HUD->draw(context);
    }

    virtual void idle(Context* context)
    {
        (void) context;
        
        body().idle();
        m_HUD->idle();
    }

    // ----- Window Events -----

    void onWindowSize(int width, int height) override
    {
        m_Viewport.setDimension(glm::vec2((float) width, (float) height));
        onResize(m_Viewport);
    }

    void onSetFramebufferSize(int width, int height) override
    {
        m_Viewport.setFramebuffer(width, height);
        onResize(m_Viewport);
    }

    virtual void onResize(const Viewport& viewport)
    {
        auto framebuffer = viewport.getFramebuffer();

        for(auto element : body().elements())
        {
            element->setVirtualDimension(viewport, element->content(), glm::vec2(1.0, 1.0));
        }

        body().arrange(glm::vec3(0.0, 0.0, 0.0), glm::vec3(framebuffer.Width, framebuffer.Height, 0));
        body().onResize(viewport);

        m_HUD->onResize(viewport);
    }

    void onCursorPos(double xpos, double ypos) override
    {
        m_HUD->onCursorPos(xpos, ypos);

        body().onCursorPos(xpos, ypos);
    }

    void onMouseButton(int button, int action, int mods) override
    {
        m_HUD->onMouseButton(button, action, mods);

        body().onMouseButton(button, action, mods);
    }

    void onKey(int key, int scancode, int action, int mods) override
    {     
        if (key == GLFW_KEY_M && action == GLFW_PRESS && mods == GLFW_MOD_ALT)
        {
            Geometry::getMetrics().dump();
            Geometry::getMetrics().reset();
            ResourceManager::getInstance().dump();
        }
        else
        { 
            m_HUD->onKey(key, scancode, action, mods);

            body().onKey(key, scancode, action, mods);
        }
    }

    void onChar(unsigned codepoint) override
    {
        m_HUD->onChar(codepoint);
    }

    void onScroll(double xoffset, double yoffset) override
    {
        body().onScroll(xoffset, yoffset);
    }

    inline GraphitiHUD* hud() { return m_HUD; }

    inline Root* getParent() { return m_Parent; }

private:
    Root* m_Parent;
    std::vector<EntityVisualizer*> m_Visualizers;
    int m_ActiveVisualizer;

    Viewport m_Viewport;

    GraphitiHUD* m_HUD;
};
