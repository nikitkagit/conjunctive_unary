#ifndef INTERSECTION_EXPRESSION_HPP
#define INTERSECTION_EXPRESSION_HPP

#include "expression.hpp"

/**
 * @brief Intersection of an arbitrary number of set expressions.
 */
class IntersectionExpression : public Expression
{
public:
    /**
     * @brief Constructs an intersection of the given operands.
     * @param operands Non-owning pointers to operand expressions.
     */
    explicit IntersectionExpression(std::vector<const Expression *> operands);

    data_t calculate() const override;

private:
    std::vector<const Expression *> m_operands;
};

#endif // INTERSECTION_EXPRESSION_HPP