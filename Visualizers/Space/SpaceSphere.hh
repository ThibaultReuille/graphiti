#pragma once

#include "Visualizers/Space/SpaceResources.hh"
#include "Visualizers/Space/SpaceWidgets.hh"

class SpaceSphere : public Scene::Node
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
            projection * glm::scale(Geometry::billboard(view * model * getModelMatrix()), glm::vec3(radius, radius, radius)),
            m_Color,
            0
        );
    }

    bool isOverlap (const glm::vec3& min, const glm::vec3& max) const
    {
        (void) min;
        (void) max;

        // TODO : SpaceSphere::isOverlap
        return false;
    }

    inline void setLOD(float lod) { m_LOD = lod; }

private:
    float m_Radius;
    glm::vec4 m_Color;
    float m_LOD;
};
