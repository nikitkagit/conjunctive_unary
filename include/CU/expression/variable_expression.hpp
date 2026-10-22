#ifndef VARIABLE_EXPRESSION_HPP
#define VARIABLE_EXPRESSION_HPP

#include "expression.hpp"

/**
 * @brief Variable expression holding a mutable set value.
 *
 * The expression evaluates to its current stored value, which can be
 * changed through the setter. This allows the variable to act as an
 * assignable target for equation results.
 */
class VariableExpression : public Expression
{
public:
    VariableExpression() = default;

    /**
     * @brief Constructs a variable with the given initial value.
     * @param data Initial value of the variable.
     */
    explicit VariableExpression(data_t data);

    data_t calculate() const override;

    /**
     * @brief Sets the current value of the variable.
     * @param data New value of the variable.
     */
    void set_data(data_t data);

    /**
     * @brief Returns the current value of the variable.
     * @return Const reference to the stored set.
     */
    const data_t &data() const;

private:
    data_t m_data;
};

#endif // VARIABLE_EXPRESSION_HPP