#ifndef CU_EXPRESSION_PRINTER_HPP
#define CU_EXPRESSION_PRINTER_HPP

#include "CU/common.hpp"

#include "CU/expression/expression_visitor.hpp"

class Expression;
class VariableExpression;

/**
 * @brief Visitor that renders an Expression tree as text.
 *
 * Parentheses are inserted only where operator precedence requires them: a
 * composite child is wrapped in parentheses if and only if its operator
 * precedence is lower than the precedence of the enclosing operator. The
 * top-level expression is printed without surrounding parentheses.
 *
 * Variable names are supplied through a resolver callback; if the resolver
 * returns an empty string (or is not set), the variable is rendered as
 * "<var>".
 */
class ExpressionPrinter : public ExpressionVisitor
{
public:
    using NameResolver = std::function<std::string(const VariableExpression *)>;

    /**
     * @brief Constructs a printer with an optional variable-name resolver.
     * @param name_resolver Callback returning a name for a variable, or an
     *        empty string if the variable is unknown.
     */
    explicit ExpressionPrinter(NameResolver name_resolver = {});

    /// @brief Returns the rendered text.
    std::string result() const;

    void visit(const ConstantExpression &expr) override;
    void visit(const VariableExpression &expr) override;
    void visit(const ExpressionProxy &expr) override;
    void visit(const UnionExpression &expr) override;
    void visit(const IntersectionExpression &expr) override;
    void visit(const SumExpression &expr) override;

private:
    /// @brief Operator precedence of union ("|").
    static constexpr int PRECEDENCE_UNION = 1;
    /// @brief Operator precedence of sum ("+").
    static constexpr int PRECEDENCE_SUM = 2;
    /// @brief Operator precedence of intersection ("&").
    static constexpr int PRECEDENCE_INTERSECTION = 3;

    /**
     * @brief Prints a list of operands separated by @p separator.
     *
     * Wraps the whole group in parentheses when @p own_precedence is lower
     * than the precedence of the enclosing operator. While printing the
     * operands the enclosing precedence is temporarily replaced by
     * @p own_precedence, so that nested groups decide for themselves.
     *
     * @param operands Operands to print.
     * @param separator Text inserted between operands (e.g. " | ").
     * @param own_precedence Precedence of the operator being printed.
     */
    void print_nary(const std::vector<std::unique_ptr<Expression>> &operands,
                    const char *separator,
                    int own_precedence);

    NameResolver m_name_resolver;
    std::ostringstream m_out;
    /// Precedence of the enclosing operator; 0 means "top level".
    int m_parent_precedence = 0;
};

#endif // CU_EXPRESSION_PRINTER_HPP