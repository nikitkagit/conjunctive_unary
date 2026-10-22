#include "equation/parser.hpp"

#include "equation/equation.hpp"
#include "equation/equation_calculator.hpp"

#include "expression/constant_expression.hpp"
#include "expression/expression_proxy.hpp"
#include "expression/intersection_expression.hpp"
#include "expression/sum_expression.hpp"
#include "expression/union_expression.hpp"
#include "expression/variable_expression.hpp"

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

Equation *Parser::parse_equation(const std::string &input)
{
    m_input = input;
    m_pos = 0;

    skip_whitespace();
    const std::string name = parse_identifier();
    skip_whitespace();
    expect('=');

    std::unique_ptr<Expression> root = parse_expression();
    skip_whitespace();
    if (!at_end())
    {
        throw std::runtime_error("Unexpected trailing input in equation");
    }

    VariableExpression *variable = m_variables.get_or_create(name);
    Equation *equation = m_calculator.add_equation(std::move(root));
    equation->set_variable(variable);
    return equation;
}

std::unique_ptr<Expression> Parser::parse_expression()
{
    std::unique_ptr<Expression> first = parse_sum();
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
        operands.push_back(parse_sum());
    }

    return std::make_unique<UnionExpression>(std::move(operands));
}

std::unique_ptr<Expression> Parser::parse_sum()
{
    std::unique_ptr<Expression> first = parse_intersection();
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
        operands.push_back(parse_intersection());
    }

    return std::make_unique<SumExpression>(std::move(operands));
}

std::unique_ptr<Expression> Parser::parse_intersection()
{
    std::unique_ptr<Expression> first = parse_primary();
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
        operands.push_back(parse_primary());
    }

    return std::make_unique<IntersectionExpression>(std::move(operands));
}

std::unique_ptr<Expression> Parser::parse_primary()
{
    skip_whitespace();
    if (at_end())
    {
        throw std::runtime_error("Unexpected end of expression");
    }

    const char c = peek();
    if (c == '(')
    {
        next();
        std::unique_ptr<Expression> inner = parse_expression();
        skip_whitespace();
        expect(')');
        return inner;
    }
    if (c == '{')
    {
        data_t value = parse_fixed_set();
        return std::make_unique<ConstantExpression>(std::move(value));
    }

    const std::string name = parse_identifier();
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
    if (at_end() || peek() != c)
    {
        throw std::runtime_error(std::string("Expected '") + c + "'");
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
    if (at_end() ||
        !(std::isalpha(static_cast<unsigned char>(peek())) || peek() == '_'))
    {
        throw std::runtime_error("Expected identifier");
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
    if (at_end() || !std::isdigit(static_cast<unsigned char>(peek())))
    {
        throw std::runtime_error("Expected number");
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

    data_t result;
    skip_whitespace();
    if (match('}'))
    {
        return result;
    }

    while (true)
    {
        const num_t value = parse_number();
        result.insert(value);
        skip_whitespace();
        if (match('}'))
        {
            break;
        }
        expect(',');
    }
    return result;
}