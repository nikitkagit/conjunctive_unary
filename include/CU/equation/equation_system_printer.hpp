#ifndef CU_APPLICATION_EQUATION_SYSTEM_PRINTER_HPP
#define CU_APPLICATION_EQUATION_SYSTEM_PRINTER_HPP

#include "common.hpp"

class VariableDictionary;
class EquationCalculator;

/**
 * @brief Renders the whole equation system as text.
 *
 * Every equation is printed as "NAME = expression", where NAME comes from
 * the variable dictionary and the expression is rendered by ExpressionPrinter.
 */
class EquationSystemPrinter
{
public:
    EquationSystemPrinter(const VariableDictionary &dictionary,
                          const EquationCalculator &calculator);

    /// @brief Returns the formatted system, one equation per line.
    std::string format() const;

    /// @brief Writes the formatted system to the given stream.
    void write(std::ostream &out) const;

    /// @brief Writes the formatted system to the given file.
    bool write_to_file(const std::string &path) const;

private:
    const VariableDictionary &m_dictionary;
    const EquationCalculator &m_calculator;
};

#endif // CU_APPLICATION_EQUATION_SYSTEM_PRINTER_HPP