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
        of(Const, number) return *number;
        of(Add, lhs, rhs) return eval(*lhs) + eval(*rhs);
        of(Sub, lhs, rhs) return eval(*lhs) - eval(*rhs);
        of(Mul, lhs, rhs) return eval(*lhs) * eval(*rhs);
        of(Div, lhs, rhs) return eval(*lhs) / eval(*rhs);
    }

    // Invalid input (no such variant).
    return -1;
}

#define EXPR(expr)       ((Expr *)(Expr[]){expr})
#define OP(lhs, op, rhs) op(EXPR(lhs), EXPR(rhs))

int main(void) {
    // 53 + ((155 / 5) - 113)
    Expr expr = OP(Const(53), Add, OP(OP(Const(155), Div, Const(5)), Sub, Const(113)));

    /*
     * Output:
     * -29.000000
     */
    printf("%f\n", eval(&expr));

    return 0;
}
