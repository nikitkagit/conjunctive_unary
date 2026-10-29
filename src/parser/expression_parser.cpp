#include "CU/parser/expression_parser.hpp"

#include "CU/expression/constant_expression.hpp"
#include "CU/expression/expression.hpp"
#include "CU/expression/expression_proxy.hpp"
#include "CU/expression/intersection_expression.hpp"
#include "CU/expression/sum_expression.hpp"
#include "CU/expression/union_expression.hpp"
#include "CU/expression/variable_expression.hpp"

#include <utility>

ExpressionParser::ExpressionParser(VariableDictionary &variables)
    : m_variables(variables)
{
}

std::unique_ptr<Expression> ExpressionParser::parse(const std::string &input)
{
    set_input(input);
    std::unique_ptr<Expression> result = parse_expression();
    if (error_reporter().has_error())
    {
        return nullptr;
    }
    return result;
}

std::size_t ExpressionParser::end_position() const noexcept
{
    return m_pos;
}

std::vector<std::unique_ptr<VariableExpression>>
ExpressionParser::take_created_variables()
{
    return std::move(m_created);
}

std::unique_ptr<Expression> ExpressionParser::parse_expression()
{
    std::unique_ptr<Expression> first = parse_sum();
    if (error_reporter().has_error())
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
        if (error_reporter().has_error())
        {
            return nullptr;
        }
        operands.push_back(std::move(operand));
    }

    return std::make_unique<UnionExpression>(std::move(operands));
}

std::unique_ptr<Expression> ExpressionParser::parse_sum()
{
    std::unique_ptr<Expression> first = parse_intersection();
    if (error_reporter().has_error())
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
        if (error_reporter().has_error())
        {
            return nullptr;
        }
        operands.push_back(std::move(operand));
    }

    return std::make_unique<SumExpression>(std::move(operands));
}

std::unique_ptr<Expression> ExpressionParser::parse_intersection()
{
    std::unique_ptr<Expression> first = parse_primary();
    if (error_reporter().has_error())
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
        if (error_reporter().has_error())
        {
            return nullptr;
        }
        operands.push_back(std::move(operand));
    }

    return std::make_unique<IntersectionExpression>(std::move(operands));
}

std::unique_ptr<Expression> ExpressionParser::parse_primary()
{
    skip_whitespace();
    if (at_end())
    {
        report_error("Unexpected end of expression");
        return nullptr;
    }

    const char c = peek();
    if (c == '(')
    {
        next();
        std::unique_ptr<Expression> inner = parse_expression();
        if (error_reporter().has_error())
        {
            return nullptr;
        }
        skip_whitespace();
        expect(')');
        if (error_reporter().has_error())
        {
            return nullptr;
        }
        return inner;
    }
    if (c == '{')
    {
        data_t value = parse_fixed_set();
        if (error_reporter().has_error())
        {
            return nullptr;
        }
        return std::make_unique<ConstantExpression>(std::move(value));
    }

    const std::string name = parse_identifier();
    if (error_reporter().has_error())
    {
        return nullptr;
    }
    VariableExpression *variable = find_or_create(name);
    return std::make_unique<ExpressionProxy>(variable);
}

data_t ExpressionParser::parse_fixed_set()
{
    expect('{');
    if (error_reporter().has_error())
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
        if (error_reporter().has_error())
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
        if (error_reporter().has_error())
        {
            return {};
        }
    }
    return result;
}

VariableExpression *ExpressionParser::find_or_create(const std::string &name)
{
    if (auto *existing = m_variables.find(name))
    {
        return existing;
    }
    auto owned = std::make_unique<VariableExpression>();
    VariableExpression *raw = owned.get();
    m_created.push_back(std::move(owned));
    m_variables.register_variable(name, raw);
    return raw;
}
