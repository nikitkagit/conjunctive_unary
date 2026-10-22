#ifndef CU_EXPRESSION_EXPRESSION_VISITOR_HPP
#define CU_EXPRESSION_EXPRESSION_VISITOR_HPP

class ConstantExpression;
class VariableExpression;
class ExpressionProxy;
class UnionExpression;
class IntersectionExpression;
class SumExpression;

/**
 * @brief Abstract visitor for the Expression hierarchy.
 */
class ExpressionVisitor
{
public:
    virtual ~ExpressionVisitor() = default;

    virtual void visit(const ConstantExpression &expr) = 0;
    virtual void visit(const VariableExpression &expr) = 0;
    virtual void visit(const ExpressionProxy &expr) = 0;
    virtual void visit(const UnionExpression &expr) = 0;
    virtual void visit(const IntersectionExpression &expr) = 0;
    virtual void visit(const SumExpression &expr) = 0;
};

#endif // CU_EXPRESSION_EXPRESSION_VISITOR_HPP