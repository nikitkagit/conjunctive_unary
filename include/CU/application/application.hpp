#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <iosfwd>

class EquationCalculator;
class Parser;

/**
 * @brief Encapsulates the top-level application logic.
 *
 * Parses command-line options, reads the input file, builds the equation
 * system via Parser, iterates EquationCalculator and prints the result to
 * the configured output file and to standard output.
 */
class Application
{
public:
    /**
     * @brief Constructs the application.
     * @param out Stream for normal output (e.g. std::cout).
     * @param err Stream for error messages (e.g. std::cerr).
     */
    Application(std::ostream &out, std::ostream &err);

    /**
     * @brief Runs the application.
     * @param argc Argument count from main().
     * @param argv Argument vector from main().
     * @return Process exit code: 0 on success, non-zero on failure.
     */
    int run(int argc, char **argv);

private:
    bool load_equations(const std::string &path,
                        Parser &parser,
                        EquationCalculator &calculator);

    std::ostream &m_out;
    std::ostream &m_err;
};

#endif // APPLICATION_HPP