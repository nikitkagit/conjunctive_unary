#include "expression/sum_expression.hpp"
#include "expression/expression_visitor.hpp"

SumExpression::SumExpression(std::vector<std::unique_ptr<Expression>> operands)
    : m_operands(std::move(operands))
{
}

data_t SumExpression::calculate() const
{
    if (m_operands.empty())
    {
        return data_t{};
    }

    data_t result = m_operands.front()->calculate();
    for (std::size_t i = 1; i < m_operands.size(); ++i)
    {
        const data_t part = m_operands[i]->calculate();
        data_t next;
        for (const num_t &lhs : result)
        {
            for (const num_t &rhs : part)
            {
                next.insert(lhs + rhs);
            }
        }
        result = std::move(next);
    }
    return result;
}

void SumExpression::accept(ExpressionVisitor &visitor) const
{
    visitor.visit(*this);
}
    
const std::vector<std::unique_ptr<Expression>> &SumExpression::operands() const
{
    return m_operands;
}