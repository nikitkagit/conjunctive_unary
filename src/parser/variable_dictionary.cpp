#include "CU/parser/variable_dictionary.hpp"

#include "CU/expression/variable_expression.hpp"

VariableExpression *VariableDictionary::find(const std::string &name) const
{
    auto it = m_by_name.find(name);
    return it != m_by_name.end() ? it->second : nullptr;
}

void VariableDictionary::register_variable(const std::string &name,
                                           VariableExpression *variable)
{
    auto it = m_by_name.find(name);
    if (it != m_by_name.end())
    {
        if (it->second == variable)
        {
            return;
        }
        m_by_variable.erase(it->second);
    }
    m_by_name[name] = variable;
    m_by_variable[variable] = name;
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