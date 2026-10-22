#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include "CU/common.hpp"

class ExpressionVisitor;

/**
 * @brief Abstract base class for an expression over sets of natural numbers.
 */
class Expression
{
public:
    virtual ~Expression() = default;

    /**
     * @brief Evaluates the expression and returns the resulting set.
     * @return Resulting set of natural numbers.
     */
    virtual data_t calculate() const = 0;

    /// @brief Dispatches to the given visitor.
    virtual void accept(ExpressionVisitor &visitor) const = 0;
};

#endif // EXPRESSION_HPP