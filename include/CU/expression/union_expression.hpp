#ifndef UNION_EXPRESSION_HPP
#define UNION_EXPRESSION_HPP

#include "CU/expression/expression.hpp"

#include <memory>
#include <vector>

/**
 * @brief Union of an arbitrary number of set expressions.
 *
 * Owns its operands.
 */
class UnionExpression : public Expression
{
public:
    /**
     * @brief Constructs a union of the given operands.
     * @param operands Owning pointers to operand expressions.
     */
    explicit UnionExpression(std::vector<std::unique_ptr<Expression>> operands);

    data_t calculate() const override;

    void accept(ExpressionVisitor &visitor) const override;

    /// @brief Returns the operands owned by this expression.
    const std::vector<std::unique_ptr<Expression>> &operands() const;

private:
    std::vector<std::unique_ptr<Expression>> m_operands;
};

#endif // UNION_EXPRESSION_HPP