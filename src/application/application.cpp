#include "application/application.hpp"

#include "application/options.hpp"
#include "application/result_printer.hpp"

#include "equation/equation_calculator.hpp"
#include "equation/parser.hpp"

#include <fstream>
#include <string>

Application::Application(std::ostream &out, std::ostream &err)
    : m_out(out)
    , m_err(err)
{
}

int Application::run(int argc, char **argv)
{
    Options options;
    if (!options.parse(argc, argv, m_err))
    {
        options.print_usage(argv[0], m_err);
        return 1;
    }

    if (options.help_requested())
    {
        options.print_usage(argv[0], m_out);
        return 0;
    }

    EquationCalculator calculator;
    Parser parser(calculator);

    if (!load_equations(options.input_file(), parser, calculator))
    {
        return 1;
    }

    for (unsigned long long i = 0; i < options.iterations(); ++i)
    {
        calculator.iterate();
    }

    const ResultPrinter printer(parser.variables());
    if (!printer.write_to_file(options.output_file()))
    {
        m_err << "Failed to write output file: " << options.output_file()
              << "\n";
        return 1;
    }
    printer.write(m_out);

    return 0;
}

bool Application::load_equations(const std::string &path,
                                 Parser &parser,
                                 EquationCalculator & /*calculator*/)
{
    std::ifstream input(path);
    if (!input)
    {
        m_err << "Failed to open input file: " << path << "\n";
        return false;
    }

    std::string line;
    std::size_t line_number = 0;
    while (std::getline(input, line))
    {
        ++line_number;

        const std::size_t first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos)
        {
            continue;
        }

        try
        {
            parser.parse_equation(line);
        }
        catch (const std::exception &e)
        {
            m_err << "Parse error on line " << line_number << ": "
                  << e.what() << "\n";
            return false;
        }
    }
    return true;
}