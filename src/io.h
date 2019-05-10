#pragma once
#include "Graph.h"

#include <iostream>
#include <vector>

namespace SumColouring
{
    Graph readGraph(std::istream stream);
    void writeColouring(std::ostream stream, std::vector<int> colouring);
}
