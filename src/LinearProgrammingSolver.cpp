#include "LinearProgrammingSolver.h"

#include <cmath>

using namespace SumColouring;

// using https://people.orie.cornell.edu/dpw/orie6300/Lectures/lec12.pdf
// as reference for finding initial feasible solution
// http://www.ou.edu/class/che-design/che5480-11/Gomory%20Cuts-The%20How%20and%20the%20Why.pdf
// as reference for finding integer solution (Gomory Cuts)
// and http://fourier.eng.hmc.edu/e176/lectures/NM/node32.html
// as reference for Simplex algorithm

#include <iostream>

std::vector<int> LinearProgrammingSolver::findColouring(const Graph& g)
{
    Eigen::VectorXi target = getTarget(g);
	std::cout << "TARGET\n" << target << std::endl;
    Eigen::MatrixXi constraints = getConstraints(g);
	std::cout << "CONSTR\n" << constraints << std::endl;
    Eigen::VectorXi solution = solveSimplex(target, constraints);
	std::cout << "SOLUTION\n" << solution << std::endl;
    return getColouring(g, solution);
}

Eigen::VectorXi LinearProgrammingSolver::getTarget(const Graph& g)
{
    /* Variable order is: (x_{ic} for i from 1 to |V|) for c from 1 to maxc
     */
    int maxc = getMaxColour(g);
    int size = g.getVertices() * maxc;
    Eigen::VectorXi target(size);

    for (int i = 0; i < maxc; ++i)
        for (int j = 0; j < g.getVertices(); ++j)
            target(i * g.getVertices() + j) = i + 1;

    return target;
}

Eigen::MatrixXi LinearProgrammingSolver::getConstraints(const Graph& g)
{
    /* Constraints:
     * for each vertice i sum{c <= maxc}x_{ic} = 1
     * for each edge between i and j for each colour c x_{ic} + x{jc} <= 1
     * for each variable x_{ic} x_{ic} >= 0
     * 
     * Which gives standard form:
     * for each vertice i sum{c <= maxc}x_{ic} = 1
     * for each edge e between i and j for each colour c x_{ic} + x{jc} + s_{ec} = 1
     * 
     * x_{ic}, s_{ec} >= 0
     * 
     * So for n vertices, m edges and c colours there are:
     * n * c + m * c variables
     *     n + m * c constraints
     */

    int maxc = getMaxColour(g);
    int vars = g.getVertices() * maxc;
    
    int additionalVars = g.getEdges().size() * maxc;
    int totalVars = vars + additionalVars;
    int constraintsN = g.getVertices() + g.getEdges().size() * maxc;

    Eigen::MatrixXi constraints(constraintsN, totalVars + 1);
    constraints.setConstant(0);
    
    //for each vertice i sum{c <= maxc}x_{ic} = 1
    for (int i = 0; i < g.getVertices(); ++i)
    {
        for (int j = 0; j < maxc; ++j)
            constraints(additionalVars + i, additionalVars + j * g.getVertices() + i) = 1;
        
        constraints(additionalVars + i, totalVars) = 1;
    }

    // for each edge e between i and j for each colour c x_{ic} + x{jc} + s_{ec} = 1
    int row = 0;
    for (const Graph::Edge e : g.getEdges())
    {
        for (int j = 0; j < maxc; ++j, ++row)
        {
            constraints(row, additionalVars + e.first - 1 + j * g.getVertices()) = 1;
            constraints(row, additionalVars + e.second - 1 + j * g.getVertices()) = 1;

            constraints(row, row) = 1;

            constraints(row, totalVars) = 1;
        }
    }

    return constraints;
}

std::vector<int> LinearProgrammingSolver::getColouring(const Graph& g, Eigen::VectorXi& solution)
{
    std::vector<int> colouring(g.getVertices());

    int maxc = solution.size() / g.getVertices();

    for (int i = 0; i < maxc; ++i)
    {
        for (int j = 0; j < g.getVertices(); ++j)
        {
            if (solution(i * g.getVertices() + j))
                colouring[j] = i + 1;
        }
    }

    return colouring;
}

Eigen::VectorXi LinearProgrammingSolver::solveSimplex(Eigen::VectorXi& target, Eigen::MatrixXi& constraints)
{
    Eigen::MatrixXf constr = constraints.cast<float>();

    auto t = getCanonicalWithFeasibleSolution(target, constr);
    Eigen::MatrixXf tableau = std::move(t.first);
    std::vector<int> basis = std::move(t.second);

    solveSimplex(tableau, basis);

    while (!isResultIntegral(tableau, basis))
    {
        for (int i = 1; i < basis.size(); ++i)
        {
            float r = tableau(i, tableau.cols() - 1);

            if (fabsf(roundf(r) - r) > EPSILON)
            {
				std::cout << "UnIntegr. " << r;
                Eigen::MatrixXf newConstraints = Eigen::MatrixXf::Zero(constr.rows() + 1, constr.cols() + 1);
                newConstraints.block(0, 1, constr.rows(), constr.cols()) << constr;
                
                for(int j = 0; j < constr.cols(); ++j)
                {
					float decimal = 0.f;
					if (fabsf(roundf(tableau(i, j + 1)) - tableau(i, j + 1)) > EPSILON)
						decimal = tableau(i, j + 1) - floorf(tableau(i, j + 1));
					std::cout << "(" << decimal << ")";
					newConstraints(constr.rows(), j + 1) = decimal;
                }

                newConstraints(constr.rows(), 0) = -1;

				std::cout << std::endl << newConstraints.row(constr.rows()) << std::endl;

                constr = newConstraints;

                break;
            }
        }

        t = getCanonicalWithFeasibleSolution(target, constr);
        tableau = std::move(t.first);
        basis = std::move(t.second);

        solveSimplex(tableau, basis);
    }

    return getResult(tableau, basis, target.size());
}

void LinearProgrammingSolver::solveSimplex(Eigen::MatrixXf& tableau, std::vector<int>& basis)
{
	std::cout << "SIMPLEX\n";
    int maxc;
    do
    {
        maxc = -1;

        for (int i = 1; i < tableau.cols() - 1; ++i)
        {
            if (tableau(0, i) <= 0)
                continue;
            if (tableau.col(i).tail(tableau.rows() - 1).maxCoeff() <= 0)
                continue;

            if (maxc == -1 || tableau(0, i) > tableau(0, maxc))
                maxc = i;
        }

        if (maxc != -1)
        {
            int bestr = -1;
            float bestrv = 0;

            for (int i = 1; i < tableau.rows(); ++i)
            {
                if (tableau(i, maxc) < EPSILON)
                    continue;
                
                float v = tableau(i, tableau.cols() - 1) / tableau(i, maxc);
                if (bestr == -1 || bestrv > v)
                {
                    bestr = i;
                    bestrv = v;
                }
            }

			std::cout << "Selected: " << maxc << "( " << tableau(0, maxc) << " ) " << bestr << "( " << bestrv << " ) " << std::endl;

            basis[bestr] = maxc;
            pivot(tableau, bestr, maxc);
        }

		std::cout << "Result " << tableau(0, tableau.cols() - 1) << std::endl;
    } while (maxc != -1);

	std::cout << std::endl;
}

std::pair<Eigen::MatrixXf, std::vector<int>> LinearProgrammingSolver::getCanonicalWithFeasibleSolution(Eigen::VectorXi& target, Eigen::MatrixXf& constraints)
{
    std::vector<int> basis(constraints.rows() + 1, -1);
    basis[0] = 0;

    for (int i = 0; i < constraints.cols(); ++i)
    {
        int id;
        if(constraints.col(i).maxCoeff(&id) == 1 && constraints.col(i).sum() == 1)
            basis[id + 1] = i + 1;
    }

    int requiredArtificialVars = std::count(basis.begin(), basis.end(), -1);
    
    Eigen::MatrixXf tableau = Eigen::MatrixXf::Zero(constraints.rows() + 1, constraints.cols() + requiredArtificialVars + 1);

    tableau.block(1, requiredArtificialVars + 1, constraints.rows(), constraints.cols()) << constraints;

    tableau.row(0).tail(constraints.cols()) << constraints.colwise().sum();
    tableau(0, 0) = 1;

    for (int i = 1, artificial = 0; i < basis.size(); ++i)
    {
        if (basis[i] == -1)
        {
            artificial += 1;
            basis[i] = artificial;
            tableau(i, artificial) = 1;
        }
        else
        {
            basis[i] += requiredArtificialVars;
            tableau.row(0) -= tableau.row(i);
        }
    }

	std::cout << "Pre Canonical Check: " << tableau.col(tableau.cols() - 1).transpose() << std::endl;

    // finding first feasible solution
    // using artificial variables
    solveSimplex(tableau, basis);
	std::cout << "Canonical Check 1: " << tableau.col(tableau.cols() - 1).transpose() << std::endl;

    // for all artificial vars still in basis, swap them with real variables
    for (int i = 1; i < basis.size(); ++i)
    {
        if (basis[i] < requiredArtificialVars + 1)
        {
            for (int j = requiredArtificialVars + 1; j < tableau.cols(); ++j)
            {
                if (tableau(i, j) != 0)
                {
                    pivot(tableau, i, j);
                    basis[i] = j;
                    break;
                }
            }
        }
    }

    // drop artificial variables
    Eigen::MatrixXf t = Eigen::MatrixXf::Zero(constraints.rows() + 1, constraints.cols() + 1);
    t(0, 0) = 1;
    t.block(1, 1, constraints.rows(), constraints.cols()) 
        << tableau.block(1, 1 + requiredArtificialVars, constraints.rows(), constraints.cols());
        
    t.row(0).tail(target.size() + 1) << -target.transpose().cast<float>(), 0;
    
    for (int i = 1; i < basis.size(); ++i)
    {
        basis[i] -= requiredArtificialVars;

        if (t(0, basis[i]) != 0)
        t.row(0) -= t.row(i) * t(0, basis[i]);
    }
	std::cout << "Canonical Check 2: " << tableau.col(tableau.cols() - 1).transpose() << std::endl;
    return {t, basis};
}

void LinearProgrammingSolver::pivot(Eigen::MatrixXf& tableau, int row, int col)
{
	tableau.row(row) /= tableau(row, col);
	for (int i = 0; i < tableau.rows(); ++i)
	{
		if (i != row)
		{
			tableau.row(i) -= tableau.row(row) * tableau(i, col);
		}
	}
}

bool LinearProgrammingSolver::isResultIntegral(Eigen::MatrixXf& tableau, std::vector<int>& basis)
{
    for (int i = 1; i < basis.size(); ++i)
    {
        float r = tableau(i, tableau.cols() - 1);

        if (fabsf(roundf(r) - r) > EPSILON)
            return false;
    }

    return true;
}

Eigen::VectorXi LinearProgrammingSolver::getResult(Eigen::MatrixXf& tableau, std::vector<int>& basis, int size)
{
    Eigen::VectorXi result = Eigen::VectorXi::Zero(size);

    int varsOffset = tableau.cols() - size - 1;

    for (int i = 0; i < basis.size(); ++i)
        if (basis[i] >= varsOffset)
            result[basis[i] - varsOffset] = (int)roundf(tableau(i, tableau.cols() - 1));

    return result;
}
