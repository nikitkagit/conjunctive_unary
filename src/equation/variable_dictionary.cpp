#include "CU/equation/variable_dictionary.hpp"

#include "CU/equation/equation_calculator.hpp"
#include "CU/expression/variable_expression.hpp"

VariableDictionary::VariableDictionary(EquationCalculator &calculator)
    : m_calculator(calculator)
{
}

VariableExpression *VariableDictionary::get_or_create(const std::string &name)
{
    auto it = m_by_name.find(name);
    if (it != m_by_name.end())
    {
        return it->second;
    }

    VariableExpression *variable = m_calculator.add_variable();
    m_by_name.emplace(name, variable);
    m_by_variable.emplace(variable, name);
    return variable;
}

VariableExpression *VariableDictionary::find(const std::string &name) const
{
    auto it = m_by_name.find(name);
    return it != m_by_name.end() ? it->second : nullptr;
}

std::string VariableDictionary::name_of(const VariableExpression *variable) const
{
    auto it = m_by_variable.find(variable);
    return it != m_by_variable.end() ? it->second : std::string{};
}

const std::map<std::string, VariableExpression *> &VariableDictionary::variables() const
{
    return m_by_name;
}