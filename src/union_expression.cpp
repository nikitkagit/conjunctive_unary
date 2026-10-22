#include "union_expression.hpp"

UnionExpression::UnionExpression(std::vector<std::unique_ptr<Expression>> operands)
    : m_operands(std::move(operands))
{
}

data_t UnionExpression::calculate() const
{
    data_t result;
    for (const auto &operand : m_operands)
    {
        const data_t part = operand->calculate();
        result.insert(part.begin(), part.end());
    }
    return result;
}