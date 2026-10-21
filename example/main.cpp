#include <iostream>

#include "CU/intersection_expression.hpp"
#include "CU/equation.hpp"
#include "CU/sum_expression.hpp"
#include "CU/union_expression.hpp"

int main()
{
    // A = {1, 2, 3}, B = {3, 4, 5}
    // Union:        {1, 2, 3, 4, 5}
    // Intersection: {3}
    // Sum:          {4, 5, 6, 7, 8}

    // Simple constant expression producing a fixed set.
    class ConstantExpression : public Expression
    {
    public:
        explicit ConstantExpression(data_t value) : value_(std::move(value)) {}

        data_t calculate() const override { return value_; }

    private:
        data_t value_;
    };

    const ConstantExpression a({1, 2, 3});
    const ConstantExpression b({3, 4, 5});

    UnionExpression uni({&a, &b});
    IntersectionExpression inter({&a, &b});
    SumExpression sum({&a, &b});

    auto print = [](const data_t &s)
    {
        std::cout << "{ ";
        for (const num_t &v : s)
        {
            std::cout << v << ' ';
        }
        std::cout << "}\n";
    };

    std::cout << "Union:        ";
    print(uni.calculate());
    std::cout << "Intersection: ";
    print(inter.calculate());
    std::cout << "Sum:          ";
    print(sum.calculate());

    Equation equation(std::make_unique<UnionExpression>(
        std::vector<const Expression *>{&a, &b}));
    equation.calculate();
    std::cout << "Equation:     ";
    print(equation.data());

    return 0;
}