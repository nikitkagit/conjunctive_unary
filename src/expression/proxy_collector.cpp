#include "CU/expression/proxy_collector.hpp"

#include "CU/expression/constant_expression.hpp"
#include "CU/expression/expression_proxy.hpp"
#include "CU/expression/intersection_expression.hpp"
#include "CU/expression/sum_expression.hpp"
#include "CU/expression/union_expression.hpp"
#include "CU/expression/variable_expression.hpp"

void ProxyCollector::visit(const ConstantExpression &)
{
}

void ProxyCollector::visit(const VariableExpression &)
{
}

void ProxyCollector::visit(const ExpressionProxy &expr)
{
    m_proxies.push_back(const_cast<ExpressionProxy *>(&expr));
}

void ProxyCollector::visit(const UnionExpression &expr)
{
    for (const auto &operand : expr.operands())
    {
        operand->accept(*this);
    }
}

void ProxyCollector::visit(const IntersectionExpression &expr)
{
    for (const auto &operand : expr.operands())
    {
        operand->accept(*this);
    }
}

void ProxyCollector::visit(const SumExpression &expr)
{
    for (const auto &operand : expr.operands())
    {
        operand->accept(*this);
    }
}

const std::vector<ExpressionProxy *> &ProxyCollector::proxies() const noexcept
{
    return m_proxies;
}

void ProxyCollector::reset() noexcept
{
    m_proxies.clear();
}