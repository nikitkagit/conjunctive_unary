#include <iostream>

#include "CU/application.hpp"

int main(int argc, char **argv)
{
    Application app(std::cout, std::cerr);
    return app.run(argc, argv);
}