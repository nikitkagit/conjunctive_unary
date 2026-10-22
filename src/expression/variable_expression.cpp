#include "CU/expression/variable_expression.hpp"
#include "CU/expression/expression_visitor.hpp"

VariableExpression::VariableExpression(data_t data)
    : m_data(std::move(data))
{
}

data_t VariableExpression::calculate() const
{
    return m_data;
}

void VariableExpression::set_data(data_t data)
{
    m_data = std::move(data);
}

const data_t &VariableExpression::data() const
{
    return m_data;
}

void VariableExpression::accept(ExpressionVisitor &visitor) const
{
    visitor.visit(*this);
}