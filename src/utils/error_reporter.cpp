#include "CU/utils/error_reporter.hpp"

#include <utility>

ErrorReporter::ErrorReporter(std::string message)
    : m_message(std::move(message))
{
}

void ErrorReporter::report(std::string message)
{
    m_message = std::move(message);
}

const std::string& ErrorReporter::message() const noexcept
{
    return m_message;
}

void ErrorReporter::set_metadata(std::string key, std::string value)
{
    m_metadata[std::move(key)] = std::move(value);
}

bool ErrorReporter::has_metadata(std::string_view key) const
{
    return m_metadata.find(std::string(key)) != m_metadata.end();
}

const std::string* ErrorReporter::metadata(std::string_view key) const
{
    auto it = m_metadata.find(std::string(key));
    return it != m_metadata.end() ? &it->second : nullptr;
}

const std::unordered_map<std::string, std::string>&
ErrorReporter::all_metadata() const noexcept
{
    return m_metadata;
}

void ErrorReporter::reset()
{
    m_message.clear();
    m_metadata.clear();
}

bool ErrorReporter::has_error() const noexcept
{
    return !m_message.empty();
}