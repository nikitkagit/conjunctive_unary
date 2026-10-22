#include "expression/union_expression.hpp"
#include "expression/expression_visitor.hpp"

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

void UnionExpression::accept(ExpressionVisitor &visitor) const
{
    visitor.visit(*this);
}
    
const std::vector<std::unique_ptr<Expression>> &UnionExpression::operands() const
{
    return m_operands;
}