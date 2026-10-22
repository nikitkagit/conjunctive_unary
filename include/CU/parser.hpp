#ifndef PARSER_HPP
#define PARSER_HPP

#include "common.hpp"

#include "expression.hpp"
#include "variable_dictionary.hpp"

class Equation;
class EquationCalculator;

/**
 * @brief Parser for equations over sets of natural numbers.
 *
 * Parses equations of the form
 * @code
 * VAR_NAME = expression
 * @endcode
 * where @c expression may contain:
 *  - variable names (added to the dictionary on first use),
 *  - fixed sets such as @c {1, 2, 3},
 *  - parentheses for grouping,
 *  - operators @c & (intersection), @c + (pairwise sum) and @c |
 *    (union), listed from highest to lowest precedence.
 *
 * Each successful call constructs an Equation, binds it to the named
 * variable and registers it with the referenced calculator.
 *
 * Composite expressions own their operands, so the parsed tree needs no
 * external ownership pool. Variables inside the tree are referenced
 * through non-owning ExpressionProxy nodes; the variables themselves are
 * owned by the calculator.
 */
class Parser
{
public:
    /**
     * @brief Constructs a parser bound to the given calculator.
     * @param calculator Calculator that will own created equations and
     *        variables.
     */
    explicit Parser(EquationCalculator &calculator);

    /**
     * @brief Parses a single equation and registers it with the calculator.
     * @param input Source text of the equation.
     * @return Non-owning pointer to the created equation.
     * @throws std::runtime_error on syntax errors.
     */
    Equation *parse_equation(const std::string &input);

    /**
     * @brief Returns the owned variable dictionary.
     * @return Reference to the dictionary.
     */
    VariableDictionary &variables();

    /**
     * @brief Returns the owned variable dictionary.
     * @return Const reference to the dictionary.
     */
    const VariableDictionary &variables() const;

private:
    // Grammar productions, from lowest to highest precedence.
    std::unique_ptr<Expression> parse_expression();
    std::unique_ptr<Expression> parse_sum();
    std::unique_ptr<Expression> parse_intersection();
    std::unique_ptr<Expression> parse_primary();

    // Token-level helpers.
    void skip_whitespace();
    bool match(char c);
    void expect(char c);
    char peek() const;
    char next();
    bool at_end() const;
    std::string parse_identifier();
    num_t parse_number();
    data_t parse_fixed_set();

    EquationCalculator &m_calculator;
    VariableDictionary m_variables;

    // Per-call parsing state.
    std::string m_input;
    std::size_t m_pos = 0;
};

#endif // PARSER_HPP