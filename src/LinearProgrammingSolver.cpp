#include "LinearProgrammingSolver.h"

#include <exception>

using namespace SumColouring;

// a helper method for finding greatest common divisor
int gcd(int a, int b)
{
	if (a == 0)
		return b;
	if (b == 0)
		return a;
	if (a < 0)
		a = -a;
	if (b < 0)
		b = -b;

	int c = a % b;
	while (c != 0)
	{
		a = b;
		b = c;
		c = a % b;
	}
	return b;
}

// using https://people.orie.cornell.edu/dpw/orie6300/Lectures/lec12.pdf
// as reference for finding initial feasible solution
// http://www.ou.edu/class/che-design/che5480-11/Gomory%20Cuts-The%20How%20and%20the%20Why.pdf
// as reference for finding integer solution (Gomory Cuts)
// and http://fourier.eng.hmc.edu/e176/lectures/NM/node32.html
// as reference for Simplex algorithm

std::vector<int> LinearProgrammingSolver::findColouring(const Graph& g)
{
    Eigen::VectorXi target = getTarget(g);
    Eigen::MatrixXi constraints = getConstraints(g);
    Eigen::VectorXi solution = solveSimplex(target, constraints);
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
	auto t = getCanonicalWithFeasibleSolution(target, constraints);
    Eigen::MatrixXi tableau = std::move(t.first);
    std::vector<int> basis = std::move(t.second);

    solveSimplex(tableau, basis);

    while (!isResultIntegral(tableau, basis))
    {
        for (int i = 1; i < basis.size(); ++i)
        {
            int r = tableau(i, tableau.cols() - 1);
			int divisor = tableau(i, basis[i]);

            if (r % divisor != 0)
            {
                Eigen::MatrixXi newConstraints = Eigen::MatrixXi::Zero(constraints.rows() + 1, constraints.cols() + 1);
                newConstraints.block(0, 1, constraints.rows(), constraints.cols()) << constraints;
                
                for(int j = 0; j < constraints.cols(); ++j)
                {
					int mod = tableau(i, j + 1) % divisor;
					if (mod < 0)
						mod += divisor;
					
					newConstraints(constraints.rows(), j + 1) = mod;
                }

                newConstraints(constraints.rows(), 0) = -divisor;

				constraints = newConstraints;

                break;
            }
        }

        t = getCanonicalWithFeasibleSolution(target, constraints);
        tableau = std::move(t.first);
        basis = std::move(t.second);

        solveSimplex(tableau, basis);
    }

    return getResult(tableau, basis, target.size());
}

void LinearProgrammingSolver::solveSimplex(Eigen::MatrixXi& tableau, std::vector<int>& basis)
{
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
                if (tableau(i, maxc) <= 0)
                    continue;
                
                float v = ((float)tableau(i, tableau.cols() - 1)) / tableau(i, maxc);
                if (bestr == -1 || bestrv > v)
                {
                    bestr = i;
                    bestrv = v;
                }
            }

			pivot(tableau, basis, bestr, maxc);
        }
    } while (maxc != -1);
}

std::pair<Eigen::MatrixXi, std::vector<int>> LinearProgrammingSolver::getCanonicalWithFeasibleSolution(Eigen::VectorXi& target, Eigen::MatrixXi& constraints)
{
    std::vector<int> basis(constraints.rows() + 1, -1);
    basis[0] = 0;

    for (int i = 0; i < constraints.cols(); ++i)
    {
        int id;
        if(constraints.col(i).minCoeff() >= 0 && constraints.col(i).maxCoeff(&id) == constraints.col(i).sum() && constraints(id, i) > 0)
            basis[id + 1] = i + 1;
    }

    int requiredArtificialVars = std::count(basis.begin(), basis.end(), -1);
    
    Eigen::MatrixXi tableau = Eigen::MatrixXi::Zero(constraints.rows() + 1, constraints.cols() + requiredArtificialVars + 1);
	
	Eigen::MatrixXi tabl = Eigen::MatrixXi::Zero(constraints.rows() + 1, constraints.cols() + 1);
	tabl.block(1, 1, constraints.rows(), constraints.cols()) << constraints;
	tabl(0, 0) = 1;
	
    tableau.block(1, requiredArtificialVars + 1, constraints.rows(), constraints.cols()) << constraints;

	tableau(0, 0) = 1;
    tableau.row(0).tail(constraints.cols()) << constraints.colwise().sum();

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

    // finding first feasible solution
    // using artificial variables
    solveSimplex(tableau, basis);

	if (tableau(0, tableau.cols() - 1) != 0)
		throw std::range_error("Numeric error");

    // for all artificial vars still in basis, swap them with real variables
    for (int i = 1; i < basis.size(); ++i)
    {
        if (basis[i] < requiredArtificialVars + 1)
        {
            for (int j = requiredArtificialVars + 1; j < tableau.cols(); ++j)
            {
                if (tableau(i, j) != 0)
                {
                    pivot(tableau, basis, i, j);
                    break;
                }
            }
        }
    }

    // drop artificial variables
    Eigen::MatrixXi t = Eigen::MatrixXi::Zero(constraints.rows() + 1, constraints.cols() + 1);
    t(0, 0) = 1;
    t.block(1, 1, constraints.rows(), constraints.cols()) 
        << tableau.block(1, 1 + requiredArtificialVars, constraints.rows(), constraints.cols());
        
    t.row(0).tail(target.size() + 1) << -target.transpose(), 0;
    
    for (int i = 1; i < basis.size(); ++i)
    {
        basis[i] -= requiredArtificialVars;

		if (t(0, basis[i]) != 0)
		{
			int opGCD = gcd(t(i, basis[i]), t(0, basis[i]));
			t.row(0) = t.row(0) * t(i, basis[i]) / opGCD - t.row(i) * t(0, basis[i]) / opGCD;
		}
    }

    return {t, basis};
}

void LinearProgrammingSolver::pivot(Eigen::MatrixXi& tableau, std::vector<int>& basis, int row, int col)
{
	for (int i = 0; i < tableau.rows(); ++i)
	{
		if (i != row && tableau(i, col) != 0)
		{
			int colGCD = gcd(tableau(i, col), tableau(row, col));

			auto newRowValue = tableau.row(i) * (tableau(row, col) / colGCD) - tableau.row(row) * (tableau(i, col) / colGCD);

			if (newRowValue(basis[i]) < 0)
				tableau.row(i) *= -1;

			if (newRowValue(tableau.cols() - 1) < 0)
				throw std::range_error("Numeric error");

			tableau.row(i) = newRowValue;

			//reduce row by greatest common denominator to avoid exponentially increasing coefficients
			int rowGCD = tableau.row(i).redux(gcd);
			if (rowGCD > 0)
				tableau.row(i) /= rowGCD;
		}
	}

	if (tableau.col(tableau.cols() - 1).minCoeff() < 0)
		throw std::range_error("Numeric error");

	basis[row] = col;

	if (tableau.col(col).maxCoeff() != tableau(row, col) || tableau.col(col).sum() != tableau(row, col))
		throw std::range_error("Numeric error");
}

bool LinearProgrammingSolver::isResultIntegral(Eigen::MatrixXi& tableau, std::vector<int>& basis)
{
    for (int i = 1; i < basis.size(); ++i)
    {
		if (tableau(i, tableau.cols() - 1) % tableau(i, basis[i]) != 0)
            return false;
    }

    return true;
}

Eigen::VectorXi LinearProgrammingSolver::getResult(Eigen::MatrixXi& tableau, std::vector<int>& basis, int size)
{
    Eigen::VectorXi result = Eigen::VectorXi::Zero(size);

    int varsOffset = tableau.cols() - size - 1;

    for (int i = 0; i < basis.size(); ++i)
        if (basis[i] >= varsOffset)
            result[basis[i] - varsOffset] = tableau(i, tableau.cols() - 1) / tableau(i, basis[i]);

    return result;
}
