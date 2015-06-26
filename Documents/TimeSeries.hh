#pragma once

#include <raindance/Core/Headers.hh>
#include <raindance/Core/Transformation.hh>
#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/Primitives/Grid.hh>
#include <raindance/Core/FS.hh>

class TimeVector
{
public:

    struct Vertex
    {
        Vertex(float time = 0, float value = 0)
        : Time(time), Value(value)
        {
        }

        float Time;
        float Value;
    };

    TimeVector()
    {
        m_FirstUpdate = true;

        FS::TextFile vert("Assets/TimeSeries/timevector.vert");
        FS::TextFile geom("Assets/TimeSeries/timevector.geom");
        FS::TextFile frag("Assets/TimeSeries/timevector.frag");

        m_Shader = ResourceManager::getInstance().loadShader("TimeSeries/TimeVector", vert.content(), frag.content(), geom.content());
        m_Shader->dump();

        update();
    }

    virtual ~TimeVector()
    {
        ResourceManager::getInstance().unload(m_Shader);
    }

    void clear()
    {
        m_VertexBuffer.clear();
    }

    void push(const Vertex& vertex)
    {
        m_VertexBuffer.push(&vertex, sizeof(Vertex));
    }

    void update()
    {
        if (!m_FirstUpdate)
            m_VertexBuffer.update();

        m_VertexBuffer.describe("a_Time", 1, GL_FLOAT, sizeof(Vertex), 0);
        m_VertexBuffer.describe("a_Value", 1, GL_FLOAT, sizeof(Vertex), sizeof(float));

        if (m_FirstUpdate)
            m_VertexBuffer.generate(Buffer::DYNAMIC);

        m_FirstUpdate = false;
    }

    void draw(Context* context, const Camera& camera, Transformation& transformation)
    {
        if (m_VertexBuffer.size() == 0)
            return;

        m_Shader->use();
        m_Shader->uniform("u_ModelViewProjectionMatrix").set(camera.getViewProjectionMatrix() * transformation.state());
        m_Shader->uniform("u_Color").set(glm::vec4(HEX_COLOR(0xC41E3A), 0.75));

        context->geometry().bind(m_VertexBuffer, *m_Shader);
        context->geometry().drawArrays(GL_LINE_STRIP, 0, m_VertexBuffer.size() / sizeof(Vertex));
        context->geometry().unbind(m_VertexBuffer);
    } 

    inline Buffer& getVertexBuffer() { return m_VertexBuffer; }

private:
    Buffer m_VertexBuffer;
    Shader::Program* m_Shader;
    bool m_FirstUpdate;
};

class TimeSeries : public Document::Node
{
public:
	TimeSeries(Document::Node* parent = NULL)
	: Document::Node(parent)
	{       
        Grid::Parameters parameters;
        parameters.Dimension = glm::vec2(100, 100);
        parameters.Scale = glm::vec2(1, 1);

        parameters.Shift = glm::vec2(0, 0);
        parameters.Color = glm::vec4(HEX_COLOR(0x2B3856), 1.0);
        m_Grid = new Grid(parameters);
	}

	virtual ~TimeSeries()
	{
		SAFE_DELETE(m_Grid);

        for (auto v : m_Vectors)
        {
            SAFE_DELETE(v);
        }
	}

	void draw(Context* context) override
	{
        float width = static_cast<float>(this->content().getWidth());
        float height = static_cast<float>(this->content().getHeight());

        m_Camera.setOrthographicProjection(0, width, 0, height, - 10.0, 10.0);
        m_Camera.lookAt(glm::vec3(0, 0, 1.0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        m_Grid->parameters().Dimension = glm::vec2(width, height);
        m_Grid->parameters().Step = glm::vec2(width, height);
        m_Grid->parameters().Division = glm::vec2(10, 10);

        glClear(GL_DEPTH_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);

        Transformation transformation;

        m_Grid->draw(context, m_Camera, transformation);

        for (auto v : m_Vectors)
            v->draw(context, m_Camera, transformation);
	}

	void idle(Context* context) override
	{
		(void) context;
	}

    void request(const Variables& input, Variables& output) override
    {
        //LOG("[TIMESERIES] Request received!\n");
        //input.dump();

        std::string function;
        if (!input.get("function", function))
            return;

        bool error = false;

        if (function == "add")
        {
            auto tv = new TimeVector();
            tv->update();
            auto id = addVector(tv);

            auto var = new IntVariable();
            var->set(id);
            output.set("id", var);
        }
        else if (function == "push")
        {
            long id = -1;
            TimeVector::Vertex vertex;

            error &= input.get("id", &id);
            error &= input.get("time", &vertex.Time);
            error &= input.get("value", &vertex.Value);

            if (!error)
            {
                vector(id)->push(vertex);
                vector(id)->update();
            }
        }
        else if (function == "clear")
        {
            long id = -1;
            TimeVector::Vertex vertex;

            error &= input.get("id", &id);

            if (!error)
            {
                vector(id)->clear();
                vector(id)->update();
            }
        }

        auto var = new IntVariable();
        var->set(error ? -1 : 1);
        output.set("error", var);
    }

    unsigned int addVector(TimeVector* tv)
    {
        m_Vectors.push_back(tv);
        return m_Vectors.size() - 1;
    }

    inline TimeVector* vector(unsigned int index) { return m_Vectors[index]; }

private:
	Camera m_Camera;
    Grid* m_Grid;
    std::vector<TimeVector*> m_Vectors;
};