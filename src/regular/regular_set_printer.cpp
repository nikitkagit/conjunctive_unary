#include "CU/regular/regular_set_printer.hpp"

#include <sstream>

RegularSetPrinter::RegularSetPrinter(const RegularSetData &data)
    : m_data(data)
{
}

std::string RegularSetPrinter::format() const
{
    std::ostringstream out;

    out << '{';
    bool first = true;
    for (const num_t &v : m_data.finite)
    {
        if (!first)
        {
            out << ", ";
        }
        out << v;
        first = false;
    }
    out << '}';

    if (m_data.period != 0 && !m_data.periodic.empty())
    {
        out << " | {";
        first = true;
        for (const num_t &v : m_data.periodic)
        {
            if (!first)
            {
                out << ", ";
            }
            out << v;
            first = false;
        }
        out << "} + [" << m_data.period << ']';
    }

    return out.str();
}

void RegularSetPrinter::write(std::ostream &out) const
{
    out << format();
}