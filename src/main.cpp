#include "io.h"
#include "Solver.h"

#include <iostream>
#include <exception>

int main(int argc, char** argv)
{
    if (argc != 2)
        return -1;
    std::unique_ptr<SumColouring::Solver> solver = SumColouring::Solver::create(argv[1]);

    if (solver == nullptr)
        return -2;
    
	try
	{
		SumColouring::Graph graph = SumColouring::readGraph(std::cin);
		std::vector<int> colouring = solver->findColouring(graph);

		SumColouring::writeColouring(std::cout, colouring);
		return 0;
	}
	catch (std::exception& exc)
	{
		std::cout << exc.what();
		return -3;
	}
}