#include <iostream>

#include "CU/solver_app/solver_app.hpp"

int main(int argc, char **argv)
{
    SolverApp app(std::cout, std::cerr);
    return app.run(argc, argv);
}