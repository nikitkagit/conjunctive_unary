#ifndef SOLVER_APP_HPP
#define SOLVER_APP_HPP

#include <iosfwd>

class EquationCalculator;
class EquationParser;

/**
 * @brief Encapsulates the top-level application logic.
 *
 * Parses command-line options, reads the input file, builds the equation
 * system via EquationParser, iterates EquationCalculator and prints the result to
 * the configured output file and to standard output.
 */
class SolverApp
{
public:
    /**
     * @brief Constructs the application.
     * @param out Stream for normal output (e.g. std::cout).
     * @param err Stream for error messages (e.g. std::cerr).
     */
    SolverApp(std::ostream &out, std::ostream &err);

    /**
     * @brief Runs the application.
     * @param argc Argument count from main().
     * @param argv Argument vector from main().
     * @return Process exit code: 0 on success, non-zero on failure.
     */
    int run(int argc, char **argv);

private:
    bool load_equations(const std::string &path,
                        EquationParser &parser,
                        EquationCalculator &calculator);

    std::ostream &m_out;
    std::ostream &m_err;
};

#endif // SOLVER_APP_HPP