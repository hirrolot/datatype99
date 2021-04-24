#include <datatype99.h>

#include <assert.h>
#include <stdbool.h>
#include <string.h>

// clang-format off
ML99_EVAL(DATATYPE99_datatype(
    v(MyType),
    v((A, const char *)),
    v((B, int, int))
));
// clang-format on

int main(void) {
    // DATATYPE99_of
    {
        const MyType expr = A("hello world");

        match(expr) {
            ML99_EVAL(DATATYPE99_of(v(A), v(str)))
            { assert(strcmp(*str, "hello world") == 0); }
            ML99_EVAL(DATATYPE99_of(v(B), v(_), v(_)))
            { assert(false); }
            otherwise {
                assert(false);
            }
        }
    }

    // DATATYPE99_ifLet
    {
        const MyType expr = B(5, 7);

        ML99_EVAL(DATATYPE99_ifLet(v(expr), v(B), v(x), v(y)))
        {
            assert(*x == 5);
            assert(*y == 7);
        }
    }
}
