#include "CU/parser/parser_base.hpp"

#include <cctype>
#include <utility>

const ErrorReporter &ParserBase::error_reporter() const noexcept
{
    return m_error;
}

void ParserBase::reset() noexcept
{
    m_input.clear();
    m_pos = 0;
    m_error.reset();
    m_error_position = 0;
}

void ParserBase::set_input(std::string input)
{
    m_input = std::move(input);
    m_pos = 0;
    m_error.reset();
    m_error_position = 0;
}

void ParserBase::skip_whitespace()
{
    while (!at_end() && std::isspace(static_cast<unsigned char>(peek())))
    {
        ++m_pos;
    }
}

char ParserBase::peek() const
{
    return m_pos < m_input.size() ? m_input[m_pos] : '\0';
}

char ParserBase::next()
{
    return m_input[m_pos++];
}

bool ParserBase::at_end() const
{
    return m_pos >= m_input.size();
}

bool ParserBase::match(char c)
{
    skip_whitespace();
    if (!at_end() && peek() == c)
    {
        ++m_pos;
        return true;
    }
    return false;
}

void ParserBase::expect(char c)
{
    skip_whitespace();
    if (at_end())
    {
        report_error(std::string("Unexpected end of input, expected '") + c + "'");
        return;
    }
    if (peek() != c)
    {
        report_error(std::string("Expected '") + c + "', got '" + peek() + "'");
        return;
    }
    ++m_pos;
}

std::string ParserBase::parse_identifier()
{
    skip_whitespace();
    if (at_end())
    {
        report_error("Expected identifier, got end of input");
        return {};
    }
    if (!(std::isalpha(static_cast<unsigned char>(peek())) || peek() == '_'))
    {
        report_error(std::string("Expected identifier, got '") + peek() + "'");
        return {};
    }

    std::string name;
    while (!at_end())
    {
        const char c = peek();
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_')
        {
            name += next();
        }
        else
        {
            break;
        }
    }
    return name;
}

num_t ParserBase::parse_number()
{
    skip_whitespace();
    if (at_end())
    {
        report_error("Expected number, got end of input");
        return 0;
    }
    if (!std::isdigit(static_cast<unsigned char>(peek())))
    {
        report_error(std::string("Expected number, got '") + peek() + "'");
        return 0;
    }

    num_t value = 0;
    while (!at_end() && std::isdigit(static_cast<unsigned char>(peek())))
    {
        value = value * 10 + static_cast<num_t>(next() - '0');
    }
    return value;
}

void ParserBase::report_error(std::string message)
{
    m_error.report(std::move(message));
    m_error_position = m_pos;
}