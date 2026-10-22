#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "equation/equation_calculator.hpp"
#include "equation/equation_system_printer.hpp"
#include "equation/parser.hpp"

namespace
{
std::string render(Parser &parser, EquationCalculator &calculator)
{
    EquationSystemPrinter printer(parser.variables(), calculator);
    return printer.format();
}
} // namespace

int main()
{
    // 1. Empty system prints nothing.
    {
        EquationCalculator calc;
        Parser parser(calc);
        assert(render(parser, calc).empty());
    }

    // 2. Constant expression at top level: no parentheses.
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = {1, 2, 3}");
        assert(render(parser, calc) == "A = {1, 2, 3}\n");
    }

    // 3. Empty constant set.
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = {}");
        assert(render(parser, calc) == "A = {}\n");
    }

    // 4. Union at top level: no parentheses.
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = {1} | {2}");
        assert(render(parser, calc) == "A = {1} | {2}\n");
    }

    // 5. Intersection at top level: no parentheses.
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = {1} & {2}");
        assert(render(parser, calc) == "A = {1} & {2}\n");
    }

    // 6. Sum at top level: no parentheses.
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = {1} + {2}");
        assert(render(parser, calc) == "A = {1} + {2}\n");
    }

    // 7. Sum of intersection: no parentheses, since & binds tighter.
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = {1} + {2} & {3}");
        assert(render(parser, calc) == "A = {1} + {2} & {3}\n");
    }

    // 8. Intersection of sum: sum is wrapped in parentheses (lower
    //    precedence than &).
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = {1} & ({2} + {3})");
        assert(render(parser, calc) == "A = {1} & ({2} + {3})\n");
    }

    // 9. Union of sum: no parentheses, since + binds tighter than |.
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = {1} | {2} + {3}");
        assert(render(parser, calc) == "A = {1} | {2} + {3}\n");
    }

    // 10. Sum of union: union needs parentheses.
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = {1} + ({2} | {3})");
        assert(render(parser, calc) == "A = {1} + ({2} | {3})\n");
    }

    // 11. Intersection of union: union needs parentheses.
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = {1} & ({2} | {3})");
        assert(render(parser, calc) == "A = {1} & ({2} | {3})\n");
    }

    // 12. Motivating example: outer parentheses around the whole RHS are
    //     not emitted, and the inner parentheses around (B & C) are also
    //     dropped, because & binds tighter than |.
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = {1, 2, 3} | (B & C)");
        assert(render(parser, calc) == "A = {1, 2, 3} | B & C\n");
    }

    // 13. Redundant parentheses in the source are dropped.
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = ({1} & {2})");
        assert(render(parser, calc) == "A = {1} & {2}\n");
    }

    // 14. Two unions under intersection keep both pairs of parentheses.
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = ({1} | {2}) & ({3} | {4})");
        assert(render(parser, calc) ==
               "A = ({1} | {2}) & ({3} | {4})\n");
    }

    // 15. Variable references are rendered by name.
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = {1}");
        parser.parse_equation("B = A");
        parser.parse_equation("C = A | B");
        assert(render(parser, calc) ==
               "A = {1}\n"
               "B = A\n"
               "C = A | B\n");
    }

    // 16. Multiple equations are printed in insertion order.
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = {1}");
        parser.parse_equation("B = A | {2}");
        parser.parse_equation("C = B & {3}");
        assert(render(parser, calc) ==
               "A = {1}\n"
               "B = A | {2}\n"
               "C = B & {3}\n");
    }

    // 17. Self-reference.
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = A | {1}");
        assert(render(parser, calc) == "A = A | {1}\n");
    }

    // 18. Alternating precedence in a deeper tree:
    //     ({1} | {2}) + ({3} & ({4} | {5}))
    //     - union under sum needs parens,
    //     - intersection under sum does not,
    //     - union under intersection needs parens.
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = ({1} | {2}) + ({3} & ({4} | {5}))");
        assert(render(parser, calc) ==
               "A = ({1} | {2}) + {3} & ({4} | {5})\n");
    }

    // 19. Round-trip: the output is valid input and re-parses to the same
    //     text.
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation(
            "A = {1, 2, 3} | (B & C)");
        parser.parse_equation(
            "B = (A + {1}) & {2, 3, 4, 5}");
        const std::string first = render(parser, calc);

        EquationCalculator calc2;
        Parser parser2(calc2);
        std::istringstream lines(first);
        std::string line;
        while (std::getline(lines, line))
        {
            if (!line.empty())
            {
                parser2.parse_equation(line);
            }
        }
        assert(render(parser2, calc2) == first);
    }

    // 20. write_to_file writes the same content as format().
    {
        const char *path = "test_esp_output.txt";
        std::remove(path);

        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = {1, 2}");
        parser.parse_equation("B = A | {3}");

        EquationSystemPrinter printer(parser.variables(), calc);
        assert(printer.write_to_file(path));

        std::ifstream in(path);
        std::ostringstream ss;
        ss << in.rdbuf();
        const std::string content = ss.str();
        std::remove(path);

        assert(content ==
               "A = {1, 2}\n"
               "B = A | {3}\n");
    }

    // 21. write_to_file to an invalid path returns false.
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = {1}");

        EquationSystemPrinter printer(parser.variables(), calc);
        assert(!printer.write_to_file("no_such_dir_xyz/out.txt"));
    }

    // 22. write() emits the same text as format().
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = {1} | {2}");
        parser.parse_equation("B = {3} & {4}");

        EquationSystemPrinter printer(parser.variables(), calc);
        std::ostringstream out;
        printer.write(out);

        assert(out.str() == printer.format());
        assert(out.str() ==
               "A = {1} | {2}\n"
               "B = {3} & {4}\n");
    }

    std::cout << "All EquationSystemPrinter tests passed\n";
    return 0;
}