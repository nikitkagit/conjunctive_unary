#include "expression/constant_expression.hpp"
#include "expression/expression_visitor.hpp"

ConstantExpression::ConstantExpression(data_t value)
    : m_value(std::move(value))
{
}

data_t ConstantExpression::calculate() const
{
    return m_value;
}

void ConstantExpression::accept(ExpressionVisitor &visitor) const
{
    visitor.visit(*this);
}