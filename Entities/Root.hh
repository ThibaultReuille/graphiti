#pragma once

#include <graphiti/Entities/MVC.hh>
#include <graphiti/Core/Console.hh>

class Root
{
public:
    virtual Context* context() = 0;
    virtual GraphitiConsole* console() = 0;
    virtual EntityManager& entities() = 0;
    virtual EntityVisualizerManager& visualizers() = 0;
};
