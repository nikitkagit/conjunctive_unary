#ifndef EQUATION_PARSER_HPP
#define EQUATION_PARSER_HPP

#include "CU/equation/equation.hpp"
#include "CU/parser/expression_parser.hpp"
#include "CU/parser/parser_base.hpp"
#include "CU/parser/variable_dictionary.hpp"
#include "CU/expression/variable_expression.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

/**
 * @brief Parser for equations over sets of natural numbers.
 *
 * Parses equations of the form
 * @code
 * VAR_NAME = expression
 * @endcode
 * Each call returns the parsed Equation together with the variables that
 * were created during the call. The caller is responsible for registering
 * both with an @ref EquationCalculator (or another owner), which keeps
 * the addresses stable and validates the non-owning pointers stored in
 * the dictionary.
 */
class EquationParser : public ParserBase
{
public:
    /**
     * @brief Result of a successful or partial parse.
     */
    struct Result
    {
        /// Variables created during the parse (owned by the caller).
        std::vector<std::unique_ptr<VariableExpression>> created_variables;
        /// Parsed equation, or nullptr on error.
        std::unique_ptr<Equation> equation;
    };

    /**
     * @brief Constructs a parser bound to the given dictionary.
     * @param variables Dictionary used to resolve and register variables.
     */
    explicit EquationParser(VariableDictionary &variables);

    /**
     * @brief Parses a single equation.
     * @param input Source text of the equation.
     * @return Created variables and the parsed equation.
     */
    Result parse_equation(const std::string &input);

    /// @brief Returns the referenced variable dictionary.
    VariableDictionary &variables();

    /// @brief Returns the referenced variable dictionary.
    const VariableDictionary &variables() const;

    /// @brief True if the last parse attempt failed.
    bool has_error() const;

    /// @brief Returns the message describing the last parse error.
    const std::string &error_message() const;

    /// @brief Returns the input position where the last error occurred.
    std::size_t error_position() const;

private:
    VariableDictionary &m_variables;
    ExpressionParser m_expression_parser;
};

#endif // EQUATION_PARSER_HPP