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
        void solveSimplex(Eigen::MatrixXf& canonical, std::vector<int>& basis);

        void pivot(Eigen::MatrixXf& m, int row, int col);
        std::pair<Eigen::MatrixXf, std::vector<int>> getCanonicalWithFeasibleSolution(Eigen::VectorXi& target, Eigen::MatrixXf& constraints);
        bool isResultIntegral(Eigen::MatrixXf& tableau, std::vector<int>& basis);
        Eigen::VectorXi getResult(Eigen::MatrixXf& tableau, std::vector<int>& basis, int size);
    };
}