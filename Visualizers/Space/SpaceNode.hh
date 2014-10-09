#pragma once

#include <raindance/Core/Scene/Node.hh>

#include "Visualizers/Space/SpaceResources.hh"
#include "Visualizers/Space/SpaceWidgets.hh"

class SpaceNode : public Scene::Node
{
public:
    typedef unsigned long ID;

    SpaceNode(const char* label)
    {
        m_ID = 0;
        m_TextureID = 0;
        m_Color = glm::vec4(1.0, 1.0, 1.0, 1.0);
        m_Size = 1.0f;
        m_Mark = 0;
        m_Label.set(label, g_SpaceResources->NodeFont);
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

        if (!g_SpaceResources->isNodeVisible(getLOD()))
            return;

        float nodeSize = getScreenSize();
        glm::mat4 billboard = Geometry::billboard(view * model * getModelMatrix());

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
            float labelRatio = 0.66;

            float fontSize = m_Label.getFont()->getSize();
            float textSize = labelRatio * nodeSize / (fontSize * m_Label.getFont()->getHeight());

            Transformation transformation;
            transformation.set(billboard);
            transformation.translate(glm::vec3(nodeSize / 2 + 0.1, -nodeSize * (1 - labelRatio) / 2, 0.0));
            transformation.scale(glm::vec3(textSize, textSize, 1.0));

            m_Label.setColor(color);
            m_Label.draw(context, projection * transformation.state());
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

    bool isOverlap (const glm::vec3& min, const glm::vec3& max) const
    {
        return Intersection::PointBox(getPosition(), min, max);
    }

    inline void setColor(const glm::vec3& color) { m_Color = glm::vec4(color, 1.0); }
    inline void setColor(const glm::vec4& color) { m_Color = color; }
    inline const glm::vec4& getColor() const { return m_Color; }

    inline void setLabel(const char* label) { m_Label.set(label, g_SpaceResources->NodeFont); }

    inline void setSize(float size) { m_Size = size; }
    inline float getSize() { return m_Size; }
    inline float getScreenSize() { return g_SpaceResources->NodeIconSize * m_Size; }

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

    inline void setID(ID id) { m_ID = id; }
    inline ID getID() { return m_ID; }

private:
    ID m_ID;
    unsigned int m_TextureID;
    glm::vec4 m_Color;
    float m_Size;
    unsigned int m_Mark;
    Text m_Label;
    float m_Activity;
};
