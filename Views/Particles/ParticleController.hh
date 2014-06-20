#pragma once

class ParticleController : public GraphController
{
public:
    void bind(GraphContext* context, GraphModel* model, ParticleView* view)
    {
        m_GraphContext = context;
        m_GraphModel = model;
        m_GraphView = view;

        int width = glutGet(GLUT_WINDOW_WIDTH);
        int height = glutGet(GLUT_WINDOW_HEIGHT);
        m_GraphView->camera()->setPerspectiveProjection(60.0f, (float)width / (float)height, 0.1f, 1000000.0f);
        m_GraphView->camera()->lookAt(glm::vec3(0, 0, 1000), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        m_SphericalCameraController.bind(m_GraphContext, m_GraphView->camera());
        m_SphericalCameraController.setTarget(glm::vec3(0, 0, 0));
        m_SphericalCameraController.setRadius(1000);
        m_SphericalCameraController.updateCamera();
    }
    virtual void reshape(int width, int height)
    {
        m_SphericalCameraController.reshape(width, height);
    }

    virtual void idle()
    {
        m_SphericalCameraController.updateCamera();
    }

    virtual void draw()
    {
    }

    virtual void onKeyboard(unsigned char key, Controller::KeyEvent event)
    {
        if (event == Controller::KEY_DOWN && key == ' ')
        {
            static bool physics = true;
            m_GraphView->setPhysics(physics);
            physics = !physics;
            return;
        }

        m_SphericalCameraController.onKeyboard(key, event);
    }

    virtual void onMouseDown(int x, int y)
    {
        m_SphericalCameraController.onMouseDown(x, y);
    }

    virtual void onMouseClick(int x, int y)
    {
        m_SphericalCameraController.onMouseClick(x, y);
    }

    virtual void onMouseDoubleClick(int x, int y)
    {
        m_SphericalCameraController.onMouseDoubleClick(x, y);
    }

    virtual void onMouseTripleClick(int x, int y)
    {
         m_SphericalCameraController.onMouseTripleClick(x, y);
    }

    virtual void onMouseMove(int x, int y, int dx, int dy)
    {
        m_SphericalCameraController.onMouseMove(x, y, dx, dy);
    }

    virtual void onSpecial(int key, Controller::KeyEvent event)
    {
        m_SphericalCameraController.onSpecial(key, event);
    }

    void notify(IMessage* message)
    {
        (void) message;
    }

    // GraphListener Interface

    virtual void onSetAttribute(const std::string& name, VariableType type, const std::string& value)
    {
        (void) name;
        (void) type;
        (void) value;
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

    virtual void onSetNodeMark(Node::ID id, unsigned int mark)
    {
        (void) id;
        (void) mark;
    }

    virtual void onSetNodeWeight(Node::ID id, float weight)
    {
        (void) id;
        (void) weight;
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

    virtual void onSetSphereMark(Sphere::ID id, unsigned int mark)
    {
        (void) id;
        (void) mark;
    }
private:
    GraphContext* m_GraphContext;
    GraphModel* m_GraphModel;
    ParticleView* m_GraphView;

    SphericalCameraController m_SphericalCameraController;
};
