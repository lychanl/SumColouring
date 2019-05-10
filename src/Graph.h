#pragma once
#include <set>

namespace SumColouring
{
    class Graph
    {
    public:
        typedef std::pair<int, int> Edge;

        Graph(int vertices, std::set<Edge> edges) : v(vertices), e(edges) {}

        int getVertices() const { return v; }
        std::set<Edge>& getEdges() { return e; }
        const std::set<Edge>& getEdges() const { return e; }

    private:
        int v;
        std::set<Edge> e;
    };
}
