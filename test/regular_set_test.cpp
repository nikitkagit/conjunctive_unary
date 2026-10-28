#include <cassert>
#include <stdexcept>
#include <string>
#include <utility>

#include "CU/regular/regular_set.hpp"
#include "CU/regular/regular_set_data.hpp"
#include "CU/regular/regular_set_printer.hpp"

namespace
{
RegularSetData make_data(data_t finite, data_t periodic, num_t period)
{
    RegularSetData data;
    data.finite = std::move(finite);
    data.periodic = std::move(periodic);
    data.period = period;
    return data;
}

template <typename Fn>
bool throws_runtime_error(Fn &&fn)
{
    try
    {
        fn();
    }
    catch (const std::runtime_error &)
    {
        return true;
    }
    catch (...)
    {
        return false;
    }
    return false;
}
} // namespace

int main()
{
    // --- Default construction is empty ---
    {
        RegularSet set;
        assert(set.data().finite.empty());
        assert(set.data().periodic.empty());
        assert(set.data().period == 0);
    }

    // --- Construction with zero period clears periodic ---
    {
        RegularSet set(make_data({1, 2}, {3, 4}, 0));
        assert((set.data().finite == data_t{1, 2}));
        assert(set.data().periodic.empty());
        assert(set.data().period == 0);
    }

    // --- Construction with period preserves data ---
    {
        RegularSet set(make_data({1}, {2}, 3));
        assert((set.data().finite == data_t{1}));
        assert((set.data().periodic == data_t{2}));
        assert(set.data().period == 3);
    }

    // --- Minimize: finite only ---
    {
        RegularSet set(make_data({1, 2, 3}, {}, 0));
        set.minimize();
        assert((set.data().finite == data_t{1, 2, 3}));
        assert(set.data().periodic.empty());
        assert(set.data().period == 0);
    }

    // --- Minimize: removes redundant periodic bases ---
    {
        RegularSet set(make_data({}, {10, 20}, 10));
        set.minimize();
        assert(set.data().finite.empty());
        assert((set.data().periodic == data_t{10}));
        assert(set.data().period == 10);
    }

    // --- Minimize: keeps distinct residue bases ---
    {
        RegularSet set(make_data({}, {5, 10}, 10));
        set.minimize();
        assert((set.data().periodic == data_t{5, 10}));
        assert(set.data().period == 10);
    }

    // --- Minimize: removes finite covered by periodic ---
    {
        RegularSet set(make_data({7, 10, 15}, {5}, 10));
        set.minimize();
        assert((set.data().finite == data_t{7, 10}));
        assert((set.data().periodic == data_t{5}));
        assert(set.data().period == 10);
    }

    // --- Minimize: clears period when periodic becomes empty ---
    {
        RegularSet set(make_data({1, 2}, {}, 10));
        set.minimize();
        assert((set.data().finite == data_t{1, 2}));
        assert(set.data().periodic.empty());
        assert(set.data().period == 0);
    }

    // --- Contains: finite ---
    {
        RegularSet set(make_data({1, 2, 3}, {}, 0));
        assert(set.contains(1));
        assert(set.contains(2));
        assert(set.contains(3));
        assert(!set.contains(0));
        assert(!set.contains(4));
    }

    // --- Contains: periodic ---
    {
        RegularSet set(make_data({}, {5}, 10));
        assert(set.contains(5));
        assert(set.contains(15));
        assert(set.contains(25));
        assert(!set.contains(10));
        assert(!set.contains(0));
    }

    // --- Contains: finite + periodic ---
    {
        RegularSet set(make_data({3}, {5}, 10));
        assert(set.contains(3));
        assert(set.contains(5));
        assert(set.contains(15));
        assert(!set.contains(4));
    }

    // --- Union of finite sets ---
    {
        RegularSet a(make_data({1, 2}, {}, 0));
        RegularSet b(make_data({2, 3}, {}, 0));
        RegularSet c = a | b;
        assert((c.data().finite == data_t{1, 2, 3}));
        assert(c.data().periodic.empty());
        assert(c.data().period == 0);
    }

    // --- Union of periodic and finite ---
    {
        RegularSet a(make_data({}, {5}, 10));
        RegularSet b(make_data({3, 4}, {}, 0));
        RegularSet c = a | b;
        assert((c.data().finite == data_t{3, 4}));
        assert((c.data().periodic == data_t{5}));
        assert(c.data().period == 10);
    }

    // --- Union: finite element absorbed by periodic ---
    {
        RegularSet a(make_data({}, {5}, 10));
        RegularSet b(make_data({15, 20}, {}, 0));
        RegularSet c = a | b;
        assert((c.data().finite == data_t{20}));
        assert((c.data().periodic == data_t{5}));
        assert(c.data().period == 10);
    }

    // --- Union: periodic sets with different periods ---
    {
        RegularSet a(make_data({}, {0}, 2));
        RegularSet b(make_data({}, {1}, 3));
        RegularSet c = a | b;
        assert(c.data().period == 6);
        assert(c.data().periodic.size() == 4);
        assert(c.contains(0));
        assert(c.contains(1));
        assert(c.contains(2));
        assert(c.contains(4));
        assert(!c.contains(3));
        assert(!c.contains(5));
    }

    // --- Minkowski sum of finite sets ---
    {
        RegularSet a(make_data({1, 2}, {}, 0));
        RegularSet b(make_data({3, 4}, {}, 0));
        RegularSet c = a + b;
        assert((c.data().finite == data_t{4, 5, 6}));
        assert(c.data().periodic.empty());
        assert(c.data().period == 0);
    }

    // --- Minkowski sum: zero is identity ---
    {
        RegularSet a(make_data({0}, {}, 0));
        RegularSet b(make_data({}, {5}, 10));
        RegularSet c = a + b;
        assert(c.data().finite.empty());
        assert((c.data().periodic == data_t{5}));
        assert(c.data().period == 10);
    }

    // --- Minkowski sum of periodic sets (Frobenius) ---
    {
        RegularSet a(make_data({}, {0}, 2));
        RegularSet b(make_data({}, {0}, 3));
        RegularSet c = a + b;
        assert(c.contains(0));
        assert(!c.contains(1));
        for (num_t i = 2; i <= 50; ++i)
        {
            assert(c.contains(i));
        }
    }

    // --- Intersection of finite sets ---
    {
        RegularSet a(make_data({1, 2, 3}, {}, 0));
        RegularSet b(make_data({2, 3, 4}, {}, 0));
        RegularSet c = a & b;
        assert((c.data().finite == data_t{2, 3}));
        assert(c.data().periodic.empty());
        assert(c.data().period == 0);
    }

    // --- Intersection of periodic and finite ---
    {
        RegularSet a(make_data({}, {1}, 2));
        RegularSet b(make_data({1, 2, 3, 4, 5}, {}, 0));
        RegularSet c = a & b;
        assert((c.data().finite == data_t{1, 3, 5}));
        assert(c.data().periodic.empty());
        assert(c.data().period == 0);
    }

    // --- Intersection of periodic sets with different periods ---
    {
        RegularSet a(make_data({}, {1}, 2));
        RegularSet b(make_data({}, {1}, 3));
        RegularSet c = a & b;
        assert((c.data().periodic == data_t{1}));
        assert(c.data().period == 6);
        assert(c.contains(1));
        assert(c.contains(7));
        assert(c.contains(13));
        assert(!c.contains(3));
        assert(!c.contains(5));
    }

    // --- Difference of finite sets ---
    {
        RegularSet a(make_data({1, 2, 3, 4, 5}, {}, 0));
        RegularSet b(make_data({2, 4}, {}, 0));
        RegularSet c = difference(a, b);
        assert((c.data().finite == data_t{1, 3, 5}));
        assert(c.data().periodic.empty());
        assert(c.data().period == 0);
    }

    // --- Difference: periodic minus periodic yields finite ---
    {
        RegularSet a(make_data({}, {1}, 2));
        RegularSet b(make_data({}, {3}, 2));
        RegularSet c = difference(a, b);
        assert((c.data().finite == data_t{1}));
        assert(c.data().periodic.empty());
        assert(c.data().period == 0);
    }

    // --- Difference: periodic minus finite shifts base ---
    {
        RegularSet a(make_data({}, {1}, 2));
        RegularSet b(make_data({1, 3, 5}, {}, 0));
        RegularSet c = difference(a, b);
        assert(c.data().finite.empty());
        assert((c.data().periodic == data_t{7}));
        assert(c.data().period == 2);
    }

    // --- Difference: finite minus periodic ---
    {
        RegularSet a(make_data({1, 2, 3, 4, 5}, {}, 0));
        RegularSet b(make_data({}, {1}, 2));
        RegularSet c = difference(a, b);
        assert((c.data().finite == data_t{2, 4}));
        assert(c.data().periodic.empty());
        assert(c.data().period == 0);
    }

    // --- Printer: finite set ---
    {
        RegularSetData data = make_data({1, 2, 3}, {}, 0);
        RegularSetPrinter printer(data);
        assert(printer.format() == "{1, 2, 3}");
    }

    // --- Printer: periodic set ---
    {
        RegularSetData data = make_data({1, 2}, {3, 4}, 5);
        RegularSetPrinter printer(data);
        assert(printer.format() == "{1, 2} | {3, 4} + [5]");
    }

    // --- Printer: empty set ---
    {
        RegularSetData data = make_data({}, {}, 0);
        RegularSetPrinter printer(data);
        assert(printer.format() == "{}");
    }

    return 0;
}