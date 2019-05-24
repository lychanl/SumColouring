#include "GISSolver.h"

using namespace SumColouring;

std::vector<int> GISSolver::findColouring(const Graph& g)
{
    std::vector<int> colouring(g.getVertices(), NO_COLOUR);
    std::vector<verticeEdges> edges(g.getVertices());
    for (Graph::Edge e : g.getEdges())
    {
        edges[e.first - 1].insert(e.second - 1);
        edges[e.second - 1].insert(e.first - 1);
    }
    std::set<int> vertices;
    for (int i = 0; i < g.getVertices(); ++i)
        vertices.insert(i);

    findColouring(colouring, vertices, edges, 1);

    return colouring;
}

void GISSolver::findColouring(
            std::vector<int>& colouring,
            std::set<int>& unassignedVertices,
            std::vector<verticeEdges>& edges,
            int nextColour)
{
    std::set<int> nextUnassigned;
    std::vector<int> neighbours(colouring.size(), 0);
	for (int i = 0; i < edges.size(); ++i)
		neighbours[i] = edges[i].size();

    while (unassignedVertices.size() > 0)
    {
        int nextv = -1;
        int minNeighbours = unassignedVertices.size();

        for (int v : unassignedVertices)
        {
            if (neighbours[v] < minNeighbours)
            {
                minNeighbours = neighbours[v];
                nextv = v;
            }
        }

        colouring[nextv] = nextColour;
        // each neighbour of current vertice will different colour
        // so remove them from the graph, as well as their connections to remaining vertices
        // remove edges for current vertice from all further analysis
        unassignedVertices.erase(nextv);
        for (int e : edges[nextv])
        {
            unassignedVertices.erase(e);
            nextUnassigned.insert(e);
            edges[e].erase(nextv);

            for (int ee : edges[e])
            {
                --neighbours[ee];
            }
        }
        edges[nextv].clear();
    }

    if (nextUnassigned.size() > 0)
        findColouring(colouring, nextUnassigned, edges, nextColour + 1);
}