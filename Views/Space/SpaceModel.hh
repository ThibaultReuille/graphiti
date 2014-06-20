#pragma once

#include "Views/Space/SpaceResources.hh"
#include "Views/Space/SpaceWidgets.hh"

class SpaceNode : public Scene::IDrawable
{
public:
    typedef unsigned long ID;

    SpaceNode(ID id, const char* label)
    {
        m_ID = id;
        m_TextureID = 0;
        m_Color = glm::vec4(1.0, 1.0, 1.0, 1.0);
        m_Size = 1.0f;
        m_Mark = 0;
        m_Label.set(label, g_SpaceResources->NodeFont);
        m_LOD = 1.0;
        m_Activity = 0.0;
    }

    virtual ~SpaceNode()
    {
    }

    void draw(Context* context, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model)
    {
        if (g_SpaceResources->ShowNodeShapes == SpaceResources::NONE && !g_SpaceResources->ShowNodeLabels)
            return;

        glm::vec4 color = m_Color;

        if (g_SpaceResources->ShowNodeLOD)
        {
            if (m_LOD == 0.0)
                return;

            color.a = m_LOD;
            // TODO : Smart LOD filters
            // g_SpaceResources->getTentFilter(m_LOD, 1.0, g_SpaceResources->CurrentLOD, g_SpaceResources->LODSlice);
            if (color.a > 1.0)
                color.a = 1.0;
        }

        float nodeSize = g_SpaceResources->NodeIconSize * m_Size;
        glm::mat4 billboard = Geometry::billboard(view * model);

        if (g_SpaceResources->ShowNodeShapes == SpaceResources::ALL || g_SpaceResources->ShowNodeShapes == SpaceResources::COLORS)
        {
            g_SpaceResources->NodeIcon->draw(context, projection * glm::scale(billboard, glm::vec3(nodeSize, nodeSize, nodeSize)), color, m_TextureID);
        }

        if ((g_SpaceResources->ShowNodeShapes == SpaceResources::ALL || g_SpaceResources->ShowNodeShapes == SpaceResources::MARKS) && m_Mark > 0)
        {
            glPolygonOffset(-1, -1);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glm::vec4 markerColor = MarkerWidget::color(m_Mark);
            markerColor.a = color.a;
            g_SpaceResources->NodeMarkIcon->draw(context, projection * glm::scale(billboard, glm::vec3(nodeSize, nodeSize, nodeSize)), markerColor, 0);
            glDisable(GL_POLYGON_OFFSET_FILL);
        }

        if (g_SpaceResources->ShowNodeLabels)
        {
            float textSize = 0.03 * (0.1 + nodeSize);
            float fontHeight = 18.0; // TODO : Find the font value

            glm::mat4 textMatrix;
            textMatrix = glm::translate(billboard, glm::vec3(0.1 + nodeSize / 2, 0.0, 0));
            textMatrix = glm::scale(textMatrix, glm::vec3(textSize, textSize, textSize));
            textMatrix = glm::translate(textMatrix, glm::vec3(0.0, fontHeight / 2, 0.0));
            m_Label.setColor(color);
            m_Label.draw(context, projection * textMatrix);
        }

        if (g_SpaceResources->ShowNodeActivity && m_Activity > 0.0f)
        {
            float maxScale = 5.0;
            float t = 1.0 + fmod(context->clock().seconds() * m_Activity, maxScale);
            float alpha = 1.0 - (t - 1.0) / maxScale;
            float activitySize = nodeSize * t;
            glm::vec4 activityColor = glm::vec4(color.r, color.g, color.b, alpha);

            g_SpaceResources->NodeActivityIcon->draw(context, projection * glm::scale(billboard, glm::vec3(activitySize, activitySize, activitySize)), activityColor, 0);
        }
    }

    inline void setColor(const glm::vec3& color) { m_Color = glm::vec4(color, 1.0); }
    inline void setColor(const glm::vec4& color) { m_Color = color; }
    inline const glm::vec4& getColor() const { return m_Color; }

    inline void setLabel(const char* label) { m_Label.set(label, g_SpaceResources->NodeFont); }

    inline void setMark(unsigned int mark) { m_Mark = mark; }
    inline unsigned int getMark() { return m_Mark; }

    inline void setLOD(float lod) { m_LOD = lod; }
    inline float getLOD() { return m_LOD; }

    inline void setSize(float size) { m_Size = size; }
    inline float getSize() { return m_Size; }

    inline void setActivity(float activity) { m_Activity = activity; }
    inline float getActivity() { return m_Activity; }

    void setIcon(const std::string& name)
    {
        unsigned long id;
        if (!g_SpaceResources->NodeIcon->getTexture(name.c_str(), &id))
        {
            LOG("[SPACE] Icon '%s' not found!\n", name.c_str());
            return;
        }

        m_TextureID = static_cast<unsigned int>(id);
    }

    inline ID getID() { return m_ID; }

private:
    ID m_ID;
    unsigned int m_TextureID;
    glm::vec4 m_Color;
    float m_Size;
    unsigned int m_Mark;
    Text m_Label;
    float m_LOD;
    float m_Activity;
};

class SpaceLink : public Scene::IDrawable
{
public:
    typedef unsigned long ID;

    SpaceLink(Scene::Node* node1, Scene::Node* node2)
    : m_Node1(node1), m_Node2(node2)
    {
        m_Activity = 0.0f;
        m_LOD = 1.0f;
        update();
    }

    virtual ~SpaceLink()
    {
    }

    void draw(Context* context, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model)
    {
        glm::vec4 color = glm::vec4(0.5, 0.5, 0.5, 1.0);

        if (g_SpaceResources->ShowLinkLOD)
        {
            if (m_LOD == 0.0)
                return;

            color.a = m_LOD;
            // TODO : Smart LOD filters
            // g_SpaceResources->getTentFilter(m_LOD, 1.0, g_SpaceResources->CurrentLOD, g_SpaceResources->LODSlice);
            if (color.a > 1.0)
                color.a = 1.0;
        }

        if (g_SpaceResources->ShowEdges)
        {
            Buffer& vertexBuffer = m_Line.getVertexBuffer();

            glLineWidth(g_SpaceResources->EdgeSize);

            // TODO : Figure out why this code crashes with large line sets

            g_SpaceResources->LinkShader->use();
            g_SpaceResources->LinkShader->uniform("u_ModelViewProjection").set(projection * view * model);
            g_SpaceResources->LinkShader->uniform("u_Tint").set(color);
            context->geometry().bind(vertexBuffer, *(g_SpaceResources->LinkShader));
            context->geometry().drawArrays(GL_LINES, 0, vertexBuffer.size() / sizeof(Line::Vertex));
            context->geometry().unbind(vertexBuffer);

            glLineWidth(1.0);
        }

        if (g_SpaceResources->ShowEdgeActivity && m_Activity > 0.0f)
        {
            float t = fmod(m_Activity * context->clock().seconds(), 1.0f);
            glm::vec3 v = BezierCurve::linear(m_Node1->getPosition(), m_Node2->getPosition(), t);
            glm::vec4 c = BezierCurve::linear(m_Line.getColor(0), m_Line.getColor(1), t);
            c.a *= color.a;

            float nodeSize = g_SpaceResources->NodeIconSize;
            glm::mat4 billboard = Geometry::billboard(view * glm::translate(model, v));

            g_SpaceResources->LinkActivityIcon->draw(context, projection * glm::scale(billboard, glm::vec3(nodeSize, nodeSize, nodeSize)), c, 0);
        }
    }

    void update()
    {
        Line::Vertex v1;
        Line::Vertex v2;

        m_Line.get(&v1, &v2);

        glm::vec3 nodePosition[2];

        nodePosition[0] = m_Node1->getPosition();
        nodePosition[1] = m_Node2->getPosition();

        bool needsPositionUpdate = false;
        if (v1.Position != nodePosition[0] || v2.Position != nodePosition[1])
            needsPositionUpdate = true;

        if (g_SpaceResources->m_LinkMode == SpaceResources::LINK_COLOR)
        {
            if (needsPositionUpdate)
                m_Line.setPositions(nodePosition[0], nodePosition[1]);
        }
        else if (g_SpaceResources->m_LinkMode == SpaceResources::NODE_COLOR)
        {
            glm::vec4 nodeColor[2];

            nodeColor[0] = static_cast<SpaceNode*>(m_Node1->getDrawable())->getColor();
            nodeColor[1] = static_cast<SpaceNode*>(m_Node2->getDrawable())->getColor();

            bool needsColorUpdate = false;
            if (v1.Color != nodeColor[0] || v2.Color != nodeColor[1])
                needsColorUpdate = true;

            if (needsPositionUpdate || needsColorUpdate)
                m_Line.set(Line::Vertex(m_Node1->getPosition(), nodeColor[0]),
                           Line::Vertex(m_Node2->getPosition(), nodeColor[1]));
        }
    }

    inline SpaceNode::ID getNode1() { return static_cast<SpaceNode*>(m_Node1->getDrawable())->getID(); }
    inline SpaceNode::ID getNode2() { return static_cast<SpaceNode*>(m_Node2->getDrawable())->getID(); }

    inline void setActivity(float activity) { m_Activity = activity; }
    inline float getActivity() { return m_Activity; }

    void setColor(unsigned int vertex, const glm::vec3& color)
    {
        m_Line.setColor(vertex, glm::vec4(color, 1.0));
        g_SpaceResources->m_LinkMode = SpaceResources::LINK_COLOR;
    }

    void setColor(unsigned int vertex, const glm::vec4& color)
    {
        m_Line.setColor(vertex, color);
        g_SpaceResources->m_LinkMode = SpaceResources::LINK_COLOR;
    }

    inline glm::vec4 getColor(unsigned int vertex) { return m_Line.getColor(vertex); }

    inline void setLOD(float lod) { m_LOD = lod; }

private:
    Scene::Node* m_Node1;
    Scene::Node* m_Node2;
    Line m_Line;
    float m_Activity;
    float m_LOD;
};

class SpaceSphere : public Scene::IDrawable
{
public:
    typedef unsigned long ID;

    SpaceSphere()
    {
        m_Radius = 0.5;
        m_Color = glm::vec4(1.0, 1.0, 1.0, 1.0);
        m_LOD = 0.0f;
    }
    ~SpaceSphere()
    {
    }

    inline void setRadius(float radius) { m_Radius = radius; }
    inline void setColor(const glm::vec3& color) { m_Color = glm::vec4(color, 1.0); }
    inline void setColor(const glm::vec4& color) { m_Color = color; }

    void draw(Context* context, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model)
    {
        float radius = 4 * (m_Radius + 0.5f);
        g_SpaceResources->SphereIcon->draw
        (
            context,
            projection * glm::scale(Geometry::billboard(view * model), glm::vec3(radius, radius, radius)),
            m_Color,
            0
        );
    }

    inline void setLOD(float lod) { m_LOD = lod; }

private:
	float m_Radius;
	glm::vec4 m_Color;
	float m_LOD;
};

