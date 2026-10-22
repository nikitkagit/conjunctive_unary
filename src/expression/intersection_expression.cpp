#include "expression/intersection_expression.hpp"

IntersectionExpression::IntersectionExpression(std::vector<std::unique_ptr<Expression>> operands)
    : m_operands(std::move(operands))
{
}

data_t IntersectionExpression::calculate() const
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
        for (const num_t &value : result)
        {
            if (part.find(value) != part.end())
            {
                next.insert(value);
            }
        }
        result = std::move(next);
        if (result.empty())
        {
            break;
        }
    }
    return result;
}