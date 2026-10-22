#ifndef VARIABLE_DICTIONARY_HPP
#define VARIABLE_DICTIONARY_HPP

#include "common.hpp"

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
    /**
     * @brief Constructs a dictionary bound to the given calculator.
     * @param calculator Calculator that owns the variables.
     */
    explicit VariableDictionary(EquationCalculator &calculator);

    /**
     * @brief Returns the variable with the given name, creating it if needed.
     * @param name Variable name.
     * @return Non-owning pointer to the variable.
     */
    VariableExpression *get_or_create(const std::string &name);

    /**
     * @brief Finds an existing variable by name.
     * @param name Variable name.
     * @return Non-owning pointer to the variable, or nullptr if not found.
     */
    VariableExpression *find(const std::string &name) const;

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
    EquationCalculator &m_calculator;
    std::map<std::string, VariableExpression *> m_by_name;
    std::map<const VariableExpression *, std::string> m_by_variable;
};

#endif // VARIABLE_DICTIONARY_HPP