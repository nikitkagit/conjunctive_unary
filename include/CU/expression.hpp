#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include "common.hpp"

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
};

#endif // EXPRESSION_HPP