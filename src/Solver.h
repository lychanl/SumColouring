#pragma once
#include "Graph.h"

#include <string>
#include <vector>
#include <memory>

namespace SumColouring
{
	//abstract interface for class implementing algortihm that solves sum colouring problem
    class Solver
    {
    public:
		//returns object of a solver class that implements given algorithm, or nullptr if invalid name
        static std::unique_ptr<Solver> create(const std::string& name);
        
		//runs algorithm and returns a vector of colors assigned to given graph
        virtual std::vector<int> findColouring(const Graph&) = 0;

    protected:
		//returns upper bound for colour in an optimal colouring of given graph
        int getMaxColour(const Graph&);
    };
}