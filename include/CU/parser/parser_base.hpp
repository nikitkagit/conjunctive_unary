#ifndef PARSER_BASE_HPP
#define PARSER_BASE_HPP

#include "CU/common.hpp"
#include "CU/utils/error_reporter.hpp"

#include <cstddef>
#include <string>

/**
 * @brief Common lexical primitives shared by expression and equation
 *        parsers.
 *
 * Holds the source text, the current position and the error reporter,
 * and provides low-level token operations such as skipping whitespace,
 * peeking and consuming characters, expecting a specific character,
 * parsing identifiers and numbers.
 *
 * Concrete parsers inherit from this class to reuse the primitives and
 * expose their own high-level grammar productions.
 */
class ParserBase
{
public:
    /** @brief Const access to the error reporter. */
    const ErrorReporter &error_reporter() const noexcept;

    /** @brief Clears the stored error and resets its position. */
    void reset() noexcept;

protected:
    /**
     * @brief Installs a new input string and rewinds the position.
     * @param input Source text.
     */
    void set_input(std::string input);

    /// @brief Skips whitespace at the current position.
    void skip_whitespace();

    /// @brief Returns the character at the current position (or '\\0').
    char peek() const;

    /// @brief Returns the current character and advances the position.
    char next();

    /// @brief True if the current position is at or past the end.
    bool at_end() const;

    /// @brief Consumes @p c after skipping whitespace, if present.
    bool match(char c);

    /**
     * @brief Consumes @p c after skipping whitespace.
     *
     * Reports an error if the next character is different or the end of
     * input has been reached.
     */
    void expect(char c);

    /**
     * @brief Reads an identifier from the current position.
     * @return Parsed identifier, or an empty string on error.
     */
    std::string parse_identifier();

    /**
     * @brief Reads a non-negative integer from the current position.
     * @return Parsed number, or zero on error.
     */
    num_t parse_number();

    /**
     * @brief Records @p message as an error at the current position.
     */
    void report_error(std::string message);

    std::string m_input;
    std::size_t m_pos = 0;
    ErrorReporter m_error;
    std::size_t m_error_position = 0;
};

#endif // PARSER_BASE_HPP