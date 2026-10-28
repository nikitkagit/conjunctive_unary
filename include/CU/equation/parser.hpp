#ifndef CU_EQUATION_PARSER_HPP
#define CU_EQUATION_PARSER_HPP

#include "CU/equation/variable_dictionary.hpp"

#include "CU/expression/variable_expression.hpp"

#include <memory>
#include <optional>
#include <string>

class Equation;
class EquationCalculator;
class Expression;

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
     * @return The parsed equation, or nullptr on syntax error.
     */
    Equation * parse_equation(const std::string &input);

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

    /**
     * @brief Returns the message describing the last parse error.
     *
     * Empty if no error has occurred since the last successful parse.
     */
    const std::string &error_message() const;

    /**
     * @brief Returns the input position where the last error occurred.
     */
    std::size_t error_position() const;

    /// @brief True if the last parse attempt failed.
    bool has_error() const;

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

    /**
     * @brief Records a parse error at the current input position.
     */
    void set_error(const std::string &message);

    std::string m_error_message;
    std::size_t m_error_position = 0;
};

#endif // CU_EQUATION_PARSER_HPP