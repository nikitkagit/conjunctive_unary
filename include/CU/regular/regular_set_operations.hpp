#ifndef CU_REGULAR_SET_OPERATIONS_HPP
#define CU_REGULAR_SET_OPERATIONS_HPP

#include "CU/regular/regular_set_data.hpp"

#include <vector>

/**
 * @brief Algorithms operating on @ref RegularSetData.
 */
class RegularSetOperations
{
public:
    RegularSetOperations() = default;

    /// @brief Union of two regular sets.
    RegularSetData unite(const RegularSetData &lhs, const RegularSetData &rhs);

    /// @brief Minkowski sum (element-wise addition).
    RegularSetData sum(const RegularSetData &lhs, const RegularSetData &rhs);

    /// @brief Intersection of two regular sets.
    RegularSetData intersect(const RegularSetData &lhs,
                             const RegularSetData &rhs);

    /// @brief Set-theoretic difference @c lhs\\rhs.
    RegularSetData difference(const RegularSetData &lhs,
                              const RegularSetData &rhs);

private:
    /// Internal per-residue representation used by binary operations.
    struct ResidueSet
    {
        data_t finite;
        bool has_base = false;
        num_t base = 0;
    };

    num_t gcd(num_t a, num_t b) const;
    num_t lcm(num_t a, num_t b) const;

    /// Least common multiple of the two periods (>= 1).
    num_t common_period(num_t a, num_t b) const;

    /// Expands @p data to the residue representation with period @p D.
    std::vector<ResidueSet> to_residue(const RegularSetData &data,
                                       num_t D) const;

    /// Folds the residue representation back to a @ref RegularSetData.
    RegularSetData from_residue(const std::vector<ResidueSet> &residues,
                                num_t D) const;

    ResidueSet unite_residue(const ResidueSet &a, const ResidueSet &b) const;
    ResidueSet intersect_residue(const ResidueSet &a,
                                 const ResidueSet &b) const;
    ResidueSet difference_residue(const ResidueSet &a,
                                  const ResidueSet &b,
                                  num_t D) const;

    /// Normalizes the representation (private helper for binary operations).
    RegularSetData normalize(const RegularSetData &data) const;
};

#endif // CU_REGULAR_SET_OPERATIONS_HPP