#include "CU/equation/equation_system_printer.hpp"

#include "CU/equation/equation.hpp"
#include "CU/equation/equation_calculator.hpp"
#include "CU/parser/variable_dictionary.hpp"
#include "CU/expression/expression_printer.hpp"
#include "CU/expression/variable_expression.hpp"

#include <fstream>
#include <ostream>
#include <sstream>

EquationSystemPrinter::EquationSystemPrinter(
    const VariableDictionary &dictionary,
    const EquationCalculator &calculator)
    : m_dictionary(dictionary)
    , m_calculator(calculator)
{
}

std::string EquationSystemPrinter::format() const
{
    std::ostringstream out;
    for (const auto &equation : m_calculator.equations())
    {
        const VariableExpression *target = equation->variable();
        if (target == nullptr)
        {
            continue;
        }

        const std::string name = m_dictionary.name_of(target);

        ExpressionPrinter printer(
            [this](const VariableExpression *v)
            {
                return m_dictionary.name_of(v);
            });
        equation->expression().accept(printer);

        out << name << " = " << printer.result() << '\n';
    }
    return out.str();
}

void EquationSystemPrinter::write(std::ostream &out) const
{
    out << format();
}

bool EquationSystemPrinter::write_to_file(const std::string &path) const
{
    std::ofstream file(path);
    if (!file)
    {
        return false;
    }
    file << format();
    return static_cast<bool>(file);
}