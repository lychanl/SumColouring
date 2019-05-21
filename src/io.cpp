#include <sstream>
#include "io.h"
#include <memory>

using namespace SumColouring;

std::unique_ptr<std::pair<int, int>> parseLine(const std::string &line) {
    std::stringstream ss(line);

    std::string temp;
    ss >> temp;
    int firstColumn = strtol(temp.c_str(), nullptr, 10);
    ss >> temp;
    int secondColumn = strtol(temp.c_str(), nullptr, 10);

    return std::unique_ptr<std::pair<int, int>>(new std::pair<int, int>{firstColumn, secondColumn});
}

std::unique_ptr<std::pair<int, int>> readNextLine(std::istream &in) {
    std::string linebuffer;
    while (in && getline(in, linebuffer)) {
        if (linebuffer.length() == 0) {
            continue;
        }

        return parseLine(linebuffer);
    }

    return nullptr;
}

Graph SumColouring::readGraph(std::istream &in) {

    std::set<Graph::Edge> edges;

    auto line = readNextLine(in);
    int numberOfVertices = line->first;
    int numberOFEdges = line->second;

    int readEdges = 0;
    while(readEdges < numberOFEdges) {
        line = readNextLine(in);

        edges.insert(*line);

        ++readEdges;
    }

    return Graph(numberOfVertices, edges);
}





void SumColouring::writeColouring(std::ostream &out, const std::vector<int> &colouring) {
    int currentVertex = 1;
    for(int currentVertexColor : colouring) {
        out << currentVertex << " " << currentVertexColor << std::endl;
    }
}