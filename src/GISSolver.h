#pragma once

#include "Solver.h"

namespace SumColouring
{
    class GISSolver : public Solver
    {
        typedef std::set<int> verticeEdges;
        const int NO_COLOUR = -1;
    public:
        virtual std::vector<int> findColouring(const Graph&);

    private:
        void findColouring(
            std::vector<int>& colouring,
            std::set<int>& unassignedVertices,
            std::vector<verticeEdges>& edges,
            int nextColour);
    };
}