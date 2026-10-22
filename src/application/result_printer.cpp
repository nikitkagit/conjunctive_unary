#include "application/result_printer.hpp"

#include "equation/variable_dictionary.hpp"
#include "expression/variable_expression.hpp"

#include <fstream>
#include <ostream>
#include <sstream>

namespace
{
std::string format_value(const data_t &value)
{
    std::ostringstream out;
    out << "{ ";
    for (const num_t &v : value)
    {
        out << v << ' ';
    }
    out << '}';
    return out.str();
}
} // namespace

ResultPrinter::ResultPrinter(const VariableDictionary &dictionary)
    : m_dictionary(dictionary)
{
}

std::string ResultPrinter::format() const
{
    std::ostringstream out;
    for (const auto &entry : m_dictionary.variables())
    {
        out << entry.first << " = " << format_value(entry.second->data())
            << '\n';
    }
    return out.str();
}

void ResultPrinter::write(std::ostream &out) const
{
    out << format();
}

bool ResultPrinter::write_to_file(const std::string &path) const
{
    std::ofstream file(path);
    if (!file)
    {
        return false;
    }
    file << format();
    return static_cast<bool>(file);
}