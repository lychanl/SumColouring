#pragma once
#include "Graph.h"

#include <string>
#include <vector>

namespace SumColouring
{
    class Solver
    {
    public:
        static Solver& create(std::string algorithm);
        
        virtual std::vector<int> findColouring(const Graph&) = 0;

    protected:
        int getMaxColour(const Graph&);
    };
}