#include "equation/equation.hpp"

#include "expression/variable_expression.hpp"

Equation::Equation(std::unique_ptr<Expression> expression)
    : m_expression(std::move(expression))
{
}

void Equation::set_expression(std::unique_ptr<Expression> expression)
{
    m_expression = std::move(expression);
}

void Equation::set_variable(VariableExpression *variable)
{
    m_variable = variable;
}

void Equation::calculate()
{
    if (m_variable != nullptr)
    {
        m_variable->set_data(m_expression->calculate());
    }
}

const data_t &Equation::data() const
{
    static const data_t empty;
    return m_variable != nullptr ? m_variable->data() : empty;
}