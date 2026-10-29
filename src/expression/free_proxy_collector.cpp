#include "CU/expression/free_proxy_collector.hpp"

#include "CU/expression/constant_expression.hpp"
#include "CU/expression/expression_proxy.hpp"
#include "CU/expression/intersection_expression.hpp"
#include "CU/expression/sum_expression.hpp"
#include "CU/expression/union_expression.hpp"
#include "CU/expression/variable_expression.hpp"

void FreeProxyCollector::visit(const ConstantExpression &)
{
}

void FreeProxyCollector::visit(const VariableExpression &)
{
}

void FreeProxyCollector::visit(const ExpressionProxy &expr)
{
    if (expr.target() != nullptr)
    {
        m_proxies.push_back(const_cast<ExpressionProxy *>(&expr));
    }
}

void FreeProxyCollector::visit(const UnionExpression &expr)
{
    for (const auto &operand : expr.operands())
    {
        operand->accept(*this);
    }
}

void FreeProxyCollector::visit(const IntersectionExpression &expr)
{
    for (const auto &operand : expr.operands())
    {
        operand->accept(*this);
    }
}

void FreeProxyCollector::visit(const SumExpression &expr)
{
    for (const auto &operand : expr.operands())
    {
        operand->accept(*this);
    }
}

const std::vector<ExpressionProxy *> &FreeProxyCollector::proxies() const noexcept
{
    return m_proxies;
}

void FreeProxyCollector::reset() noexcept
{
    m_proxies.clear();
}