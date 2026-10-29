#ifndef ERROR_REPORTER_HPP
#define ERROR_REPORTER_HPP

#include <string>
#include <string_view>
#include <unordered_map>

/**
 * @brief Stores an error message together with arbitrary metadata.
 *
 * Used by parser components to report errors without throwing exceptions.
 */
class ErrorReporter
{
public:
    ErrorReporter() = default;

    /**
     * @brief Constructs an error with an initial message.
     * @param message Human-readable error message.
     */
    explicit ErrorReporter(std::string message);

    /** @brief Sets the error message (implies an error is present). */
    void report(std::string message);

    /** @brief Returns the current error message. */
    const std::string& message() const noexcept;

    /** @brief Adds or replaces a metadata entry. */
    void set_metadata(std::string key, std::string value);

    /** @brief Checks whether a metadata entry exists. */
    bool has_metadata(std::string_view key) const;

    /**
     * @brief Returns a pointer to the metadata value or nullptr if absent.
     *
     * The returned pointer is valid until the entry is modified or removed.
     */
    const std::string* metadata(std::string_view key) const;

    /** @brief Returns all metadata entries. */
    const std::unordered_map<std::string, std::string>& all_metadata() const noexcept;

    /** @brief Clears the message and all metadata. */
    void reset();

    /** @brief Returns true if a non-empty message is stored. */
    bool has_error() const noexcept;

    /** @brief Convenience conversion to bool; equivalent to has_error(). */
    explicit operator bool() const noexcept { return has_error(); }

private:
    std::string m_message;
    std::unordered_map<std::string, std::string> m_metadata;
};

#endif // ERROR_REPORTER_HPP