#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <iosfwd>
#include <string>

/**
 * @brief Command-line options for the equation solver application.
 *
 * Parses arguments of the form
 * @code
 * app [-f <input_file>] [-o <output_file>] [-n <iterations>]
 * @endcode
 * Defaults are used for any option that is not supplied.
 */
class Options
{
public:
    /// Default input file name.
    static constexpr const char *DEFAULT_INPUT_FILE = "gcu_input.txt";
    /// Default output file name.
    static constexpr const char *DEFAULT_OUTPUT_FILE = "gcu_output.txt";
    /// Default number of iterations.
    static constexpr unsigned long long DEFAULT_ITERATIONS = 30;

    /**
     * @brief Parses the given argument list.
     * @param argc Argument count.
     * @param argv Argument vector.
     * @param error Output stream for error messages.
     * @return True on success (or if help was requested), false on error.
     *
     * If @c -h or @c --help is present, prints usage to @p error and
     * returns true with help_requested() set.
     */
    bool parse(int argc, char **argv, std::ostream &error);

    /**
     * @brief Prints usage information.
     * @param program Program name to include in the usage line.
     * @param out Output stream.
     */
    void print_usage(const char *program, std::ostream &out) const;

    /// @return Input file path.
    const std::string &input_file() const;
    /// @return Output file path.
    const std::string &output_file() const;
    /// @return Number of iterations.
    unsigned long long iterations() const;
    /// @return True if help was requested via -h/--help.
    bool help_requested() const;

private:
    bool parse_iterations(const std::string &value, std::ostream &error);

    std::string m_input_file = DEFAULT_INPUT_FILE;
    std::string m_output_file = DEFAULT_OUTPUT_FILE;
    unsigned long long m_iterations = DEFAULT_ITERATIONS;
    bool m_help_requested = false;
};

#endif // OPTIONS_HPP