#include "Solver.h"
#include "BruteForceSolver.h"
#include "GISSolver.h"
#include "LinearProgrammingSolver.h"
#include "MaxSATSolver.h"

#include <vector>
#include <algorithm>

using namespace SumColouring;

std::unique_ptr<Solver> Solver::create(const std::string& name)
{
	if (name == "brute-force")
		return std::unique_ptr<Solver>(new BruteForceSolver);
	if (name == "gis")
		return std::unique_ptr<Solver>(new GISSolver);
	if (name == "linear")
		return std::unique_ptr<Solver>(new LinearProgrammingSolver);
	if (name == "maxsat")
		return std::unique_ptr<Solver>(new MaxSATSolver);
	return std::unique_ptr<Solver>(nullptr);
}

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