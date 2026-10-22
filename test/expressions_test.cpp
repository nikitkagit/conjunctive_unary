#include <cassert>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "expression/constant_expression.hpp"
#include "expression/expression_proxy.hpp"
#include "expression/intersection_expression.hpp"
#include "expression/sum_expression.hpp"
#include "expression/union_expression.hpp"
#include "expression/variable_expression.hpp"

namespace
{
std::unique_ptr<Expression> make_const(data_t value)
{
    return std::make_unique<ConstantExpression>(std::move(value));
}

std::vector<std::unique_ptr<Expression>> make_operands(
    std::initializer_list<data_t> values)
{
    std::vector<std::unique_ptr<Expression>> result;
    for (const data_t &v : values)
    {
        result.push_back(make_const(v));
    }
    return result;
}
} // namespace

int main()
{
    // ConstantExpression
    {
        ConstantExpression c({1, 2, 3});
        assert((c.calculate() == data_t{1, 2, 3}));

        ConstantExpression empty({});
        assert(empty.calculate().empty());
    }

    // UnionExpression
    {
        UnionExpression disjoint(make_operands({{1, 2}, {3, 4}}));
        assert((disjoint.calculate() == data_t{1, 2, 3, 4}));

        UnionExpression overlapping(make_operands({{1, 2, 3}, {3, 4, 5}}));
        assert((overlapping.calculate() == data_t{1, 2, 3, 4, 5}));

        UnionExpression single(make_operands({{7, 8, 9}}));
        assert((single.calculate() == data_t{7, 8, 9}));

        UnionExpression no_operands({});
        assert(no_operands.calculate().empty());

        UnionExpression with_empty(make_operands({{}, {1, 2}}));
        assert((with_empty.calculate() == data_t{1, 2}));

        UnionExpression three(make_operands({{1}, {2}, {3}}));
        assert((three.calculate() == data_t{1, 2, 3}));
    }

    // IntersectionExpression
    {
        IntersectionExpression overlapping(make_operands({{1, 2, 3}, {2, 3, 4}}));
        assert((overlapping.calculate() == data_t{2, 3}));

        IntersectionExpression disjoint(make_operands({{1, 2}, {3, 4}}));
        assert(disjoint.calculate().empty());

        IntersectionExpression single(make_operands({{5, 6}}));
        assert((single.calculate() == data_t{5, 6}));

        IntersectionExpression no_operands({});
        assert(no_operands.calculate().empty());

        IntersectionExpression with_empty(make_operands({{1, 2}, {}}));
        assert(with_empty.calculate().empty());

        IntersectionExpression three(
            make_operands({{1, 2, 3, 4}, {2, 3, 4}, {3, 4, 5}}));
        assert((three.calculate() == data_t{3, 4}));
    }

    // SumExpression
    {
        SumExpression basic(make_operands({{1, 2}, {3, 4}}));
        assert((basic.calculate() == data_t{4, 5, 6}));

        SumExpression three(make_operands({{1}, {2}, {3}}));
        assert((three.calculate() == data_t{6}));

        SumExpression no_operands({});
        assert(no_operands.calculate().empty());

        SumExpression with_empty(make_operands({{1, 2}, {}}));
        assert(with_empty.calculate().empty());

        SumExpression duplicate_paths(make_operands({{1, 2}, {1, 2}}));
        assert((duplicate_paths.calculate() == data_t{2, 3, 4}));

        SumExpression single(make_operands({{5, 6}}));
        assert((single.calculate() == data_t{5, 6}));

        SumExpression with_zero(make_operands({{0, 1}, {0, 2}}));
        assert((with_zero.calculate() == data_t{0, 1, 2, 3}));
    }

    // VariableExpression
    {
        VariableExpression default_constructed;
        assert(default_constructed.data().empty());
        assert(default_constructed.calculate().empty());

        VariableExpression with_value({1, 2, 3});
        assert((with_value.data() == data_t{1, 2, 3}));
        assert((with_value.calculate() == data_t{1, 2, 3}));

        VariableExpression mutated;
        mutated.set_data({4, 5});
        assert((mutated.data() == data_t{4, 5}));
        assert((mutated.calculate() == data_t{4, 5}));
    }

    // ExpressionProxy
    {
        ExpressionProxy no_target;
        assert(no_target.calculate().empty());
        assert(no_target.target() == nullptr);

        ConstantExpression constant({1, 2});
        ExpressionProxy with_target(&constant);
        assert((with_target.calculate() == data_t{1, 2}));
        assert(with_target.target() == &constant);

        ConstantExpression other({3, 4});
        no_target.set_target(&other);
        assert((no_target.calculate() == data_t{3, 4}));
        assert(no_target.target() == &other);

        no_target.set_target(nullptr);
        assert(no_target.calculate().empty());
    }

    // Composites own their operands and reflect variable updates
    {
        auto variable = std::make_unique<VariableExpression>(data_t{1, 2});
        VariableExpression *raw = variable.get();

        std::vector<std::unique_ptr<Expression>> operands;
        operands.push_back(std::move(variable));
        UnionExpression union_expr(std::move(operands));
        assert((union_expr.calculate() == data_t{1, 2}));

        raw->set_data({3, 4});
        assert((union_expr.calculate() == data_t{3, 4}));
    }

    // Nested expressions: (A | B) & C
    {
        auto inner = std::make_unique<UnionExpression>(
            make_operands({{1, 2, 3}, {3, 4, 5}}));

        std::vector<std::unique_ptr<Expression>> outer_operands;
        outer_operands.push_back(std::move(inner));
        outer_operands.push_back(make_const({1, 3, 5}));

        IntersectionExpression outer(std::move(outer_operands));
        assert((outer.calculate() == data_t{1, 3, 5}));
    }

    // Proxy references an expression inside a composite
    {
        VariableExpression variable({10, 20});
        ExpressionProxy proxy(&variable);

        std::vector<std::unique_ptr<Expression>> operands;
        operands.push_back(std::make_unique<ExpressionProxy>(proxy));
        operands.push_back(make_const({1, 2}));
        SumExpression sum(std::move(operands));

        assert((sum.calculate() == data_t{11, 12, 21, 22}));

        variable.set_data({100});
        assert((sum.calculate() == data_t{101, 102}));
    }

    std::cout << "All Expression tests passed\n";
    return 0;
}