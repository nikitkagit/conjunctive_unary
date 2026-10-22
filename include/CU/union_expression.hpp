#ifndef UNION_EXPRESSION_HPP
#define UNION_EXPRESSION_HPP

#include "expression.hpp"

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

private:
    std::vector<std::unique_ptr<Expression>> m_operands;
};

#endif // UNION_EXPRESSION_HPP