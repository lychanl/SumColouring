#ifndef SUMCOLOURING_BRUTEFORCESOLVER_H
#define SUMCOLOURING_BRUTEFORCESOLVER_H

#include "Solver.h"

namespace SumColouring {

    class BruteForceSolver : public Solver {
    public:
        std::vector<int> findColouring(const Graph &graph) override;


    private:
        bool isAllowedCombination(std::vector<int> &colorsCombination, const std::set<Graph::Edge> &relatedVertices);

        int calculateCombinationSum(std::vector<int> &colorsCombination);
    };

}


#endif //SUMCOLOURING_BRUTEFORCESOLVER_H
