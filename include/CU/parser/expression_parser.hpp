#ifndef EXPRESSION_PARSER_HPP
#define EXPRESSION_PARSER_HPP

#include "CU/parser/parser_base.hpp"
#include "CU/parser/variable_dictionary.hpp"
#include "CU/expression/variable_expression.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

class Expression;

/**
 * @brief Parser for expressions over sets of natural numbers.
 *
 * Reads an expression from a string and reports the position where the
 * expression ends, so that an external parser can continue afterwards.
 *
 * The grammar supports variable names, fixed sets such as @c {1, 2, 3},
 * parentheses and the operators @c &, @c + and @c | with precedence
 * (from highest to lowest): @c &, @c +, @c |.
 *
 * Errors are reported through the internal @ref ErrorReporter and are
 * never thrown.
 */
class ExpressionParser : public ParserBase
{
public:
    /**
     * @brief Constructs a parser bound to the given dictionary.
     * @param variables Dictionary used to resolve and create variables.
     */
    explicit ExpressionParser(VariableDictionary &variables);

    /**
     * @brief Parses an expression from the given string.
     *
     * Parsing starts at the beginning of @p input; leading whitespace is
     * skipped. On success the returned expression is owned by the caller
     * and @ref end_position returns the offset immediately after the
     * parsed expression. On failure @c nullptr is returned and the error
     * is available via @ref error_reporter.
     *
     * @param input Source text.
     * @return Parsed expression, or nullptr on error.
     */
    std::unique_ptr<Expression> parse(const std::string &input);

    /**
     * @brief Position immediately after the last parsed expression.
     *
     * On a successful parse this is the offset after the expression; on a
     * failed parse it is the position where the parser stopped.
     */
    std::size_t end_position() const noexcept;

    /**
     * @brief Drains the variables created during the last parse session.
     *
     * The caller takes ownership of the returned variables and is
     * responsible for keeping their addresses stable (e.g. by moving them
     * into an @ref EquationCalculator). The dictionary still holds
     * non-owning pointers to them.
     *
     * @return Variables created since the last call.
     */
    std::vector<std::unique_ptr<VariableExpression>> take_created_variables();

private:
    std::unique_ptr<Expression> parse_expression();
    std::unique_ptr<Expression> parse_sum();
    std::unique_ptr<Expression> parse_intersection();
    std::unique_ptr<Expression> parse_primary();
    data_t parse_fixed_set();
    VariableExpression *find_or_create(const std::string &name);

    VariableDictionary &m_variables;
    std::vector<std::unique_ptr<VariableExpression>> m_created;
};

#endif // EXPRESSION_PARSER_HPP