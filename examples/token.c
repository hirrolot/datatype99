#include <datatype99.h>

#include <stdio.h>

// clang-format off
datatype(
    Token,
    (Ident, const char *),
    (Int, int),
    (LParen),
    (RParen),
    (Plus)
);
// clang-format on

void print_token(Token token) {
    match(token) {
        of(Ident, ident) {
            printf("%s", *ident);
        }
        of(Int, x) {
            printf("%d", *x);
        }
        of(LParen) {
            printf("(");
        }
        of(RParen) {
            printf(")");
        }
        of(Plus) {
            printf(" + ");
        }
    }
}

int main(void) {
    Token tokens[] = {
        LParen(),
        Ident("x"),
        Plus(),
        Int(123),
        RParen(),
    };

    /*
     * Output:
     * (x + 123)
     */
    for (size_t i = 0; i < sizeof(tokens) / sizeof(tokens[0]); i++) {
        print_token(tokens[i]);
    }

    puts("");
}
