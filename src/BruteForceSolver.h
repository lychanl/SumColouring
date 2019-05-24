#ifndef SUMCOLOURING_BRUTEFORCESOLVER_H
#define SUMCOLOURING_BRUTEFORCESOLVER_H

#include "Solver.h"

namespace SumColouring {

	//class that provides interface for brute-force implementation
    class BruteForceSolver : public Solver {
    public:
		//runs algorithm and returns a vector of colors assigned to given graph
        std::vector<int> findColouring(const Graph &graph) override;


    private:
		// checks if given colours combination satisfies edges constraints
        bool isAllowedCombination(std::vector<int> &colorsCombination, const std::set<Graph::Edge> &relatedVertices);

		// returns chromatic sum for given colouring
        int calculateCombinationSum(std::vector<int> &colorsCombination);
    };

}


#endif //SUMCOLOURING_BRUTEFORCESOLVER_H
