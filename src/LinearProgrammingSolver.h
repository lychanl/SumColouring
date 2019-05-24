#pragma once

#include "Solver.h"

#include <Eigen>

namespace SumColouring
{
	//class that provides interface for linear programming-based algorithm implementation
    class LinearProgrammingSolver : public Solver
    {
    public:
		//runs algorithm and returns a vector of colors assigned to given graph
        virtual std::vector<int> findColouring(const Graph&);

    private:
        // Problem transformation - returns optimized function
        Eigen::VectorXi getTarget(const Graph&);
		// Problem transformation - returns constraints
        Eigen::MatrixXi getConstraints(const Graph&);
		// Problem transformation - returns colouring for given LP result
        std::vector<int> getColouring(const Graph&, Eigen::VectorXi&);

        // Solves integer linear programming problem using simplex algorithm
        Eigen::VectorXi solveSimplex(Eigen::VectorXi& target, Eigen::MatrixXi& constraints);
		// Solves linear simplex for given tableau and basis
        void solveSimplex(Eigen::MatrixXi& canonical, std::vector<int>& basis);

		// Pivot operation
        void pivot(Eigen::MatrixXi& m, std::vector<int>& basis, int row, int col);
		// Finds initial valid solution for given LP problem
        std::pair<Eigen::MatrixXi, std::vector<int>> getCanonicalWithFeasibleSolution(Eigen::VectorXi& target, Eigen::MatrixXi& constraints);
		// Returns if all variables have integer values
        bool isResultIntegral(Eigen::MatrixXi& tableau, std::vector<int>& basis);
		// returns vector of values of variables for given simplex solution.
		// returns only last size variables
        Eigen::VectorXi getResult(Eigen::MatrixXi& tableau, std::vector<int>& basis, int size);
    };
}