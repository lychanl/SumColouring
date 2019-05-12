#include "Solver.h"

#include <vector>
#include <algorithm>

using namespace SumColouring;

//Max graph degree + 1
int Solver::getMaxColour(const Graph& g)
{
    std::vector<int> degrees(g.getVertices(), 0);

    for (Graph::Edge e : g.getEdges())
    {
        ++degrees[e.first - 1];
        ++degrees[e.second - 1];
    }

    return *std::max_element(degrees.begin(), degrees.end()) + 1;
}