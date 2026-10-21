// test/test_equation.cpp
#include <cassert>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "CU/equation.hpp"
#include "CU/intersection_expression.hpp"
#include "CU/sum_expression.hpp"
#include "CU/union_expression.hpp"
#include "CU/variable_expression.hpp"

namespace
{
class ConstantExpression : public Expression
{
public:
    explicit ConstantExpression(data_t value) : m_value(std::move(value)) {}

    data_t calculate() const override { return m_value; }

private:
    data_t m_value;
};
} // namespace

int main()
{
    const ConstantExpression a({1, 2, 3});
    const ConstantExpression b({3, 4, 5});

    // 1. calculate() assigns the union result to the variable.
    {
        VariableExpression variable;
        Equation equation(std::make_unique<UnionExpression>(
            std::vector<const Expression *>{&a, &b}));
        equation.set_variable(&variable);
        equation.calculate();
        assert((variable.data() == data_t{1, 2, 3, 4, 5}));
        assert((equation.data() == data_t{1, 2, 3, 4, 5}));
    }

    // 2. calculate() assigns the intersection result, overwriting the
    //    variable's previous value.
    {
        VariableExpression variable({9, 10});
        Equation equation(std::make_unique<IntersectionExpression>(
            std::vector<const Expression *>{&a, &b}));
        equation.set_variable(&variable);
        equation.calculate();
        assert((variable.data() == data_t{3}));
    }

    // 3. calculate() assigns the pairwise sum result.
    {
        VariableExpression variable;
        Equation equation(std::make_unique<SumExpression>(
            std::vector<const Expression *>{&a, &b}));
        equation.set_variable(&variable);
        equation.calculate();
        assert((variable.data() == data_t{4, 5, 6, 7, 8}));
    }

    // 4. set_expression() replaces the expression used by calculate().
    {
        VariableExpression variable;
        Equation equation(std::make_unique<UnionExpression>(
            std::vector<const Expression *>{&a, &b}));
        equation.set_variable(&variable);
        equation.calculate();
        assert((variable.data() == data_t{1, 2, 3, 4, 5}));

        equation.set_expression(std::make_unique<IntersectionExpression>(
            std::vector<const Expression *>{&a, &b}));
        equation.calculate();
        assert((variable.data() == data_t{3}));
    }

    // 5. Repeated calculate() calls update the variable each time.
    {
        VariableExpression variable({7});
        Equation equation(std::make_unique<ConstantExpression>(data_t{1, 2}));
        equation.set_variable(&variable);
        equation.calculate();
        assert((variable.data() == data_t{1, 2}));
        equation.calculate();
        assert((variable.data() == data_t{1, 2}));
    }

    // 6. calculate() without a variable does nothing and does not crash.
    {
        Equation equation(std::make_unique<ConstantExpression>(data_t{1, 2}));
        equation.calculate();
        assert((equation.data() == data_t{}));
    }

    // 7. data() reflects the variable's current value, including external
    //    changes made after set_variable().
    {
        VariableExpression variable({42, 43});
        Equation equation(std::make_unique<ConstantExpression>(data_t{}));
        equation.set_variable(&variable);
        assert((equation.data() == data_t{42, 43}));

        variable.set_data({1, 2, 3});
        assert((equation.data() == data_t{1, 2, 3}));
    }

    // 8. Variable bound by set_variable() can be swapped.
    {
        VariableExpression first;
        VariableExpression second;
        Equation equation(std::make_unique<UnionExpression>(
            std::vector<const Expression *>{&a, &b}));

        equation.set_variable(&first);
        equation.calculate();
        assert((first.data() == data_t{1, 2, 3, 4, 5}));
        assert(second.data().empty());

        equation.set_variable(&second);
        equation.calculate();
        assert((second.data() == data_t{1, 2, 3, 4, 5}));
    }

    std::cout << "All Equation tests passed\n";
    return 0;
}