#include "MaxSATSolver.h"
#include <iostream>
int main(int argc, char** argv)
{
    SumColouring::MaxSATSolver m;
    SumColouring::Graph g(6, {{1, 2}, {2, 3}, {1, 3}, {2,4}, {1, 6}, {3, 5}});
    std::vector<int> r = m.findColouring(g);

    std::cout << "RESULT:" << std::endl;
    for (int i = 0; i < r.size(); ++i)
        std::cout << r[i] << " ";
    
    std::cout << std::endl;
    
    return 0;
}