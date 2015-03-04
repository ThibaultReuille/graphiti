#pragma once

#include "Visualizers/Space/SpaceResources.hh"
#include "Visualizers/Space/SpaceWidgets.hh"

class SpaceEdge : public Scene::Node
{
public:
    typedef unsigned long ID;

    SpaceEdge(Scene::Node* node1, Scene::Node* node2)
    : Scene::Node(), m_Node1(node1), m_Node2(node2)
    {
        m_Activity = 0.0f;
        m_TextureID = 0;
        update();
    }

    virtual ~SpaceEdge()
    {
    }

    void draw(Context* context, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model)
    {
        glm::vec4 color = glm::vec4(0.5, 0.5, 0.5, 1.0);

        if (!g_SpaceResources->isEdgeVisible(getLOD()))
            return;

        if (g_SpaceResources->m_EdgeMode != SpaceResources::OFF)
        {
            float width = g_SpaceResources->EdgeSize * m_WideLine.getWidth();
            if (width > 0.0)
            {
                Buffer& vertexBuffer = m_WideLine.getVertexBuffer();
                glm::vec3 extrusion;

                switch(g_SpaceResources->m_EdgeMode)
                {
                case SpaceResources::LINES:

                    // TODO : Figure out why this code crashes with large line sets

                    glLineWidth(width);

                    g_SpaceResources->EdgeShader->use();
                    g_SpaceResources->EdgeShader->uniform("u_Mode").set(0.0f);
                    g_SpaceResources->EdgeShader->uniform("u_ModelViewProjection").set(projection * view * model * getModelMatrix());
                    g_SpaceResources->EdgeShader->uniform("u_StartPosition").set(m_WideLine.getPosition(0));
                    g_SpaceResources->EdgeShader->uniform("u_EndPosition").set(m_WideLine.getPosition(1));
                    g_SpaceResources->EdgeShader->uniform("u_Tint").set(color);
                    context->geometry().bind(vertexBuffer, *(g_SpaceResources->EdgeShader));
                    context->geometry().drawArrays(GL_LINES, 0, 2 * sizeof(WideLine::Vertex));
                    context->geometry().unbind(vertexBuffer);

                    glLineWidth(1.0);
                    break;

                case SpaceResources::WIDE_LINES:
                    extrusion = 0.25f * g_SpaceResources->EdgeSize * m_WideLine.calculateExtrudeDirection(*context->getCamera());

                    g_SpaceResources->EdgeShader->use();
                    g_SpaceResources->EdgeShader->uniform("u_Mode").set(1.0f);
                    g_SpaceResources->EdgeShader->uniform("u_ModelViewProjection").set(projection * view * model * getModelMatrix());
                    g_SpaceResources->EdgeShader->uniform("u_StartPosition").set(m_WideLine.getPosition(0));
                    g_SpaceResources->EdgeShader->uniform("u_EndPosition").set(m_WideLine.getPosition(1));
                    g_SpaceResources->EdgeShader->uniform("u_ExtrudeDirection").set(extrusion);
                    g_SpaceResources->EdgeShader->uniform("u_Tint").set(color);
                    g_SpaceResources->EdgeShader->uniform("u_Texture").set(g_SpaceResources->EdgeStyleIcon->getTexture(m_TextureID));

                    context->geometry().bind(vertexBuffer, *(g_SpaceResources->EdgeShader));
                    context->geometry().drawArrays(GL_TRIANGLE_STRIP, 0, vertexBuffer.size() / sizeof(WideLine::Vertex));
                    context->geometry().unbind(vertexBuffer);
                    break;

                case SpaceResources::OFF:
                default:
                    break;
                }
            }
        }

        if (g_SpaceResources->ShowEdgeActivity && m_Activity > 0.0f)
        {
            float t = fmod(m_Activity * context->clock().seconds(), 1.0f);
            glm::vec3 v = BezierCurve::linear(m_Node1->getPosition(), m_Node2->getPosition(), t);
            glm::vec4 c = BezierCurve::linear(m_WideLine.getColor(0), m_WideLine.getColor(1), t);
            c.a *= color.a;

            float iconSize = 2.0 * g_SpaceResources->EdgeSize;
            glm::mat4 billboard = Geometry::billboard(view * glm::translate(model * getModelMatrix(), v));

            g_SpaceResources->EdgeActivityIcon->draw(context, projection * glm::scale(billboard, glm::vec3(iconSize, iconSize, iconSize)), c, 0);
        }
    }

    bool isOverlap (const glm::vec3& min, const glm::vec3& max) const
    {
        return Intersection::SegmentBox(m_WideLine.getPosition(0), m_WideLine.getPosition(1), min, max);
    }

    void update()
    {
        glm::vec3 nodePosition[2] =
        {
                m_Node1->getPosition(),
                m_Node2->getPosition()
        };

        glm::vec3 linePosition[2] =
        {
                m_WideLine.getPosition(0),
                m_WideLine.getPosition(1)
        };

        bool needsPositionUpdate = false;
        if (linePosition[0] != nodePosition[0] || linePosition[1] != nodePosition[1])
            needsPositionUpdate = true;

        if (g_SpaceResources->m_LinkMode == SpaceResources::LINK_COLOR)
        {
            if (m_Dirty || needsPositionUpdate)
            {
                m_WideLine.setPosition(0, nodePosition[0]);
                m_WideLine.setPosition(1, nodePosition[1]);
                m_WideLine.update();
            }
        }
        else if (g_SpaceResources->m_LinkMode == SpaceResources::NODE_COLOR)
        {
            glm::vec4 lineColor[2] =
            {
                    m_WideLine.getColor(0),
                    m_WideLine.getColor(1)
            };

            glm::vec4 nodeColor[2];

            nodeColor[0] = static_cast<SpaceNode*>(m_Node1)->getColor();
            nodeColor[1] = static_cast<SpaceNode*>(m_Node2)->getColor();

            bool needsColorUpdate = false;
            if (lineColor[0] != nodeColor[0] || lineColor[1] != nodeColor[1])
                needsColorUpdate = true;

            if (needsPositionUpdate || needsColorUpdate)
            {
                m_WideLine.setPosition(0, nodePosition[0]);
                m_WideLine.setPosition(1, nodePosition[1]);
                m_WideLine.setColor(0, nodeColor[0]);
                m_WideLine.setColor(1, nodeColor[1]);
                m_WideLine.update();
            }
        }

        m_Dirty = false;
    }

    inline SpaceNode::ID getNode1() { return static_cast<SpaceNode*>(m_Node1)->getID(); }
    inline SpaceNode::ID getNode2() { return static_cast<SpaceNode*>(m_Node2)->getID(); }

    inline void setWidth(float width) { m_WideLine.setWidth(width); }
    inline float getWidth() { return m_WideLine.getWidth(); }

    inline void setActivity(float activity) { m_Activity = activity; }
    inline float getActivity() { return m_Activity; }

    void setColor(unsigned int vertex, const glm::vec4& color)
    {
        m_WideLine.setColor(vertex, color);
        g_SpaceResources->m_LinkMode = SpaceResources::LINK_COLOR;
    }

    inline glm::vec4 getColor(unsigned int vertex) { return m_WideLine.getColor(vertex); }

    inline void setDirty(bool dirty) { m_Dirty = dirty; }

    void setIcon(const std::string& name)
    {
        unsigned long id;
        if (!g_SpaceResources->EdgeStyleIcon->getTexture(name.c_str(), &id))
        {
            LOG("[SPACE] Icon '%s' not found!\n", name.c_str());
            return;
        }

        m_TextureID = static_cast<unsigned int>(id);
    }

private:
    Scene::Node* m_Node1;
    Scene::Node* m_Node2;
    WideLine m_WideLine;
    unsigned int m_TextureID;
    bool m_Dirty;
    float m_Activity;
};
