#include "constant_expression.hpp"

ConstantExpression::ConstantExpression(data_t value)
    : m_value(std::move(value))
{
}

data_t ConstantExpression::calculate() const
{
    return m_value;
}