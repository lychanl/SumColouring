#include "BruteForceSolver.h"
#include <iostream>

using namespace SumColouring;

class ColouringIterator {
private:
    std::vector<int> currentCombination;

	bool isLast = false;
	bool isFirst = true;

    const int minColor = 1;

    int maxColor;

    int activeVertex;

public:
    ColouringIterator(int numberOfVertices, int maxColor) {
        this->currentCombination = std::vector<int>(numberOfVertices, this->minColor);
        this->maxColor = maxColor;

        resetActiveVertex();
    }

    virtual ~ColouringIterator() = default;

    bool hasNext() {
		return !isLast;
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

		this->isLast = true;
		for (int color : this->currentCombination)
			if (color != maxColor)
				this->isLast = false;
    }

    void resetActiveVertex() {
        activeVertex = currentCombination.size() - 1;
    }

    std::vector<int>& next() {
        if (this->isFirst) {
            this->isFirst = false;
            return currentCombination;
        }

        increaseCombination();

        return currentCombination;
    }
};

std::vector<int> BruteForceSolver::findColouring(const Graph &graph) {

    int numberOfVertices = graph.getVertices();

    int minSum = numberOfVertices * numberOfVertices;
    std::vector<int> minCombination(numberOfVertices, numberOfVertices);

    auto colouringIterator = ColouringIterator(numberOfVertices, getMaxColour(graph));
    while (colouringIterator.hasNext()) {
        std::vector<int>& currentCombination = colouringIterator.next();

        if (isAllowedCombination(currentCombination, graph.getEdges())) {
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
                                            const std::set<Graph::Edge> &edges) {
	for (const Graph::Edge& edge : edges)
		if (colorsCombination[edge.first - 1] == colorsCombination[edge.second - 1])
			return false;

    return true;
}

int BruteForceSolver::calculateCombinationSum(std::vector<int> &colorsCombination) {

    int sum = 0;
    for (int color : colorsCombination) {
        sum += color;
    }

    return sum;
}
