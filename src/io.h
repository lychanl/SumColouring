#pragma once
#include "Graph.h"

#include <iostream>
#include <vector>

namespace SumColouring
{
	// reads graph from stream
    Graph readGraph(std::istream& in);
	// writes colouring to the stream
    void writeColouring(std::ostream& out, const std::vector<int>& colouring);
}
