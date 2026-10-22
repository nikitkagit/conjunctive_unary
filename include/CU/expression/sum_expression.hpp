#ifndef SUM_EXPRESSION_HPP
#define SUM_EXPRESSION_HPP

#include "CU/expression/expression.hpp"

#include <memory>
#include <vector>

/**
 * @brief Pairwise sum of all elements of the given set expressions.
 *
 * The result contains every value that can be obtained by taking one
 * element from each operand and summing them together. Owns its operands.
 */
class SumExpression : public Expression
{
public:
    /**
     * @brief Constructs a pairwise sum of the given operands.
     * @param operands Owning pointers to operand expressions.
     */
    explicit SumExpression(std::vector<std::unique_ptr<Expression>> operands);

    data_t calculate() const override;

    void accept(ExpressionVisitor &visitor) const override;

    /// @brief Returns the operands owned by this expression.
    const std::vector<std::unique_ptr<Expression>> &operands() const;

private:
    std::vector<std::unique_ptr<Expression>> m_operands;
};

#endif // SUM_EXPRESSION_HPP