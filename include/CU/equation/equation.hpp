#ifndef EQUATION_HPP
#define EQUATION_HPP

#include "CU/expression/expression.hpp"

class VariableExpression;

/**
 * @brief Encapsulates the state and logic of an equation over sets of
 *        natural numbers.
 *
 * The equation evaluates its expression and assigns the result to the
 * referenced variable. The variable is not owned by the equation.
 */
class Equation
{
public:
    /**
     * @brief Constructs an equation with the given expression.
     * @param expression Owning pointer to the expression to evaluate.
     */
    explicit Equation(std::unique_ptr<Expression> expression);

    /**
     * @brief Replaces the stored expression.
     * @param expression Owning pointer to the new expression.
     */
    void set_expression(std::unique_ptr<Expression> expression);

    /**
     * @brief Sets the target variable for the equation result.
     * @param variable Non-owning pointer to the variable.
     */
    void set_variable(VariableExpression *variable);

    /**
     * @brief Evaluates the stored expression and assigns the result to the
     *        target variable.
     */
    void calculate();

    /**
     * @brief Returns the current value of the target variable.
     * @return Const reference to the stored set.
     */
    const data_t &data() const;

    /**
     * @brief Returns the target variable of the equation.
     * @return Non-owning pointer to the target variable, or nullptr if none.
     */
    const VariableExpression *variable() const;

    /// @brief Returns the expression owned by this equation.
    const Expression &expression() const;

private:
    std::unique_ptr<Expression> m_expression;
    VariableExpression *m_variable = nullptr;
};

#endif // EQUATION_HPP