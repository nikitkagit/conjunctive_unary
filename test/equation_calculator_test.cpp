#include <cassert>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "CU/constant_expression.hpp"
#include "CU/equation.hpp"
#include "CU/equation_calculator.hpp"
#include "CU/expression_proxy.hpp"
#include "CU/intersection_expression.hpp"
#include "CU/sum_expression.hpp"
#include "CU/union_expression.hpp"
#include "CU/variable_expression.hpp"

namespace
{
std::unique_ptr<Expression> make_const(data_t value)
{
    return std::make_unique<ConstantExpression>(std::move(value));
}

std::vector<std::unique_ptr<Expression>> make_operands(
    std::initializer_list<data_t> values)
{
    std::vector<std::unique_ptr<Expression>> result;
    for (const data_t &v : values)
    {
        result.push_back(make_const(v));
    }
    return result;
}

std::unique_ptr<Expression> proxy_of(VariableExpression *variable)
{
    return std::make_unique<ExpressionProxy>(variable);
}
} // namespace

int main()
{
    // --- Equation ---

    // calculate() assigns expression result to the variable
    {
        VariableExpression variable;
        Equation equation(make_const({1, 2, 3}));
        equation.set_variable(&variable);
        equation.calculate();
        assert((variable.data() == data_t{1, 2, 3}));
        assert((equation.data() == data_t{1, 2, 3}));
    }

    // calculate() overwrites the previous variable value
    {
        VariableExpression variable({9, 10});
        Equation equation(make_const({1, 2}));
        equation.set_variable(&variable);
        equation.calculate();
        assert((variable.data() == data_t{1, 2}));
    }

    // calculate() without a variable does nothing
    {
        Equation equation(make_const({1, 2}));
        equation.calculate();
        assert(equation.data().empty());
    }

    // set_expression replaces the expression
    {
        VariableExpression variable;
        Equation equation(make_const({1}));
        equation.set_variable(&variable);
        equation.calculate();
        assert((variable.data() == data_t{1}));

        equation.set_expression(make_const({2, 3}));
        equation.calculate();
        assert((variable.data() == data_t{2, 3}));
    }

    // set_variable swaps the target
    {
        VariableExpression first;
        VariableExpression second;
        Equation equation(make_const({7, 8}));

        equation.set_variable(&first);
        equation.calculate();
        assert((first.data() == data_t{7, 8}));
        assert(second.data().empty());

        equation.set_variable(&second);
        equation.calculate();
        assert((second.data() == data_t{7, 8}));
    }

    // data() reflects external changes to the variable
    {
        VariableExpression variable({42});
        Equation equation(make_const({}));
        equation.set_variable(&variable);
        assert((equation.data() == data_t{42}));

        variable.set_data({1, 2, 3});
        assert((equation.data() == data_t{1, 2, 3}));
    }

    // Repeated calculate() on a constant expression is idempotent
    {
        VariableExpression variable;
        Equation equation(make_const({5}));
        equation.set_variable(&variable);
        equation.calculate();
        equation.calculate();
        assert((variable.data() == data_t{5}));
    }

    // Equation wrapping a composite expression
    {
        VariableExpression variable;
        Equation equation(
            std::make_unique<SumExpression>(make_operands({{1, 2}, {3, 4}})));
        equation.set_variable(&variable);
        equation.calculate();
        assert((variable.data() == data_t{4, 5, 6}));
    }

    // --- EquationCalculator ---

    // add_variable returns distinct non-null pointers
    {
        EquationCalculator calc;
        VariableExpression *a = calc.add_variable();
        VariableExpression *b = calc.add_variable();
        assert(a != nullptr);
        assert(b != nullptr);
        assert(a != b);
        assert(calc.variables().size() == 2);
    }

    // add_equation returns non-null and is registered
    {
        EquationCalculator calc;
        Equation *e = calc.add_equation(make_const({1, 2}));
        assert(e != nullptr);
        assert(calc.equations().size() == 1);
    }

    // iterate() updates every equation
    {
        EquationCalculator calc;
        VariableExpression *a = calc.add_variable();
        VariableExpression *b = calc.add_variable();

        Equation *ea = calc.add_equation(make_const({1, 2}));
        ea->set_variable(a);
        Equation *eb = calc.add_equation(make_const({3, 4}));
        eb->set_variable(b);

        calc.iterate();
        assert((a->data() == data_t{1, 2}));
        assert((b->data() == data_t{3, 4}));
    }

    // iterate() propagates through chains over multiple passes
    {
        // Insertion order: eb first (b = a + {2}), then ea (a = {1}).
        // Pass 1: b = {} + {2} = {}; a = {1}.
        // Pass 2: b = {1} + {2} = {3}; a = {1}.
        EquationCalculator calc;
        VariableExpression *a = calc.add_variable();
        VariableExpression *b = calc.add_variable();

        std::vector<std::unique_ptr<Expression>> sum_operands;
        sum_operands.push_back(proxy_of(a));
        sum_operands.push_back(make_const({2}));
        Equation *eb = calc.add_equation(
            std::make_unique<SumExpression>(std::move(sum_operands)));
        eb->set_variable(b);

        Equation *ea = calc.add_equation(make_const({1}));
        ea->set_variable(a);

        calc.iterate();
        assert((a->data() == data_t{1}));
        assert(b->data().empty());

        calc.iterate();
        assert((a->data() == data_t{1}));
        assert((b->data() == data_t{3}));
    }

    // Iterate over an empty calculator is safe
    {
        EquationCalculator calc;
        calc.iterate();
        assert(calc.variables().empty());
        assert(calc.equations().empty());
    }

    // Equations without a variable are calculated harmlessly
    {
        EquationCalculator calc;
        Equation *e = calc.add_equation(make_const({1}));
        calc.iterate();
        assert(e->data().empty());
    }

    // Union of a variable and a constant
    {
        // Insertion order: ea first (a = {1, 2}), eb second (b = a | {3}).
        // Pass 1: a = {1, 2}; b = {1, 2} | {3} = {1, 2, 3}.
        EquationCalculator calc;
        VariableExpression *a = calc.add_variable();
        VariableExpression *b = calc.add_variable();

        Equation *ea = calc.add_equation(make_const({1, 2}));
        ea->set_variable(a);

        std::vector<std::unique_ptr<Expression>> union_operands;
        union_operands.push_back(proxy_of(a));
        union_operands.push_back(make_const({3}));
        Equation *eb = calc.add_equation(
            std::make_unique<UnionExpression>(std::move(union_operands)));
        eb->set_variable(b);

        calc.iterate();
        assert((a->data() == data_t{1, 2}));
        assert((b->data() == data_t{1, 2, 3}));
    }

    // Intersection of two variables, both driven by constants
    {
        // a = {1, 2, 3}, b = {2, 3, 4}; c = a & b -> {2, 3}
        EquationCalculator calc;
        VariableExpression *a = calc.add_variable();
        VariableExpression *b = calc.add_variable();
        VariableExpression *c = calc.add_variable();

        Equation *ea = calc.add_equation(make_const({1, 2, 3}));
        ea->set_variable(a);
        Equation *eb = calc.add_equation(make_const({2, 3, 4}));
        eb->set_variable(b);

        std::vector<std::unique_ptr<Expression>> inter_operands;
        inter_operands.push_back(proxy_of(a));
        inter_operands.push_back(proxy_of(b));
        Equation *ec = calc.add_equation(
            std::make_unique<IntersectionExpression>(std::move(inter_operands)));
        ec->set_variable(c);

        calc.iterate();
        assert((a->data() == data_t{1, 2, 3}));
        assert((b->data() == data_t{2, 3, 4}));
        assert((c->data() == data_t{2, 3}));
    }

    // variables() and equations() expose non-null entries
    {
        EquationCalculator calc;
        calc.add_variable();
        calc.add_variable();
        calc.add_equation(make_const({}));

        for (const auto &v : calc.variables())
        {
            assert(v != nullptr);
        }
        for (const auto &e : calc.equations())
        {
            assert(e != nullptr);
        }
    }

    std::cout << "All Equation/EquationCalculator tests passed\n";
    return 0;
}