#ifndef INTERSECTION_EXPRESSION_HPP
#define INTERSECTION_EXPRESSION_HPP

#include "CU/expression/expression.hpp"

/**
 * @brief Intersection of an arbitrary number of set expressions.
 *
 * Owns its operands.
 */
class IntersectionExpression : public Expression
{
public:
    /**
     * @brief Constructs an intersection of the given operands.
     * @param operands Owning pointers to operand expressions.
     */
    explicit IntersectionExpression(std::vector<std::unique_ptr<Expression>> operands);

    data_t calculate() const override;

    void accept(ExpressionVisitor &visitor) const override;

    /// @brief Returns the operands owned by this expression.
    const std::vector<std::unique_ptr<Expression>> &operands() const;

private:
    std::vector<std::unique_ptr<Expression>> m_operands;
};

#endif // INTERSECTION_EXPRESSION_HPP