#include <datatype99.h>

#include <metalang99.h>

#include <assert.h>
#include <stdbool.h>
#include <string.h>

ML99_ASSERT_UNEVAL(DATATYPE99_datatype_ARITY == 1);
ML99_ASSERT_UNEVAL(DATATYPE99_record_ARITY == 1);
ML99_ASSERT_UNEVAL(DATATYPE99_of_ARITY == 1);
ML99_ASSERT_UNEVAL(DATATYPE99_ifLet_ARITY == 3);
ML99_ASSERT_UNEVAL(DATATYPE99_attrIsPresent_ARITY == 1);
ML99_ASSERT_UNEVAL(DATATYPE99_attrValue_ARITY == 1);
ML99_ASSERT_UNEVAL(DATATYPE99_assertAttrIsPresent_ARITY == 1);

// clang-format off
ML99_EVAL(DATATYPE99_datatype(
    v(MyType),
    v((A, const char *)),
    v((B, int, int))
));

ML99_EVAL(DATATYPE99_record(
    v(MyRecord),
    v((int, x)),
    v((int, y))
));

ML99_EVAL(DATATYPE99_record(v(MyEmptyRecord)));
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

    // Record type usage.
    {
        MyRecord r = {.x = 3, .y = 5};
        (void)r;

        MyEmptyRecord empty_r = {.dummy = '\0'};
        (void)empty_r;
    }
}
