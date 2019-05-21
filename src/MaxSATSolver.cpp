#include "MaxSATSolver.h"

#include <map>

using namespace SumColouring;

const int MaxSATSolver::WEIGHT_INFINITY = 99999999;

std::vector<int> MaxSATSolver::findColouring(const Graph& g)
{
    std::tuple<std::vector<Clause>, std::vector<SpecialClause>, std::vector<int>, int> maxSATProblem = transformToMaxSAT(g);
    std::vector<bool> solution = solveMaxSAT(std::get<0>(maxSATProblem), std::get<1>(maxSATProblem), std::get<2>(maxSATProblem), std::get<3>(maxSATProblem));

    return getColouring(solution, g);
}

std::tuple<std::vector<MaxSATSolver::Clause>, std::vector<MaxSATSolver::SpecialClause>, std::vector<int>, int> MaxSATSolver::transformToMaxSAT(const Graph& g)
{
    int maxColours = getMaxColour(g);
    int varsNumber = g.getVertices() * maxColours;

    std::vector<Clause> clauses;
    std::vector<SpecialClause> eq1Clauses;

    for (int i = 0; i < g.getVertices(); ++i)
    {
        SpecialClause has1Colour;
        for (int j = 0; j < maxColours; ++j)
            has1Colour.insert(i * maxColours + j + 1);
        
        eq1Clauses.push_back(std::move(has1Colour));
    }

    for (const Graph::Edge& e: g.getEdges())
        for (int i = 0; i < maxColours; ++i)
            clauses.push_back({(const int)clauses.size(),
                {-((e.first - 1) * maxColours + i + 1), -((e.second - 1) * maxColours + i + 1)}}); //adjacent vertices mustn't have same colours
    
    //all previous clauses are hard
    std::vector<int> weights(clauses.size(), WEIGHT_INFINITY);

    for (int i = 0; i < g.getVertices(); ++i)
    {
        for (int j = 0; j < maxColours; ++j)
        {
            // the higher colour assigned the higher the priority to satisfy clause where this colour is NOT assigned
            clauses.push_back({(const int)clauses.size(), {-(i * maxColours + j + 1)}});
            weights.push_back(j + 1);
        }
    }

    return std::make_tuple(std::move(clauses), std::move(eq1Clauses), std::move(weights), varsNumber);
}

std::vector<int> MaxSATSolver::getColouring(const std::vector<bool>& vars, const Graph& g)
{
    std::vector<int> colouring(g.getVertices(), 0);

    int coloursPerVertex = vars.size() / g.getVertices();

    for (int i = 0; i < g.getVertices(); ++i)
        for (int j = 0; j < coloursPerVertex; ++j)
            if (vars[i * coloursPerVertex + j])
                colouring[i] = j + 1;

    return colouring;
}

std::pair<std::vector<int>, std::vector<bool>> MaxSATSolver::solveSAT(std::vector<Clause>& clauses, std::vector<SpecialClause>& eq1Clauses, int vars)
{
    std::vector<int> solvedVars;
    std::vector<Clause> clausesCopy(clauses);
    std::vector<SpecialClause> eq1ClausesCopy(eq1Clauses);
    std::vector<SpecialClause> emptyEq0Clauses;
    std::pair<std::set<int>, bool> satResult = solveSAT(clausesCopy, emptyEq0Clauses, eq1ClausesCopy, solvedVars, vars);
    if (satResult.second)
    {
        std::vector<bool> solution(vars);

        for (int var : solvedVars)
        {
            if (var > 0)
                solution[var - 1] = true;
            else
                solution[-var - 1] = false;
        }

        return {{}, solution};
    }
    else
    {
        std::vector<int> core;
        for (Clause c : clauses)
            if (satResult.first.find(c.first) == satResult.first.end())
                core.push_back(c.first);
        return {core, {}};
    }
}

std::pair<std::set<int>, bool> MaxSATSolver::solveSAT(std::vector<Clause>& clauses, std::vector<SpecialClause>& eq0Clauses,
            std::vector<SpecialClause>& eq1Clauses, std::vector<int>& solvedVars, int vars)
{
    if (clauses.empty() && eq0Clauses.empty() && eq1Clauses.empty())
        return {{}, true};
    
    for (const SpecialClause& clause : eq1Clauses)
    {
        if (clause.empty())
        {
            std::set<int> notInCore;
            for (const Clause& c : clauses)
                notInCore.insert(c.first);

            return {notInCore, false};
        }
    }

    for (const Clause& clause : clauses)
    {
        if (clause.second.size() == 0)    
        {
            std::set<int> notInCore;
            for (const Clause& c : clauses)
                if (c.first != clause.first)
                    notInCore.insert(c.first);

            return {notInCore, false};

        }
    }

    //if there is a =0 clause, all variables in it must be equal to 0

    if (!eq0Clauses.empty())
    {
        solvedVars.push_back(-*eq0Clauses[0].begin());
        solveVar(clauses, eq0Clauses, eq1Clauses, *eq0Clauses[0].begin(), false);
        return solveSAT(clauses, eq0Clauses, eq1Clauses, solvedVars, vars);
    }

    //if there is a clause with only one literal, there is only one way to satisfy that clause
   
    for (const Clause& clause : clauses)
    {
        if (clause.second.size() == 1)
        {
            solvedVars.push_back(clause.second[0]);

            if (clause.second[0] > 0)
                solveVar(clauses, eq0Clauses, eq1Clauses, clause.second[0], true);
            else
                solveVar(clauses, eq0Clauses, eq1Clauses, -clause.second[0], false);

            return solveSAT(clauses, eq0Clauses, eq1Clauses, solvedVars, vars);
        }
    }
     
    for (const SpecialClause& clause : eq1Clauses)
    {
        if (clause.size() == 1)
        {
            solvedVars.push_back(*clause.begin());
            solveVar(clauses, eq0Clauses, eq1Clauses, *clause.begin(), true);
            return solveSAT(clauses, eq0Clauses, eq1Clauses, solvedVars, vars);
        }
    }

    // check if there is a variable that occurs only as negated or only as not negated
    // else branch using one that occurs most often (a heuristic choice)
    std::vector<std::pair<int, int>> occurences(vars);
    for (const SpecialClause& clause : eq1Clauses)
    {
        for (int v : clause)
        { 
            //variables in eq1 are actually both negated and not
            ++occurences[v - 1].first;
            occurences[v - 1].second += clause.size() - 1;
        }
    }

    for (const Clause& clause : clauses)
    {
        for (int v : clause.second)
        {
            int var = v > 0 ? v : -v;

            if (v > 0)
                ++occurences[var - 1].first;
            else
                ++occurences[var - 1].second;
        }
    }

    int maxOcc = 0;
    int var = -1;
	int i = 0;
    for (const std::pair<int, int>& occ : occurences)
    {
		++i;
		if (occ.first + occ.second == 0)
			continue;

        if (occ.first == 0)
        {
            // only negative occurences - by setting value of var to false we satisfy all clauses that contain it
            solveVar(clauses, eq0Clauses, eq1Clauses, occ.first, false);
            solvedVars.push_back(-occ.first);
            return solveSAT(clauses, eq0Clauses, eq1Clauses, solvedVars, vars);
        }
        if (occ.second == 0)
        {
            // only positive occurences - by setting value of var to true we satisfy all clauses that contain it
            solveVar(clauses, eq0Clauses, eq1Clauses, occ.first, true);
            solvedVars.push_back(occ.first);
            return solveSAT(clauses, eq0Clauses, eq1Clauses, solvedVars, vars);
        }

        if (occ.first + occ.second > maxOcc)
        {
            maxOcc = occ.first + occ.second;
            var = i;
        }
    }

    std::vector<Clause> clausesCopy(clauses);
    std::vector<SpecialClause> eq0ClausesCopy(eq0Clauses);
    std::vector<SpecialClause> eq1ClausesCopy(eq1Clauses);

    int fallbackSolvedVars = solvedVars.size();
    solvedVars.push_back(-var);

    solveVar(clausesCopy, eq0ClausesCopy, eq1ClausesCopy, var, false);
    if (solveSAT(clausesCopy, eq0ClausesCopy, eq1ClausesCopy, solvedVars, vars).second)
        return {{}, true};

    solvedVars.resize(fallbackSolvedVars);
    solvedVars.push_back(var);
    solveVar(clauses, eq0Clauses, eq1Clauses, var, true);
    return solveSAT(clauses, eq0Clauses, eq1Clauses, solvedVars, vars);
}

void MaxSATSolver::solveVar(std::vector<Clause>& clauses, std::vector<SpecialClause>& eq0Clauses,
            std::vector<SpecialClause>& eq1Clauses, int var, bool value)
{
	for (int i = eq0Clauses.size() - 1; i >= 0; --i)
	{
		SpecialClause& clause = eq0Clauses[i];
		clause.erase(var);
		if (clause.size() == 0)
		{
			if (i != eq0Clauses.size() - 1)
				clause = std::move(eq0Clauses[eq0Clauses.size() - 1]);
			eq0Clauses.pop_back();
		}
	}

	for (int i = eq1Clauses.size() - 1; i >= 0; --i)
	{
		SpecialClause& clause = eq1Clauses[i];
		bool removed = clause.erase(var) > 0;
		if (removed && value)
		{
			if (clause.size() > 0)
				eq0Clauses.push_back(std::move(eq1Clauses[i]));
			if (i != eq1Clauses.size() - 1)
				clause = std::move(eq1Clauses[eq1Clauses.size() - 1]);
			eq1Clauses.pop_back();
		}
	}

	for (int i = clauses.size() - 1; i >= 0; --i)
	{
		Clause& clause = clauses[i];
		bool remove = false;
		for (int j = clause.second.size() - 1; j >= 0; --j)
		{
			if (value && clause.second[j] == var || !value && clause.second[j] == -var)
			{
				remove = true; //clause is now always true;
				break;
			}
			else if (clause.second[j] == var || clause.second[j] == -var)
			{
				if (j != clause.second.size() - 1)
					clause.second[j] = clause.second[clause.second.size() - 1];
				clause.second.pop_back();
				break;
			}
		}
		if (remove)
		{
			if (i != clauses.size() - 1)
				clause = std::move(clauses[clauses.size() - 1]);
			clauses.pop_back();
		}
	}
}

std::vector<bool> MaxSATSolver::solveMaxSAT(std::vector<Clause>& clauses, std::vector<SpecialClause>& eq1Clauses, std::vector<int>& weights, int vars)
{
    int additionalVars = 0;

    while (true)
    {
        auto satResult = solveSAT(clauses, eq1Clauses, vars + additionalVars);
        if (satResult.first.size() == 0)
        {
            //clauses are satisfiable
            //remove additionalVars from result
			satResult.second.resize(vars);
            return satResult.second;
        }

        int wmin = WEIGHT_INFINITY;
        for (int unsatisfiableClause : satResult.first)
            if (weights[unsatisfiableClause] < wmin)
                wmin = weights[unsatisfiableClause];

        SpecialClause newEq1Clause;
        for (int unsatisfiableClause : satResult.first)
        {
            if (weights[unsatisfiableClause] != WEIGHT_INFINITY) // is soft clause
            {
                if (weights[unsatisfiableClause] > wmin)
                {
                    clauses.push_back({clauses.size(), clauses[unsatisfiableClause].second});
                    weights.push_back(weights[unsatisfiableClause] - wmin);
                }

                clauses[unsatisfiableClause].second.push_back(vars + additionalVars + 1);
                weights[unsatisfiableClause] = wmin;
                newEq1Clause.insert(vars + additionalVars + 1);
                ++additionalVars;
            }
        }

        eq1Clauses.push_back(newEq1Clause);
    }
}
