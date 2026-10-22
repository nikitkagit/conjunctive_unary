#include "CU/expression/expression_printer.hpp"

#include "CU/expression/constant_expression.hpp"
#include "CU/expression/expression_proxy.hpp"
#include "CU/expression/intersection_expression.hpp"
#include "CU/expression/sum_expression.hpp"
#include "CU/expression/union_expression.hpp"
#include "CU/expression/variable_expression.hpp"

ExpressionPrinter::ExpressionPrinter(NameResolver name_resolver)
    : m_name_resolver(std::move(name_resolver))
{
}

std::string ExpressionPrinter::result() const
{
    return m_out.str();
}

void ExpressionPrinter::visit(const ConstantExpression &expr)
{
    m_out << '{';
    bool first = true;
    for (const num_t &v : expr.calculate())
    {
        if (!first)
        {
            m_out << ", ";
        }
        m_out << v;
        first = false;
    }
    m_out << '}';
}

void ExpressionPrinter::visit(const VariableExpression &expr)
{
    if (m_name_resolver)
    {
        const std::string name = m_name_resolver(&expr);
        if (!name.empty())
        {
            m_out << name;
            return;
        }
    }
    m_out << "<var>";
}

void ExpressionPrinter::visit(const ExpressionProxy &expr)
{
    const Expression *target = expr.target();
    if (target == nullptr)
    {
        m_out << "<null>";
        return;
    }
    // The proxy is transparent: the enclosing precedence propagates
    // unchanged, so the wrapped expression is parenthesized according to
    // its own precedence.
    target->accept(*this);
}

void ExpressionPrinter::visit(const UnionExpression &expr)
{
    print_nary(expr.operands(), " | ", PRECEDENCE_UNION);
}

void ExpressionPrinter::visit(const IntersectionExpression &expr)
{
    print_nary(expr.operands(), " & ", PRECEDENCE_INTERSECTION);
}

void ExpressionPrinter::visit(const SumExpression &expr)
{
    print_nary(expr.operands(), " + ", PRECEDENCE_SUM);
}

void ExpressionPrinter::print_nary(
    const std::vector<std::unique_ptr<Expression>> &operands,
    const char *separator,
    int own_precedence)
{
    const bool need_parens = own_precedence < m_parent_precedence;
    if (need_parens)
    {
        m_out << '(';
    }

    const int saved_parent = m_parent_precedence;
    m_parent_precedence = own_precedence;
    for (std::size_t i = 0; i < operands.size(); ++i)
    {
        if (i > 0)
        {
            m_out << separator;
        }
        operands[i]->accept(*this);
    }
    m_parent_precedence = saved_parent;

    if (need_parens)
    {
        m_out << ')';
    }
}