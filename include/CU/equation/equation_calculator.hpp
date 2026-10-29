#ifndef EQUATION_CALCULATOR_HPP
#define EQUATION_CALCULATOR_HPP

#include "CU/expression/expression.hpp"

class VariableExpression;
class Equation;

/**
 * @brief Owns variables and equations and performs iterative evaluation.
 *
 * Variables are created through the calculator and are owned by it.
 * Equations are added together with their target variable and can be
 * looked up by that variable. A single call to iterate() evaluates every
 * stored equation once, assigning results to their target variables.
 */
class EquationCalculator
{
public:
    EquationCalculator();
    
    ~EquationCalculator();
    
    EquationCalculator(const EquationCalculator &) = delete;
    
    EquationCalculator &operator=(const EquationCalculator &) = delete;
    
    EquationCalculator(EquationCalculator &&) noexcept;
    
    EquationCalculator &operator=(EquationCalculator &&) noexcept;

    /**
     * @brief Creates and owns a new variable.
     * @return Non-owning pointer to the created variable.
     */
    VariableExpression *add_variable();

    /**
     * @brief Creates and owns a new equation.
     * @param expression Owning pointer to the expression to evaluate.
     * @return Non-owning pointer to the created equation.
     */
    Equation *add_equation(std::unique_ptr<Expression> expression);

    /**
     * @brief Takes ownership of an already-created variable.
     * @param variable Owning pointer to the variable.
     * @return Non-owning pointer to the stored variable.
     */
    VariableExpression *add_variable(std::unique_ptr<VariableExpression> variable);

    /**
     * @brief Takes ownership of an already-created equation.
     * @param equation Owning pointer to the equation.
     * @return Non-owning pointer to the stored equation.
     */
    Equation *add_equation(std::unique_ptr<Equation> equation);
    
    /**
     * @brief Performs one iteration: calls calculate() on every equation.
     */
    void iterate();

    /**
     * @brief Returns the owned variables.
     * @return Const reference to the collection of variables.
     */
    const std::vector<std::unique_ptr<VariableExpression>> &variables() const;

    /**
     * @brief Returns the owned equations.
     * @return Const reference to the collection of equations.
     */
    const std::vector<std::unique_ptr<Equation>> &equations() const;

private:
    std::vector<std::unique_ptr<VariableExpression>> m_variables;
    std::vector<std::unique_ptr<Equation>> m_equations;
};

#endif // EQUATION_CALCULATOR_HPP