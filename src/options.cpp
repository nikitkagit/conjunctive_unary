#include "options.hpp"

#include <ostream>
#include <string>

bool Options::parse(int argc, char **argv, std::ostream &error)
{
    for (int i = 1; i < argc; ++i)
    {
        const std::string arg = argv[i];

        if (arg == "-f")
        {
            if (i + 1 >= argc)
            {
                error << "Missing value for -f\n";
                return false;
            }
            m_input_file = argv[++i];
        }
        else if (arg == "-o")
        {
            if (i + 1 >= argc)
            {
                error << "Missing value for -o\n";
                return false;
            }
            m_output_file = argv[++i];
        }
        else if (arg == "-n")
        {
            if (i + 1 >= argc)
            {
                error << "Missing value for -n\n";
                return false;
            }
            if (!parse_iterations(argv[++i], error))
            {
                return false;
            }
        }
        else if (arg == "-h" || arg == "--help")
        {
            m_help_requested = true;
            return true;
        }
        else
        {
            error << "Unknown argument: " << arg << "\n";
            return false;
        }
    }
    return true;
}

void Options::print_usage(const char *program, std::ostream &out) const
{
    out << "Usage: " << program
        << " [-f <input_file>] [-o <output_file>] [-n <iterations>]\n"
        << "  -f <input_file>   Path to the input file "
           "(default: " << DEFAULT_INPUT_FILE << ")\n"
        << "  -o <output_file>  Path to the output file "
           "(default: " << DEFAULT_OUTPUT_FILE << ")\n"
        << "  -n <iterations>   Number of iterations "
           "(default: " << DEFAULT_ITERATIONS << ")\n"
        << "  -h, --help        Show this help message\n";
}

const std::string &Options::input_file() const
{
    return m_input_file;
}

const std::string &Options::output_file() const
{
    return m_output_file;
}

unsigned long long Options::iterations() const
{
    return m_iterations;
}

bool Options::help_requested() const
{
    return m_help_requested;
}

bool Options::parse_iterations(const std::string &value, std::ostream &error)
{
    std::size_t consumed = 0;
    try
    {
        m_iterations = std::stoull(value, &consumed);
    }
    catch (const std::exception &)
    {
        error << "Invalid value for -n: " << value << "\n";
        return false;
    }
    if (consumed != value.size())
    {
        error << "Invalid value for -n: " << value << "\n";
        return false;
    }
    return true;
}