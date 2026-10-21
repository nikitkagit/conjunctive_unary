#ifndef UNION_EXPRESSION_HPP
#define UNION_EXPRESSION_HPP

#include "expression.hpp"

/**
 * @brief Union of an arbitrary number of set expressions.
 */
class UnionExpression : public Expression
{
public:
    /**
     * @brief Constructs a union of the given operands.
     * @param operands Non-owning pointers to operand expressions.
     */
    explicit UnionExpression(std::vector<const Expression *> operands);

    data_t calculate() const override;

private:
    std::vector<const Expression *> m_operands;
};

#endif // UNION_EXPRESSION_HPP