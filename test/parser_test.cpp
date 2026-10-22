#include <iostream>
#include <stdexcept>
#include <string>
#include <cassert>

#include "equation/equation.hpp"
#include "equation/equation_calculator.hpp"
#include "equation/parser.hpp"

#include "expression/variable_expression.hpp"

namespace
{
template <typename Fn>
bool throws_runtime_error(Fn &&fn)
{
    try
    {
        fn();
    }
    catch (const std::runtime_error &)
    {
        return true;
    }
    catch (...)
    {
        return false;
    }
    return false;
}
} // namespace

int main()
{
    // --- Basic assignment of a fixed set ---
    {
        EquationCalculator calc;
        Parser parser(calc);
        Equation *eq = parser.parse_equation("A = {1, 2, 3}");
        assert(eq != nullptr);
        eq->calculate();
        assert((eq->data() == data_t{1, 2, 3}));
    }

    // --- Empty fixed set ---
    {
        EquationCalculator calc;
        Parser parser(calc);
        Equation *eq = parser.parse_equation("A = {}");
        eq->calculate();
        assert(eq->data().empty());
    }

    // --- Union ---
    {
        EquationCalculator calc;
        Parser parser(calc);
        Equation *eq = parser.parse_equation("A = {1, 2} | {3, 4}");
        eq->calculate();
        assert((eq->data() == data_t{1, 2, 3, 4}));
    }

    // --- Intersection ---
    {
        EquationCalculator calc;
        Parser parser(calc);
        Equation *eq = parser.parse_equation("A = {1, 2, 3} & {2, 3, 4}");
        eq->calculate();
        assert((eq->data() == data_t{2, 3}));
    }

    // --- Pairwise sum ---
    {
        EquationCalculator calc;
        Parser parser(calc);
        Equation *eq = parser.parse_equation("A = {1, 2} + {3, 4}");
        eq->calculate();
        assert((eq->data() == data_t{4, 5, 6}));
    }

    // --- Precedence: & > + ---
    {
        // {1, 2} + ({3} & {3}) = {1, 2} + {3} = {4, 5}
        EquationCalculator calc;
        Parser parser(calc);
        Equation *eq = parser.parse_equation("A = {1, 2} + {3} & {3}");
        eq->calculate();
        assert((eq->data() == data_t{4, 5}));
    }

    // --- Precedence: + > | ---
    {
        // ({1} + {2}) | {3} = {3} | {3} = {3}
        EquationCalculator calc;
        Parser parser(calc);
        Equation *eq = parser.parse_equation("A = {1} + {2} | {3}");
        eq->calculate();
        assert((eq->data() == data_t{3}));
    }

    // --- Precedence: | lowest, & highest ---
    {
        // {1, 2} | ({3} & {3, 4}) = {1, 2} | {3} = {1, 2, 3}
        EquationCalculator calc;
        Parser parser(calc);
        Equation *eq = parser.parse_equation("A = {1, 2} | {3} & {3, 4}");
        eq->calculate();
        assert((eq->data() == data_t{1, 2, 3}));
    }

    // --- Parentheses override precedence ---
    {
        // ({1, 2} | {3}) & {3, 4} = {1, 2, 3} & {3, 4} = {3}
        EquationCalculator calc;
        Parser parser(calc);
        Equation *eq = parser.parse_equation("A = ({1, 2} | {3}) & {3, 4}");
        eq->calculate();
        assert((eq->data() == data_t{3}));
    }

    // --- Nested parentheses ---
    {
        EquationCalculator calc;
        Parser parser(calc);
        Equation *eq = parser.parse_equation("A = (({1} + {2}) & {3})");
        eq->calculate();
        assert((eq->data() == data_t{3}));
    }

    // --- Chained operators of the same kind ---
    {
        EquationCalculator calc;
        Parser parser(calc);
        Equation *eq = parser.parse_equation("A = {1} | {2} | {3}");
        eq->calculate();
        assert((eq->data() == data_t{1, 2, 3}));
    }
    {
        EquationCalculator calc;
        Parser parser(calc);
        Equation *eq = parser.parse_equation("A = {1, 2, 3} & {2, 3, 4} & {3, 4, 5}");
        eq->calculate();
        assert((eq->data() == data_t{3}));
    }
    {
        EquationCalculator calc;
        Parser parser(calc);
        Equation *eq = parser.parse_equation("A = {1} + {2} + {3}");
        eq->calculate();
        assert((eq->data() == data_t{6}));
    }

    // --- Equation is registered in the calculator ---
    {
        EquationCalculator calc;
        Parser parser(calc);
        Equation *eq = parser.parse_equation("A = {1}");
        assert(eq != nullptr);
        assert(calc.equations().size() == 1);
        assert(calc.equations().front().get() == eq);
    }

    // --- Unknown variable is created in dictionary ---
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = {1}");
        VariableExpression *a = parser.variables().find("A");
        assert(a != nullptr);
        assert(parser.variables().name_of(a) == "A");
    }

    // --- Multiple equations share the dictionary ---
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = {1, 2}");
        parser.parse_equation("B = {3}");
        VariableExpression *a = parser.variables().find("A");
        VariableExpression *b = parser.variables().find("B");
        assert(a != nullptr);
        assert(b != nullptr);
        assert(a != b);
        assert(calc.equations().size() == 2);
    }

    // --- Variable reference yields correct value after iteration ---
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = {1, 2}");
        Equation *eq = parser.parse_equation("B = A | {3}");
        calc.iterate();
        eq->calculate();
        assert((eq->data() == data_t{1, 2, 3}));
    }

    // --- Variable reference in intersection ---
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = {1, 2, 3}");
        Equation *eq = parser.parse_equation("B = A & {2, 3, 4}");
        calc.iterate();
        eq->calculate();
        assert((eq->data() == data_t{2, 3}));
    }

    // --- Same variable used twice in one expression ---
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = {1, 2}");
        Equation *eq = parser.parse_equation("B = A & A");
        calc.iterate();
        eq->calculate();
        assert((eq->data() == data_t{1, 2}));
    }

    // --- Whitespace handling ---
    {
        EquationCalculator calc;
        Parser parser(calc);
        Equation *eq = parser.parse_equation("  A   =   { 1 , 2 , 3 }  ");
        eq->calculate();
        assert((eq->data() == data_t{1, 2, 3}));
    }

    // --- Self-reference does not crash ---
    {
        EquationCalculator calc;
        Parser parser(calc);
        Equation *eq = parser.parse_equation("A = A | {1}");
        eq->calculate();
        assert((eq->data() == data_t{1}));
    }

    // --- Two mutually referencing equations ---
    {
        EquationCalculator calc;
        Parser parser(calc);
        parser.parse_equation("A = B | {1}");
        parser.parse_equation("B = {2}");
        calc.iterate();
        calc.iterate();
        VariableExpression *a = parser.variables().find("A");
        VariableExpression *b = parser.variables().find("B");
        assert(a != nullptr);
        assert(b != nullptr);
        assert((b->data() == data_t{2}));
        assert((a->data() == data_t{1, 2}));
    }

    // --- Syntax errors ---
    {
        EquationCalculator calc;
        Parser parser(calc);

        assert(throws_runtime_error([&] { parser.parse_equation(""); }));
        assert(throws_runtime_error([&] { parser.parse_equation("A"); }));
        assert(throws_runtime_error([&] { parser.parse_equation("A ="); }));
        assert(throws_runtime_error([&] { parser.parse_equation("= {1}"); }));
        assert(throws_runtime_error([&] { parser.parse_equation("A = {1"); }));
        assert(throws_runtime_error([&] { parser.parse_equation("A = {1, }"); }));
        assert(throws_runtime_error([&] { parser.parse_equation("A = {1 2}"); }));
        assert(throws_runtime_error([&] { parser.parse_equation("A = {x}"); }));
        assert(throws_runtime_error([&] { parser.parse_equation("A = ( {1} ")}; ));
        assert(throws_runtime_error([&] { parser.parse_equation("A = & {1}"); }));
        assert(throws_runtime_error([&] { parser.parse_equation("A = {1} extra"); }));
        assert(throws_runtime_error([&] { parser.parse_equation("A = {1} +"); }));
        assert(throws_runtime_error([&] { parser.parse_equation("A = {1} |"); }));
        assert(throws_runtime_error([&] { parser.parse_equation("A = {1} &"); }));
        assert(throws_runtime_error([&] { parser.parse_equation("A = ) "); }));
    }

    // --- Failed parse does not register an equation ---
    {
        EquationCalculator calc;
        Parser parser(calc);
        assert(throws_runtime_error([&] { parser.parse_equation("A ="); }));
        assert(calc.equations().empty());
    }

    // --- Failed parse does not register the LHS variable ---
    {
        // "A" is only created on successful parse; "B" inside the expression
        // is created eagerly by the variable parser.
        EquationCalculator calc;
        Parser parser(calc);
        assert(throws_runtime_error([&] { parser.parse_equation("A = B &"); }));
        assert(parser.variables().find("A") == nullptr);
    }

    std::cout << "All Parser tests passed\n";
    return 0;
}