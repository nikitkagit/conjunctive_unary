#include "CU/equation/equation_calculator.hpp"

#include "CU/equation/equation.hpp"
#include "CU/expression/variable_expression.hpp"

EquationCalculator::EquationCalculator() = default;
EquationCalculator::~EquationCalculator() = default;
EquationCalculator::EquationCalculator(EquationCalculator &&) noexcept = default;
EquationCalculator &EquationCalculator::operator=(EquationCalculator &&) noexcept = default;

VariableExpression *EquationCalculator::add_variable()
{
    m_variables.push_back(std::make_unique<VariableExpression>());
    return m_variables.back().get();
}

Equation *EquationCalculator::add_equation(std::unique_ptr<Expression> expression)
{
    m_equations.push_back(std::make_unique<Equation>(std::move(expression)));
    return m_equations.back().get();
}

VariableExpression *EquationCalculator::add_variable(
    std::unique_ptr<VariableExpression> variable)
{
    m_variables.push_back(std::move(variable));
    return m_variables.back().get();
}

Equation *EquationCalculator::add_equation(std::unique_ptr<Equation> equation)
{
    m_equations.push_back(std::move(equation));
    return m_equations.back().get();
}

void EquationCalculator::iterate()
{
    for (const auto &equation : m_equations)
    {
        equation->calculate();
    }
}

const std::vector<std::unique_ptr<VariableExpression>> &EquationCalculator::variables() const
{
    return m_variables;
}

const std::vector<std::unique_ptr<Equation>> &EquationCalculator::equations() const
{
    return m_equations;
}