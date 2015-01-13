#pragma once

class ParticleController : public GraphController
{
public:
    void bind(GraphContext* context, GraphModel* model, ParticleView* view)
    {
        m_GraphContext = context;
        m_GraphModel = model;
        m_GraphView = view;

        m_GraphView->camera()->setPerspectiveProjection(60.0f, view->getViewport().getDimension()[0] / view->getViewport().getDimension()[1], 0.1f, 1000000.0f);
        m_GraphView->camera()->lookAt(glm::vec3(0, 0, 1000), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        m_SphericalCameraController.bind(m_GraphContext, m_GraphView->camera());
        m_SphericalCameraController.setTarget(glm::vec3(0, 0, 0));
        m_SphericalCameraController.setRadius(1000);
        m_SphericalCameraController.updateCamera();
    }

    void idle() override
    {
        m_SphericalCameraController.updateCamera();
    }

    void onWindowSize(int width, int height) override
    {
        m_SphericalCameraController.onWindowSize(width, height);
    }

    void onKey(int key, int scancode, int action, int mods) override
    {
        if (action == GLFW_PRESS && key == GLFW_KEY_SPACE)
        {
            static bool physics = true;
            m_GraphView->setPhysics(physics);
            physics = !physics;
            return;
        }

        m_SphericalCameraController.onKey(key, scancode, action, mods);
    }

    void onMouseDown(const glm::vec2& pos) override
    {
        m_SphericalCameraController.onMouseDown(pos);
    }

    void onMouseClick(const glm::vec2& pos) override
    {
        m_SphericalCameraController.onMouseClick(pos);
    }

    void onMouseDoubleClick(const glm::vec2& pos) override
    {
        m_SphericalCameraController.onMouseDoubleClick(pos);
    }

    void onMouseTripleClick(const glm::vec2& pos) override
    {
         m_SphericalCameraController.onMouseTripleClick(pos);
    }

    void onMouseMove(const glm::vec2& pos, const glm::vec2& dpos) override
    {
        m_SphericalCameraController.onMouseMove(pos, dpos);
    }

    void notify(IMessage* message)
    {
        (void) message;
    }

private:
    GraphContext* m_GraphContext;
    GraphModel* m_GraphModel;
    ParticleView* m_GraphView;

    SphericalCameraController m_SphericalCameraController;
};
