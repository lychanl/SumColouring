#pragma once
#include "Graph.h"

#include <iostream>
#include <vector>

namespace SumColouring
{
    Graph readGraph(std::istream& in);
    void writeColouring(std::ostream& out, const std::vector<int>& colouring);
}
