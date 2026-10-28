#include "CU/regular/regular_set_operations.hpp"

#include <algorithm>
#include <utility>

// ---------------------------------------------------------------------------
// Integer helpers
// ---------------------------------------------------------------------------

num_t RegularSetOperations::gcd(num_t a, num_t b) const
{
    while (b != 0)
    {
        num_t t = a % b;
        a = b;
        b = t;
    }
    return a;
}

num_t RegularSetOperations::lcm(num_t a, num_t b) const
{
    if (a == 0 || b == 0)
    {
        return 0;
    }
    return a / gcd(a, b) * b;
}

num_t RegularSetOperations::common_period(num_t a, num_t b) const
{
    num_t p = (a == 0) ? b : (b == 0) ? a : lcm(a, b);
    return p == 0 ? 1 : p;
}

// ---------------------------------------------------------------------------
// Conversion to/from the residue representation
// ---------------------------------------------------------------------------

std::vector<RegularSetOperations::ResidueSet>
RegularSetOperations::to_residue(const RegularSetData &data, num_t D) const
{
    std::vector<ResidueSet> result(D);

    for (num_t x : data.finite)
    {
        result[x % D].finite.insert(x);
    }

    if (data.period != 0)
    {
        const num_t m = D / data.period;
        for (num_t p : data.periodic)
        {
            for (num_t i = 0; i < m; ++i)
            {
                const num_t base = p + i * data.period;
                const num_t r = base % D;
                if (!result[r].has_base || base < result[r].base)
                {
                    result[r].has_base = true;
                    result[r].base = base;
                }
            }
        }
    }

    for (auto &rs : result)
    {
        if (rs.has_base)
        {
            for (auto it = rs.finite.begin(); it != rs.finite.end();)
            {
                if (*it >= rs.base)
                {
                    it = rs.finite.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }
    }

    return result;
}

RegularSetData
RegularSetOperations::from_residue(const std::vector<ResidueSet> &residues,
                                   num_t D) const
{
    RegularSetData result;

    for (num_t r = 0; r < D; ++r)
    {
        for (num_t x : residues[r].finite)
        {
            result.finite.insert(x);
        }
        if (residues[r].has_base)
        {
            result.periodic.insert(residues[r].base);
        }
    }

    if (!result.periodic.empty())
    {
        result.period = D;
    }

    return result;
}

// ---------------------------------------------------------------------------
// Per-residue set operations
// ---------------------------------------------------------------------------

RegularSetOperations::ResidueSet
RegularSetOperations::unite_residue(const ResidueSet &a,
                                    const ResidueSet &b) const
{
    ResidueSet result;
    result.finite = a.finite;
    result.finite.insert(b.finite.begin(), b.finite.end());

    if (a.has_base && b.has_base)
    {
        result.has_base = true;
        result.base = std::min(a.base, b.base);
    }
    else if (a.has_base)
    {
        result.has_base = true;
        result.base = a.base;
    }
    else if (b.has_base)
    {
        result.has_base = true;
        result.base = b.base;
    }

    if (result.has_base)
    {
        for (auto it = result.finite.begin(); it != result.finite.end();)
        {
            if (*it >= result.base)
            {
                it = result.finite.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    return result;
}

RegularSetOperations::ResidueSet
RegularSetOperations::intersect_residue(const ResidueSet &a,
                                        const ResidueSet &b) const
{
    ResidueSet result;

    for (num_t x : a.finite)
    {
        if (b.finite.count(x))
        {
            result.finite.insert(x);
        }
    }

    if (b.has_base)
    {
        for (num_t x : a.finite)
        {
            if (x >= b.base)
            {
                result.finite.insert(x);
            }
        }
    }

    if (a.has_base)
    {
        for (num_t x : b.finite)
        {
            if (x >= a.base)
            {
                result.finite.insert(x);
            }
        }
    }

    if (a.has_base && b.has_base)
    {
        result.has_base = true;
        result.base = std::max(a.base, b.base);
    }

    if (result.has_base)
    {
        for (auto it = result.finite.begin(); it != result.finite.end();)
        {
            if (*it >= result.base)
            {
                it = result.finite.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    return result;
}

RegularSetOperations::ResidueSet
RegularSetOperations::difference_residue(const ResidueSet &a,
                                         const ResidueSet &b,
                                         num_t D) const
{
    ResidueSet result;

    if (!b.has_base)
    {
        for (num_t x : a.finite)
        {
            if (!b.finite.count(x))
            {
                result.finite.insert(x);
            }
        }

        if (a.has_base)
        {
            num_t new_base = a.base;
            while (b.finite.count(new_base))
            {
                new_base += D;
            }

            result.has_base = true;
            result.base = new_base;

            for (auto it = result.finite.begin(); it != result.finite.end();)
            {
                if (*it >= new_base)
                {
                    it = result.finite.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }
    }
    else
    {
        // b covers an infinite tail; a\b is finite per residue.
        for (num_t x : a.finite)
        {
            if (x < b.base && !b.finite.count(x))
            {
                result.finite.insert(x);
            }
        }

        if (a.has_base)
        {
            for (num_t x = a.base; x < b.base; x += D)
            {
                if (!b.finite.count(x))
                {
                    result.finite.insert(x);
                }
            }
        }
    }

    return result;
}

// ---------------------------------------------------------------------------
// Public operations
// ---------------------------------------------------------------------------

RegularSetData RegularSetOperations::unite(const RegularSetData &lhs,
                                           const RegularSetData &rhs)
{
    const num_t D = common_period(lhs.period, rhs.period);
    const auto lhs_res = to_residue(lhs, D);
    const auto rhs_res = to_residue(rhs, D);

    std::vector<ResidueSet> result(D);
    for (num_t r = 0; r < D; ++r)
    {
        result[r] = unite_residue(lhs_res[r], rhs_res[r]);
    }

    return normalize(from_residue(result, D));
}

RegularSetData RegularSetOperations::intersect(const RegularSetData &lhs,
                                               const RegularSetData &rhs)
{
    const num_t D = common_period(lhs.period, rhs.period);
    const auto lhs_res = to_residue(lhs, D);
    const auto rhs_res = to_residue(rhs, D);

    std::vector<ResidueSet> result(D);
    for (num_t r = 0; r < D; ++r)
    {
        result[r] = intersect_residue(lhs_res[r], rhs_res[r]);
    }

    return normalize(from_residue(result, D));
}

RegularSetData RegularSetOperations::difference(const RegularSetData &lhs,
                                                const RegularSetData &rhs)
{
    const num_t D = common_period(lhs.period, rhs.period);
    const auto lhs_res = to_residue(lhs, D);
    const auto rhs_res = to_residue(rhs, D);

    std::vector<ResidueSet> result(D);
    for (num_t r = 0; r < D; ++r)
    {
        result[r] = difference_residue(lhs_res[r], rhs_res[r], D);
    }

    return normalize(from_residue(result, D));
}

RegularSetData RegularSetOperations::sum(const RegularSetData &lhs,
                                         const RegularSetData &rhs)
{
    RegularSetData result;

    // Fast path: both sets are finite.
    if (lhs.periodic.empty() && rhs.periodic.empty())
    {
        for (num_t a : lhs.finite)
        {
            for (num_t b : rhs.finite)
            {
                result.finite.insert(a + b);
            }
        }
        return result;
    }

    const num_t D = common_period(lhs.period, rhs.period);

    struct Progression
    {
        num_t start;
        num_t step; // 0 means a single point
    };
    std::vector<Progression> progressions;

    for (num_t a : lhs.finite)
    {
        for (num_t b : rhs.finite)
        {
            progressions.push_back({a + b, 0});
        }
        for (num_t b : rhs.periodic)
        {
            progressions.push_back({a + b, rhs.period});
        }
    }
    for (num_t a : lhs.periodic)
    {
        for (num_t b : rhs.finite)
        {
            progressions.push_back({a + b, lhs.period});
        }
    }

    if (lhs.period != 0 && rhs.period != 0)
    {
        const num_t g = gcd(lhs.period, rhs.period);
        const num_t a = lhs.period / g;
        const num_t b = rhs.period / g;
        const num_t conductor =
            (a == 1 || b == 1) ? 0 : (a * b - a - b + 1);

        std::vector<bool> representable(conductor, false);
        if (conductor > 0)
        {
            representable[0] = true;
            for (num_t i = 0; i < conductor; ++i)
            {
                if (!representable[i])
                {
                    continue;
                }
                if (i + a < conductor)
                {
                    representable[i + a] = true;
                }
                if (i + b < conductor)
                {
                    representable[i + b] = true;
                }
            }
        }

        for (num_t p : lhs.periodic)
        {
            for (num_t q : rhs.periodic)
            {
                for (num_t k = 0; k < conductor; ++k)
                {
                    if (representable[k])
                    {
                        progressions.push_back({p + q + g * k, 0});
                    }
                }
                progressions.push_back({p + q + g * conductor, g});
            }
        }
    }

    // Expand every progression to the common period D.
    std::vector<bool> has_base(D, false);
    std::vector<num_t> min_base(D, 0);
    data_t finite;

    for (const auto &prog : progressions)
    {
        if (prog.step == 0)
        {
            finite.insert(prog.start);
            continue;
        }
        const num_t m = D / prog.step;
        for (num_t i = 0; i < m; ++i)
        {
            const num_t base = prog.start + i * prog.step;
            const num_t r = base % D;
            if (!has_base[r] || base < min_base[r])
            {
                has_base[r] = true;
                min_base[r] = base;
            }
        }
    }

    for (num_t r = 0; r < D; ++r)
    {
        if (has_base[r])
        {
            result.periodic.insert(min_base[r]);
        }
    }
    if (!result.periodic.empty())
    {
        result.period = D;
    }

    for (num_t x : finite)
    {
        bool covered = false;
        if (result.period != 0)
        {
            for (num_t b : result.periodic)
            {
                if (x >= b && (x - b) % result.period == 0)
                {
                    covered = true;
                    break;
                }
            }
        }
        if (!covered)
        {
            result.finite.insert(x);
        }
    }

    return normalize(result);
}

// ---------------------------------------------------------------------------
// Normalization (private helper)
// ---------------------------------------------------------------------------

RegularSetData
RegularSetOperations::normalize(const RegularSetData &data) const
{
    RegularSetData result = data;

    if (result.period == 0)
    {
        result.periodic.clear();
        return result;
    }

    // Remove periodic bases covered by another base with the same residue.
    data_t new_periodic;
    for (num_t b : result.periodic)
    {
        bool redundant = false;
        for (num_t other : result.periodic)
        {
            if (other == b)
            {
                continue;
            }
            if (b >= other && (b - other) % result.period == 0)
            {
                redundant = true;
                break;
            }
        }
        if (!redundant)
        {
            new_periodic.insert(b);
        }
    }
    result.periodic = std::move(new_periodic);

    // Remove finite elements already generated by the periodic part.
    data_t new_finite;
    for (num_t x : result.finite)
    {
        bool covered = false;
        for (num_t b : result.periodic)
        {
            if (x >= b && (x - b) % result.period == 0)
            {
                covered = true;
                break;
            }
        }
        if (!covered)
        {
            new_finite.insert(x);
        }
    }
    result.finite = std::move(new_finite);

    if (result.periodic.empty())
    {
        result.period = 0;
    }

    return result;
}