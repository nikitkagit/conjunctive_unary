#ifndef EXPRESSION_PROXY_HPP
#define EXPRESSION_PROXY_HPP

#include "CU/expression/expression.hpp"

/**
 * @brief Proxy expression holding a non-owning pointer to another expression.
 *
 * Used to break ownership cycles and enable recursive links between
 * expressions (e.g. mutually recursive variables and formulas).
 */
class ExpressionProxy : public Expression
{
public:
    ExpressionProxy() = default;

    /**
     * @brief Constructs a proxy pointing to the given expression.
     * @param target Non-owning pointer to the wrapped expression.
     */
    explicit ExpressionProxy(const Expression *target);

    data_t calculate() const override;

    /**
     * @brief Replaces the wrapped expression.
     * @param target Non-owning pointer to the new expression.
     */
    void set_target(const Expression *target);

    /**
     * @brief Returns the currently wrapped expression.
     * @return Non-owning pointer to the wrapped expression, or nullptr.
     */
    const Expression *target() const;

    void accept(ExpressionVisitor &visitor) const override;

private:
    const Expression *m_target = nullptr;
};

#endif // EXPRESSION_PROXY_HPP