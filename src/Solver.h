#pragma once
#include "Graph.h"

#include <string>
#include <vector>
#include <memory>

namespace SumColouring
{
    class Solver
    {
    public:
        static std::unique_ptr<Solver> create(const std::string& name);
        
        virtual std::vector<int> findColouring(const Graph&) = 0;

    protected:
        int getMaxColour(const Graph&);
    };
}