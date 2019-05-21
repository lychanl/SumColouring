#include <sstream>
#include "io.h"

using namespace SumColouring;

Graph SumColouring::readGraph(std::istream &in) {
    int numberOfVertices, numberOFEdges;
    in >> numberOfVertices >> numberOFEdges;

    int v1, v2;
    std::set<Graph::Edge> edges;
    for(int readEdges = 0; readEdges < numberOFEdges; ++readEdges) {
        in >> v1 >> v2;
        edges.insert({v1, v2});
    }

    return Graph(numberOfVertices, std::move(edges));
}

void SumColouring::writeColouring(std::ostream &out, const std::vector<int> &colouring) {
    int currentVertex = 1;
    for(int currentVertexColor : colouring) {
        out << currentVertex << " " << currentVertexColor << std::endl;
        currentVertex++;
    }
}