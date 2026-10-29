#ifndef FREE_PROXY_COLLECTOR_HPP
#define FREE_PROXY_COLLECTOR_HPP

#include "CU/expression/expression_visitor.hpp"

#include <vector>

class ExpressionProxy;

/**
 * @brief Visitor that collects all @ref ExpressionProxy nodes in an
 *        expression tree.
 *
 * Proxies are collected in the order they are encountered by a depth-first
 * traversal of the composite operands. The target expression of a proxy is
 * not traversed: only the proxies that are structurally part of the visited
 * tree are reported.
 */
class FreeProxyCollector : public ExpressionVisitor
{
public:
    void visit(const ConstantExpression &expr) override;
    void visit(const VariableExpression &expr) override;
    void visit(const ExpressionProxy &expr) override;
    void visit(const UnionExpression &expr) override;
    void visit(const IntersectionExpression &expr) override;
    void visit(const SumExpression &expr) override;

    /// @brief Returns the collected proxies in traversal order.
    const std::vector<ExpressionProxy *> &proxies() const noexcept;

    /// @brief Clears the collected proxies.
    void reset() noexcept;

private:
    std::vector<ExpressionProxy *> m_proxies;
};

#endif // FREE_PROXY_COLLECTOR_HPP