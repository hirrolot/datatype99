#include <datatype99.h>

#include <stdio.h>

// Deriver implementation {
#define DATATYPE99_DERIVE_Print_IMPL(name, variants)                                               \
    ML99_prefixedBlock(                                                                            \
        v(inline static void name##_print(name self, FILE *stream)),                               \
        ML99_prefixedBlock(                                                                        \
            v(match(self)),                                                                        \
            ML99_listMapInPlace(ML99_compose(v(genArm), v(ML99_untuple)), v(variants))))

#define genArm_IMPL(tag, sig)                                                                      \
    ML99_TERMS(                                                                                    \
        DATATYPE99_assertAttrIsPresent(v(tag##_Print_fmt)),                                        \
        ML99_prefixedBlock(                                                                        \
            DATATYPE99_of(v(tag), ML99_indexedArgs(ML99_listLen(v(sig)))),                         \
            ML99_invokeStmt(v(fprintf), v(stream), DATATYPE99_attrValue(v(tag##_Print_fmt)))))
#define genArm_ARITY 1
// (Deriver implementation)

#define Foo_Print_fmt attr("Foo(\"%s\")", *_0)
#define Bar_Print_fmt attr("Bar(%d, %d)", *_0, *_1)

// clang-format off
datatype(
    derive(Print),
    MyType,
    (Foo, const char *),
    (Bar, int, int)
);
// clang-format on

#undef Foo_Print_fmt
#undef Bar_Print_fmt

/*
 * Output:
 *
 * Foo("hello world")
 * Bar(3, 5)
 */
int main(void) {
    MyType_print(Foo("hello world"), stdout);
    puts("");

    MyType_print(Bar(3, 5), stdout);
    puts("");

    return 0;
}
