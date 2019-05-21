#pragma once

#include "Solver.h"
#include <set>

namespace SumColouring
{
    class MaxSATSolver : public Solver
    {
        const static int WEIGHT_INFINITY;
        typedef std::pair<int, std::vector<int>> Clause;
        typedef std::set<int> SpecialClause;
    public:
        virtual std::vector<int> findColouring(const Graph&);

    private:
        // Transforms sum colouring problem to MaxSAT problem
        // Variables are (for each colour) for every vertex
        // Returns vector of clauses, vector of =1 clauses, vector of weights and a number of variables
        std::tuple<std::vector<Clause>, std::vector<SpecialClause>, std::vector<int>, int> transformToMaxSAT(const Graph&);
        std::vector<int> getColouring(const std::vector<bool>&, const Graph&);

        // SAT problem is passed as a vector of clauses,
        // where each clause is a pair - id and a vector of literals - abs. number represents 
        // id of variable, sign represents if it is negated
        // returned value is a boolean vector.
        // if it is empty, first pair element contains unsatisfiable clauses (with eq1 clauses excluded!)
        // else second pair element reprents values of variables that satisfy all clauses
        std::pair<std::vector<int>, std::vector<bool>> solveSAT(std::vector<Clause>& clauses, std::vector<SpecialClause>& eq1Clauses, int vars);
        // first element in returned pair is a set of clauses ids that are not in the unsatisfiable core
        // eq0 clauses are norClauses, eq1 clauses are clauses where only one value may be true
        // and they are introduced because of performance reasons
        // eq0 and eq1 clauses are never mentioned as part of unsatisfiable core
        std::pair<std::set<int>, bool> solveSAT(
            std::vector<Clause>& clauses,
            std::vector<SpecialClause>& eq0Clauses,
            std::vector<SpecialClause>& eq1Clauses,
            std::vector<int>& solvedVars, int vars);
        void solveVar(
            std::vector<Clause>& clauses,
            std::vector<SpecialClause>& eq0Clauses, //assumption - if eq0 clauses are not empty, value MUST be false
            std::vector<SpecialClause>& eq1Clauses,
            int var, bool value);

        // assumption: clauses are numbered from 0
        // returns values of optimized variables
        std::vector<bool> solveMaxSAT(std::vector<Clause>& clauses, std::vector<SpecialClause>& eq1Clauses, std::vector<int>& weights, int vars);
    };
}