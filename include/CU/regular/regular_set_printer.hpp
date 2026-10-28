#ifndef CU_REGULAR_SET_PRINTER_HPP
#define CU_REGULAR_SET_PRINTER_HPP

#include "CU/regular/regular_set_data.hpp"

#include <ostream>
#include <string>

/**
 * @brief Formats a regular set in its textual form.
 */
class RegularSetPrinter
{
public:
    explicit RegularSetPrinter(const RegularSetData &data);

    std::string format() const;
    void write(std::ostream &out) const;

private:
    const RegularSetData &m_data;
};

#endif // CU_REGULAR_SET_PRINTER_HPP