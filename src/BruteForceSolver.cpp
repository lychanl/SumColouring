#include "BruteForceSolver.h"
#include <cmath>
#include <iostream>

using namespace SumColouring;

class ColouringIterator {
private:
    std::vector<int> currentCombination;

    long maxColouringCombinations;

    int processedColouringCombinations = 0;

    const int minColor = 1;

    int maxColor;

    int activeVertex;

public:
    ColouringIterator(int numberOfVertices) {
        this->currentCombination = std::vector<int>(numberOfVertices, this->minColor);
        this->maxColouringCombinations = (long) powl(numberOfVertices, numberOfVertices);
        this->maxColor = numberOfVertices;

        resetActiveVertex();
    }

    virtual ~ColouringIterator() = default;

    bool hasNext() {
        return processedColouringCombinations != maxColouringCombinations;
    }

    void increaseCombination() {
        int nextActiveVertex = this->activeVertex;
        while (nextActiveVertex >= 0 && this->currentCombination[nextActiveVertex] == maxColor) {
            this->currentCombination[nextActiveVertex] = this->minColor;
            nextActiveVertex--;
        }

        if (nextActiveVertex < 0) {
            throw std::runtime_error("Iterator overflow!");
        }

        this->currentCombination[nextActiveVertex]++;

        if (nextActiveVertex != this->activeVertex) {
            resetActiveVertex();
        }

        this->processedColouringCombinations++;
    }

    void resetActiveVertex() {
        activeVertex = currentCombination.size() - 1;
    }

    std::vector<int> next() {
        if (processedColouringCombinations == 0) {
            processedColouringCombinations++;
            return currentCombination;
        }

        increaseCombination();

        return currentCombination;
    }
};

std::vector<int> BruteForceSolver::findColouring(const Graph &graph) {

    int numberOfVertices = graph.getVertices();
    std::vector<std::set<int>> relatedVertices(numberOfVertices);
    for (Graph::Edge e : graph.getEdges()) {
        relatedVertices[e.first - 1].insert(e.second - 1);
        relatedVertices[e.second - 1].insert(e.first - 1);
    }

    int minSum = numberOfVertices * numberOfVertices;
    std::vector<int> minCombination(numberOfVertices, numberOfVertices);

    auto colouringIterator = ColouringIterator(numberOfVertices);
    while (colouringIterator.hasNext()) {
        std::vector<int> currentCombination = colouringIterator.next();

        if (isAllowedCombination(currentCombination, relatedVertices)) {
            int combinationSum = calculateCombinationSum(currentCombination);
            if (combinationSum < minSum) {
                minSum = combinationSum;
                minCombination = currentCombination;
            }
        }
    }

    return minCombination;
}

bool BruteForceSolver::isAllowedCombination(std::vector<int> &colorsCombination,
                                            std::vector<std::set<int>> &relatedVertices) {

    for (int currentVertex = 0; currentVertex < colorsCombination.size(); ++currentVertex) {
        int currentVertexColor = colorsCombination[currentVertex];

        for (int currentRelatedVertex : relatedVertices[currentVertex]) {
            if (colorsCombination[currentRelatedVertex] == currentVertexColor) {
                return false;
            }
        };
    }

    return true;
}

int BruteForceSolver::calculateCombinationSum(std::vector<int> &colorsCombination) {

    int sum = 0;
    for (int color : colorsCombination) {
        sum += color;
    }

    return sum;
}
