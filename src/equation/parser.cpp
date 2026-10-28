#include "CU/equation/parser.hpp"

#include "CU/equation/equation.hpp"
#include "CU/equation/equation_calculator.hpp"

#include "CU/expression/constant_expression.hpp"
#include "CU/expression/expression_proxy.hpp"
#include "CU/expression/intersection_expression.hpp"
#include "CU/expression/sum_expression.hpp"
#include "CU/expression/union_expression.hpp"
#include "CU/expression/variable_expression.hpp"

Parser::Parser(EquationCalculator &calculator)
    : m_calculator(calculator)
    , m_variables(calculator)
{
}

VariableDictionary &Parser::variables()
{
    return m_variables;
}

const VariableDictionary &Parser::variables() const
{
    return m_variables;
}

const std::string &Parser::error_message() const
{
    return m_error_message;
}

std::size_t Parser::error_position() const
{
    return m_error_position;
}

bool Parser::has_error() const
{
    return !m_error_message.empty();
}

void Parser::set_error(const std::string &message)
{
    m_error_message = message;
    m_error_position = m_pos;
}

Equation * Parser::parse_equation(const std::string &input)
{
    m_input = input;
    m_pos = 0;
    m_error_message.clear();
    m_error_position = 0;

    skip_whitespace();
    const std::string name = parse_identifier();
    if (has_error())
    {
        return nullptr;
    }
    skip_whitespace();
    expect('=');
    if (has_error())
    {
        return nullptr;
    }

    std::unique_ptr<Expression> root = parse_expression();
    if (has_error())
    {
        return nullptr;
    }
    skip_whitespace();
    if (!at_end())
    {
        set_error("Unexpected trailing input: '"
                  + m_input.substr(m_pos) + "'");
        return nullptr;
    }

    VariableExpression *variable = m_variables.get_or_create(name);
    Equation *equation = m_calculator.add_equation(std::move(root));
    equation->set_variable(variable);
    return equation;
}

std::unique_ptr<Expression> Parser::parse_expression()
{
    std::unique_ptr<Expression> first = parse_sum();
    if (has_error())
    {
        return nullptr;
    }
    skip_whitespace();
    if (at_end() || peek() != '|')
    {
        return first;
    }

    std::vector<std::unique_ptr<Expression>> operands;
    operands.push_back(std::move(first));

    while (true)
    {
        skip_whitespace();
        if (at_end() || peek() != '|')
        {
            break;
        }
        next();
        std::unique_ptr<Expression> operand = parse_sum();
        if (has_error())
        {
            return nullptr;
        }
        operands.push_back(std::move(operand));
    }

    return std::make_unique<UnionExpression>(std::move(operands));
}

std::unique_ptr<Expression> Parser::parse_sum()
{
    std::unique_ptr<Expression> first = parse_intersection();
    if (has_error())
    {
        return nullptr;
    }
    skip_whitespace();
    if (at_end() || peek() != '+')
    {
        return first;
    }

    std::vector<std::unique_ptr<Expression>> operands;
    operands.push_back(std::move(first));

    while (true)
    {
        skip_whitespace();
        if (at_end() || peek() != '+')
        {
            break;
        }
        next();
        std::unique_ptr<Expression> operand = parse_intersection();
        if (has_error())
        {
            return nullptr;
        }
        operands.push_back(std::move(operand));
    }

    return std::make_unique<SumExpression>(std::move(operands));
}

std::unique_ptr<Expression> Parser::parse_intersection()
{
    std::unique_ptr<Expression> first = parse_primary();
    if (has_error())
    {
        return nullptr;
    }
    skip_whitespace();
    if (at_end() || peek() != '&')
    {
        return first;
    }

    std::vector<std::unique_ptr<Expression>> operands;
    operands.push_back(std::move(first));

    while (true)
    {
        skip_whitespace();
        if (at_end() || peek() != '&')
        {
            break;
        }
        next();
        std::unique_ptr<Expression> operand = parse_primary();
        if (has_error())
        {
            return nullptr;
        }
        operands.push_back(std::move(operand));
    }

    return std::make_unique<IntersectionExpression>(std::move(operands));
}

std::unique_ptr<Expression> Parser::parse_primary()
{
    skip_whitespace();
    if (at_end())
    {
        set_error("Unexpected end of expression");
        return nullptr;
    }

    const char c = peek();
    if (c == '(')
    {
        next();
        std::unique_ptr<Expression> inner = parse_expression();
        if (has_error())
        {
            return nullptr;
        }
        skip_whitespace();
        expect(')');
        if (has_error())
        {
            return nullptr;
        }
        return inner;
    }
    if (c == '{')
    {
        data_t value = parse_fixed_set();
        if (has_error())
        {
            return nullptr;
        }
        return std::make_unique<ConstantExpression>(std::move(value));
    }

    const std::string name = parse_identifier();
    if (has_error())
    {
        return nullptr;
    }
    VariableExpression *variable = m_variables.get_or_create(name);
    return std::make_unique<ExpressionProxy>(variable);
}

void Parser::skip_whitespace()
{
    while (!at_end() && std::isspace(static_cast<unsigned char>(peek())))
    {
        ++m_pos;
    }
}

bool Parser::match(char c)
{
    skip_whitespace();
    if (!at_end() && peek() == c)
    {
        ++m_pos;
        return true;
    }
    return false;
}

void Parser::expect(char c)
{
    skip_whitespace();
    if (at_end())
    {
        set_error(std::string("Unexpected end of input, expected '") + c + "'");
        return;
    }
    if (peek() != c)
    {
        set_error(std::string("Expected '") + c + "', got '" + peek() + "'");
        return;
    }
    ++m_pos;
}

char Parser::peek() const
{
    return m_input[m_pos];
}

char Parser::next()
{
    return m_input[m_pos++];
}

bool Parser::at_end() const
{
    return m_pos >= m_input.size();
}

std::string Parser::parse_identifier()
{
    skip_whitespace();
    if (at_end())
    {
        set_error("Expected identifier, got end of input");
        return {};
    }
    if (!(std::isalpha(static_cast<unsigned char>(peek())) || peek() == '_'))
    {
        set_error(std::string("Expected identifier, got '") + peek() + "'");
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

num_t Parser::parse_number()
{
    skip_whitespace();
    if (at_end())
    {
        set_error("Expected number, got end of input");
        return 0;
    }
    if (!std::isdigit(static_cast<unsigned char>(peek())))
    {
        set_error(std::string("Expected number, got '") + peek() + "'");
        return 0;
    }

    num_t value = 0;
    while (!at_end() && std::isdigit(static_cast<unsigned char>(peek())))
    {
        value = value * 10 + static_cast<num_t>(next() - '0');
    }
    return value;
}

data_t Parser::parse_fixed_set()
{
    expect('{');
    if (has_error())
    {
        return {};
    }

    data_t result;
    skip_whitespace();
    if (match('}'))
    {
        return result;
    }

    while (true)
    {
        const num_t value = parse_number();
        if (has_error())
        {
            return {};
        }
        result.insert(value);
        skip_whitespace();
        if (match('}'))
        {
            break;
        }
        expect(',');
        if (has_error())
        {
            return {};
        }
    }
    return result;
}