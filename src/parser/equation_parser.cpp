#include "CU/parser/equation_parser.hpp"

#include "CU/expression/expression.hpp"

#include <utility>

EquationParser::EquationParser(VariableDictionary &variables)
    : m_variables(variables)
    , m_expression_parser(m_variables)
{
}

EquationParser::Result EquationParser::parse_equation(const std::string &input)
{
    Result result;
    set_input(input);

    const std::string name = parse_identifier();
    if (has_error())
    {
        return result;
    }

    expect('=');
    if (has_error())
    {
        return result;
    }

    skip_whitespace();
    const std::size_t expr_start = m_pos;
    const std::string expression_text = m_input.substr(expr_start);

    std::unique_ptr<Expression> root = m_expression_parser.parse(expression_text);
    if (root == nullptr)
    {
        result.created_variables = m_expression_parser.take_created_variables();
        m_pos = expr_start + m_expression_parser.end_position();
        const ErrorReporter &expression_error = m_expression_parser.error_reporter();
        report_error(expression_error.message());
        for (const auto &entry : expression_error.all_metadata())
        {
            m_error.set_metadata(entry.first, entry.second);
        }
        return result;
    }

    result.created_variables = m_expression_parser.take_created_variables();

    m_pos = expr_start + m_expression_parser.end_position();

    skip_whitespace();
    if (!at_end())
    {
        report_error("Unexpected trailing input: '" + m_input.substr(m_pos) + "'");
        return result;
    }

    VariableExpression *variable = m_variables.find(name);
    if (variable == nullptr)
    {
        auto owned = std::make_unique<VariableExpression>();
        variable = owned.get();
        result.created_variables.push_back(std::move(owned));
        m_variables.register_variable(name, variable);
    }

    auto equation = std::make_unique<Equation>(std::move(root));
    equation->set_variable(variable);
    result.equation = std::move(equation);
    return result;
}

VariableDictionary &EquationParser::variables()
{
    return m_variables;
}

const VariableDictionary &EquationParser::variables() const
{
    return m_variables;
}

bool EquationParser::has_error() const
{
    return error_reporter().has_error();
}

const std::string &EquationParser::error_message() const
{
    return error_reporter().message();
}

std::size_t EquationParser::error_position() const
{
    return m_error_position;
}