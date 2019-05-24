#pragma once

#include "Solver.h"

namespace SumColouring
{
	//class that provides interface for greedy independent set-based algorithm implementation
    class GISSolver : public Solver
    {
        typedef std::set<int> verticeEdges;
        const int NO_COLOUR = -1;
    public:
		//runs algorithm and returns a vector of colors assigned to given graph
        virtual std::vector<int> findColouring(const Graph&);

    private:
		// assings colours, starting with nextColour, to unassignedVertices
        void findColouring(
            std::vector<int>& colouring,
            std::set<int>& unassignedVertices,
            std::vector<verticeEdges>& edges,
            int nextColour);
    };
}