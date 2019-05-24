#pragma once
#include <set>

namespace SumColouring
{
	// class for graph
    class Graph
    {
    public:
        typedef std::pair<int, int> Edge;
		
		// creates graph with given number of vertices and given edges
        Graph(int vertices, std::set<Edge>&& edges) : v(vertices), e(edges) {}

		// returns number of vertices
        int getVertices() const { return v; }
		// returns edges
        std::set<Edge>& getEdges() { return e; }
		// returns edges
        const std::set<Edge>& getEdges() const { return e; }

    private:
        int v;
        std::set<Edge> e;
    };
}
