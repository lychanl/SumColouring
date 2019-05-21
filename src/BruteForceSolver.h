#ifndef SUMCOLOURING_BRUTEFORCESOLVER_H
#define SUMCOLOURING_BRUTEFORCESOLVER_H

#include "Solver.h"

namespace SumColouring {

    class BruteForceSolver : public Solver {
    public:
        std::vector<int> findColouring(const Graph &graph) override;


    private:
        bool isAllowedCombination(std::vector<int> *colorsCombination, std::vector<std::set<int>> *relatedVertices);

        int calculateCombinationSum(std::vector<int> *pVector);
    };

}


#endif //SUMCOLOURING_BRUTEFORCESOLVER_H
