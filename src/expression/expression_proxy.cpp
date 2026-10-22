#include "expression/expression_proxy.hpp"
#include "expression/expression_visitor.hpp"

ExpressionProxy::ExpressionProxy(const Expression *target)
    : m_target(target)
{
}

data_t ExpressionProxy::calculate() const
{
    return m_target != nullptr ? m_target->calculate() : data_t{};
}

void ExpressionProxy::set_target(const Expression *target)
{
    m_target = target;
}

const Expression *ExpressionProxy::target() const
{
    return m_target;
}

void ExpressionProxy::accept(ExpressionVisitor &visitor) const
{
    visitor.visit(*this);
}