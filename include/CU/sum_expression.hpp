#ifndef SUM_EXPRESSION_HPP
#define SUM_EXPRESSION_HPP

#include "expression.hpp"

/**
 * @brief Pairwise sum of all elements of the given set expressions.
 *
 * The result contains every value that can be obtained by taking one
 * element from each operand and summing them together.
 */
class SumExpression : public Expression
{
public:
    /**
     * @brief Constructs a pairwise sum of the given operands.
     * @param operands Non-owning pointers to operand expressions.
     */
    explicit SumExpression(std::vector<const Expression *> operands);

    data_t calculate() const override;

private:
    std::vector<const Expression *> m_operands;
};

#endif // SUM_EXPRESSION_HPP