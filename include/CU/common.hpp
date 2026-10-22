#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstddef>
#include <cctype>
#include <stdexcept>
#include <memory>
#include <set>
#include <map>
#include <utility>
#include <string>
#include <vector>

/// Numeric type used for elements of the sets.
using num_t = unsigned long long;

/// Container type used for set values.
using data_t = std::set<num_t>;

#endif // COMMON_HPP