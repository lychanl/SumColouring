#pragma once

#include "Solver.h"

#include <Eigen>

namespace SumColouring
{
    class LinearProgrammingSolver : public Solver
    {
        const float EPSILON = 0.0001f;
    public:
        virtual std::vector<int> findColouring(const Graph&);

    private:
        // Problem transformation
        Eigen::VectorXi getTarget(const Graph&);
        Eigen::MatrixXi getConstraints(const Graph&);
        std::vector<int> getColouring(const Graph&, Eigen::VectorXi&);

        // Actual solution
        Eigen::VectorXi solveSimplex(Eigen::VectorXi& target, Eigen::MatrixXi& constraints);
        void solveSimplex(Eigen::MatrixXi& canonical, std::vector<int>& basis);

        void pivot(Eigen::MatrixXi& m, std::vector<int>& basis, int row, int col);
        std::pair<Eigen::MatrixXi, std::vector<int>> getCanonicalWithFeasibleSolution(Eigen::VectorXi& target, Eigen::MatrixXi& constraints);
        bool isResultIntegral(Eigen::MatrixXi& tableau, std::vector<int>& basis);
        Eigen::VectorXi getResult(Eigen::MatrixXi& tableau, std::vector<int>& basis, int size);
    };
}