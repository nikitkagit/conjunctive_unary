#ifndef CONSTANT_EXPRESSION_HPP
#define CONSTANT_EXPRESSION_HPP

#include "expression.hpp"

/**
 * @brief Expression that always evaluates to a fixed set of numbers.
 */
class ConstantExpression : public Expression
{
public:
    /**
     * @brief Constructs a constant expression with the given value.
     * @param value Fixed value of the expression.
     */
    explicit ConstantExpression(data_t value);

    data_t calculate() const override;

private:
    data_t m_value;
};

#endif // CONSTANT_EXPRESSION_HPP