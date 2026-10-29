#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <cassert>

#include "CU/equation/equation.hpp"
#include "CU/equation/equation_calculator.hpp"
#include "CU/parser/equation_parser.hpp"
#include "CU/parser/variable_dictionary.hpp"

#include "CU/expression/variable_expression.hpp"

namespace
{
Equation *parse_eq(EquationParser &parser,
                   EquationCalculator &calc,
                   const std::string &input)
{
    EquationParser::Result result = parser.parse_equation(input);
    for (auto &variable : result.created_variables)
    {
        calc.add_variable(std::move(variable));
    }
    if (result.equation == nullptr)
    {
        return nullptr;
    }
    return calc.add_equation(std::move(result.equation));
}
} // namespace

int main()
{
    // --- Basic assignment of a fixed set ---
    {
        EquationCalculator calc;
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq = parse_eq(parser, calc, "A = {1, 2, 3}");
        assert(eq != nullptr);
        assert(!parser.has_error());
        eq->calculate();
        assert((eq->data() == data_t{1, 2, 3}));
    }

    // --- Empty fixed set ---
    {
        EquationCalculator calc;
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq = parse_eq(parser, calc, "A = {}");
        assert(eq != nullptr);
        assert(!parser.has_error());
        eq->calculate();
        assert(eq->data().empty());
    }

    // --- Union ---
    {
        EquationCalculator calc;
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq = parse_eq(parser, calc, "A = {1, 2} | {3, 4}");
        assert(eq != nullptr);
        assert(!parser.has_error());
        eq->calculate();
        assert((eq->data() == data_t{1, 2, 3, 4}));
    }

    // --- Intersection ---
    {
        EquationCalculator calc;
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq = parse_eq(parser, calc, "A = {1, 2, 3} & {2, 3, 4}");
        assert(eq != nullptr);
        assert(!parser.has_error());
        eq->calculate();
        assert((eq->data() == data_t{2, 3}));
    }

    // --- Pairwise sum ---
    {
        EquationCalculator calc;
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq = parse_eq(parser, calc, "A = {1, 2} + {3, 4}");
        assert(eq != nullptr);
        assert(!parser.has_error());
        eq->calculate();
        assert((eq->data() == data_t{4, 5, 6}));
    }

    // --- Precedence: & > + ---
    {
        EquationCalculator calc;
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq = parse_eq(parser, calc, "A = {1, 2} + {3} & {3}");
        assert(eq != nullptr);
        assert(!parser.has_error());
        eq->calculate();
        assert((eq->data() == data_t{4, 5}));
    }

    // --- Precedence: + > | ---
    {
        EquationCalculator calc;
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq = parse_eq(parser, calc, "A = {1} + {2} | {3}");
        assert(eq != nullptr);
        assert(!parser.has_error());
        eq->calculate();
        assert((eq->data() == data_t{3}));
    }

    // --- Precedence: | lowest, & highest ---
    {
        EquationCalculator calc;
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq = parse_eq(parser, calc, "A = {1, 2} | {3} & {3, 4}");
        assert(eq != nullptr);
        assert(!parser.has_error());
        eq->calculate();
        assert((eq->data() == data_t{1, 2, 3}));
    }

    // --- Parentheses override precedence ---
    {
        EquationCalculator calc;
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq = parse_eq(parser, calc, "A = ({1, 2} | {3}) & {3, 4}");
        assert(eq != nullptr);
        assert(!parser.has_error());
        eq->calculate();
        assert((eq->data() == data_t{3}));
    }

    // --- Nested parentheses ---
    {
        EquationCalculator calc;
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq = parse_eq(parser, calc, "A = (({1} + {2}) & {3})");
        assert(eq != nullptr);
        assert(!parser.has_error());
        eq->calculate();
        assert((eq->data() == data_t{3}));
    }

    // --- Chained operators of the same kind ---
    {
        EquationCalculator calc;
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq = parse_eq(parser, calc, "A = {1} | {2} | {3}");
        assert(eq != nullptr);
        assert(!parser.has_error());
        eq->calculate();
        assert((eq->data() == data_t{1, 2, 3}));
    }
    {
        EquationCalculator calc;
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq = parse_eq(parser, calc, "A = {1, 2, 3} & {2, 3, 4} & {3, 4, 5}");
        assert(eq != nullptr);
        assert(!parser.has_error());
        eq->calculate();
        assert((eq->data() == data_t{3}));
    }
    {
        EquationCalculator calc;
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq = parse_eq(parser, calc, "A = {1} + {2} + {3}");
        assert(eq != nullptr);
        assert(!parser.has_error());
        eq->calculate();
        assert((eq->data() == data_t{6}));
    }

    // --- Equation is registered in the calculator ---
    {
        EquationCalculator calc;
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq = parse_eq(parser, calc, "A = {1}");
        assert(eq != nullptr);
        assert(!parser.has_error());
        assert(calc.equations().size() == 1);
        assert(calc.equations().front().get() == eq);
    }

    // --- Unknown variable is created in dictionary ---
    {
        EquationCalculator calc;
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq = parse_eq(parser, calc, "A = {1}");
        assert(eq != nullptr);
        assert(!parser.has_error());
        VariableExpression *a = parser.variables().find("A");
        assert(a != nullptr);
        assert(parser.variables().name_of(a) == "A");
    }

    // --- Multiple equations share the dictionary ---
    {
        EquationCalculator calc;
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq1 = parse_eq(parser, calc, "A = {1, 2}");
        assert(eq1 != nullptr);
        assert(!parser.has_error());
        Equation *eq2 = parse_eq(parser, calc, "B = {3}");
        assert(eq2 != nullptr);
        assert(!parser.has_error());
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
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq_a = parse_eq(parser, calc, "A = {1, 2}");
        assert(eq_a != nullptr);
        assert(!parser.has_error());
        Equation *eq = parse_eq(parser, calc, "B = A | {3}");
        assert(eq != nullptr);
        assert(!parser.has_error());
        calc.iterate();
        eq->calculate();
        assert((eq->data() == data_t{1, 2, 3}));
    }

    // --- Variable reference in intersection ---
    {
        EquationCalculator calc;
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq_a = parse_eq(parser, calc, "A = {1, 2, 3}");
        assert(eq_a != nullptr);
        assert(!parser.has_error());
        Equation *eq = parse_eq(parser, calc, "B = A & {2, 3, 4}");
        assert(eq != nullptr);
        assert(!parser.has_error());
        calc.iterate();
        eq->calculate();
        assert((eq->data() == data_t{2, 3}));
    }

    // --- Same variable used twice in one expression ---
    {
        EquationCalculator calc;
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq_a = parse_eq(parser, calc, "A = {1, 2}");
        assert(eq_a != nullptr);
        assert(!parser.has_error());
        Equation *eq = parse_eq(parser, calc, "B = A & A");
        assert(eq != nullptr);
        assert(!parser.has_error());
        calc.iterate();
        eq->calculate();
        assert((eq->data() == data_t{1, 2}));
    }

    // --- Whitespace handling ---
    {
        EquationCalculator calc;
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq = parse_eq(parser, calc, "  A   =   { 1 , 2 , 3 }  ");
        assert(eq != nullptr);
        assert(!parser.has_error());
        eq->calculate();
        assert((eq->data() == data_t{1, 2, 3}));
    }

    // --- Self-reference does not crash ---
    {
        EquationCalculator calc;
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq = parse_eq(parser, calc, "A = A | {1}");
        assert(eq != nullptr);
        assert(!parser.has_error());
        eq->calculate();
        assert((eq->data() == data_t{1}));
    }

    // --- Two mutually referencing equations ---
    {
        EquationCalculator calc;
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq_a = parse_eq(parser, calc, "A = B | {1}");
        assert(eq_a != nullptr);
        assert(!parser.has_error());
        Equation *eq_b = parse_eq(parser, calc, "B = {2}");
        assert(eq_b != nullptr);
        assert(!parser.has_error());
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
        VariableDictionary dictionary;
        EquationParser parser(dictionary);

        Equation *eq = parse_eq(parser, calc, "");
        assert(eq == nullptr);
        assert(parser.has_error());

        eq = parse_eq(parser, calc, "A");
        assert(eq == nullptr);
        assert(parser.has_error());

        eq = parse_eq(parser, calc, "A =");
        assert(eq == nullptr);
        assert(parser.has_error());

        eq = parse_eq(parser, calc, "= {1}");
        assert(eq == nullptr);
        assert(parser.has_error());

        eq = parse_eq(parser, calc, "A = {1");
        assert(eq == nullptr);
        assert(parser.has_error());

        eq = parse_eq(parser, calc, "A = {1, }");
        assert(eq == nullptr);
        assert(parser.has_error());

        eq = parse_eq(parser, calc, "A = {1 2}");
        assert(eq == nullptr);
        assert(parser.has_error());

        eq = parse_eq(parser, calc, "A = {x}");
        assert(eq == nullptr);
        assert(parser.has_error());

        eq = parse_eq(parser, calc, "A = ( {1} ");
        assert(eq == nullptr);
        assert(parser.has_error());

        eq = parse_eq(parser, calc, "A = & {1}");
        assert(eq == nullptr);
        assert(parser.has_error());

        eq = parse_eq(parser, calc, "A = {1} extra");
        assert(eq == nullptr);
        assert(parser.has_error());

        eq = parse_eq(parser, calc, "A = {1} +");
        assert(eq == nullptr);
        assert(parser.has_error());

        eq = parse_eq(parser, calc, "A = {1} |");
        assert(eq == nullptr);
        assert(parser.has_error());

        eq = parse_eq(parser, calc, "A = {1} &");
        assert(eq == nullptr);
        assert(parser.has_error());

        eq = parse_eq(parser, calc, "A = ) ");
        assert(eq == nullptr);
        assert(parser.has_error());
    }

    // --- Failed parse does not register an equation ---
    {
        EquationCalculator calc;
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq = parse_eq(parser, calc, "A =");
        assert(eq == nullptr);
        assert(parser.has_error());
        assert(calc.equations().empty());
    }

    // --- Failed parse does not register the LHS variable ---
    {
        EquationCalculator calc;
        VariableDictionary dictionary;
        EquationParser parser(dictionary);
        Equation *eq = parse_eq(parser, calc, "A = B &");
        assert(eq == nullptr);
        assert(parser.has_error());
        assert(parser.variables().find("A") == nullptr);
    }

    std::cout << "All EquationParser tests passed\n";
    return 0;
}