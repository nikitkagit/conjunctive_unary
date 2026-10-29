#ifndef VARIABLE_DICTIONARY_HPP
#define VARIABLE_DICTIONARY_HPP

#include "CU/common.hpp"

class EquationCalculator;
class VariableExpression;

/**
 * @brief Bidirectional dictionary mapping variable names to variables.
 *
 * Provides forward (name -> variable) and reverse (variable -> name)
 * lookups. New variables are created through the referenced calculator
 * and are owned by it; the dictionary only stores non-owning pointers.
 */
class VariableDictionary
{
public:
    VariableDictionary() = default;

    /**
     * @brief Finds an existing variable by name.
     * @param name Variable name.
     * @return Non-owning pointer to the variable, or nullptr if not found.
     */
    VariableExpression *find(const std::string &name) const;

    /**
     * @brief Registers a variable under the given name.
     *
     * The dictionary does not take ownership. Registering a name that is
     * already present replaces the previous mapping.
     *
     * @param name Variable name.
     * @param variable Non-owning pointer to the variable.
     */
    void register_variable(const std::string &name,
                           VariableExpression *variable);

    /**
     * @brief Returns the name of the given variable.
     * @param variable Variable to look up.
     * @return Name of the variable, or empty string if not registered.
     */
    std::string name_of(const VariableExpression *variable) const;

    /**
     * @brief Returns the const list of all registered variables.
     * @return Const reference to the name-to-variable mapping.
     */
    const std::map<std::string, VariableExpression *> &variables() const;

private:
    std::map<std::string, VariableExpression *> m_by_name;
    std::map<const VariableExpression *, std::string> m_by_variable;
};

#endif // VARIABLE_DICTIONARY_HPP