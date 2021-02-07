/*
 * Let us have a simple arithmetical language, consisting of:
 *
 *  1) Constants of double;
 *  2) Expressions: either expr1 (+ | - | * | /) expr2 or a constant.
 *
 *
 * A sum type can be used to represent the hierarchical structure of ASTs, and pattern matching is a
 * technique to evaluate them.
 */

#include <datatype99.h>

#include <stdio.h>

// clang-format off
datatype(
    Expr,
    (Const, double),
    (Add, Expr *, Expr *),
    (Sub, Expr *, Expr *),
    (Mul, Expr *, Expr *),
    (Div, Expr *, Expr *)
);
// clang-format on

double eval(const Expr *expr) {
    match(*expr) {
        of(Const, number) {
            return *number;
        }
        of(Add, lhs, rhs) {
            return eval(*lhs) + eval(*rhs);
        }
        of(Sub, lhs, rhs) {
            return eval(*lhs) - eval(*rhs);
        }
        of(Mul, lhs, rhs) {
            return eval(*lhs) * eval(*rhs);
        }
        of(Div, lhs, rhs) {
            return eval(*lhs) / eval(*rhs);
        }
    }
}

#define EXPR(expr)       ((Expr *)(Expr[]){expr})
#define OP(op, lhs, rhs) op(EXPR(lhs), EXPR(rhs))

int main(void) {
    Expr expr = OP(Add, Const(53), OP(Sub, OP(Div, Const(155), Const(5)), Const(113)));

    /*
     * Output:
     * -29.000000
     */
    printf("%f\n", eval(&expr));
}
