// include/CU/result_printer.hpp
#ifndef RESULT_PRINTER_HPP
#define RESULT_PRINTER_HPP

#include "common.hpp"

#include <iosfwd>
#include <string>

class VariableDictionary;

/**
 * @brief Formats and outputs the final variable values.
 *
 * Renders every variable in the dictionary as @c "NAME = { v1 v2 ... }".
 * The result can be written to an arbitrary output stream and to a file.
 */
class ResultPrinter
{
public:
    /**
     * @brief Constructs a printer bound to the given dictionary.
     * @param dictionary Dictionary whose variables are printed.
     */
    explicit ResultPrinter(const VariableDictionary &dictionary);

    /**
     * @brief Renders the current values of all variables.
     * @return Formatted text, one variable per line.
     */
    std::string format() const;

    /**
     * @brief Writes the formatted result to the given stream.
     * @param out Destination stream.
     */
    void write(std::ostream &out) const;

    /**
     * @brief Writes the formatted result to the given file.
     * @param path Path of the output file.
     * @return True on success, false if the file could not be opened or
     *         writing failed.
     */
    bool write_to_file(const std::string &path) const;

private:
    const VariableDictionary &m_dictionary;
};

#endif // RESULT_PRINTER_HPP