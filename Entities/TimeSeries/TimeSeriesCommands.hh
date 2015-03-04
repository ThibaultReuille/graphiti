#pragma once

#include <raindance/Core/Sequencer/Sequencer.hh>

#include "Entities/MVC.hh"

class TimeSeriesCommand : public Sequence
{
public:
    TimeSeriesCommand(TimeSeriesEntity* entity, const char* name)
    : Sequence(name)
    {
        m_TimeSeries = entity;
    }

    virtual ~TimeSeriesCommand() {}

    virtual void start(Timecode timecode)
    {
        (void) timecode;
    }

    virtual void stop(Timecode timecode)
    {
        (void) timecode;
    }

protected:
    TimeSeriesEntity* m_TimeSeries;
};

class TimeSeriesCommand_SetAttribute : public TimeSeriesCommand
{
public:
    TimeSeriesCommand_SetAttribute(TimeSeriesEntity* entity, const char* name, const char* type, const char* value)
    : TimeSeriesCommand(entity, "SetAttribute")
    {
        m_Input.Name = std::string(name);
        m_Input.Type = std::string(type);
        m_Input.Value = std::string(value);
    }

    virtual Sequence::Status play(Timecode timecode)
    {
        (void) timecode;
        m_TimeSeries->setAttribute(m_Input.Name.c_str(), m_Input.Type.c_str(), m_Input.Value.c_str());
        LOG("[COMMAND] SetAttribute{ Input : (%s, %s, %s) }\n", m_Input.Name.c_str(), m_Input.Type.c_str(), m_Input.Value.c_str());
        return KILL;
    }

    struct Input
    {
        std::string Name;
        std::string Type;
        std::string Value;
    } m_Input;
};
