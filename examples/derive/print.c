#include <datatype99.h>

#include <stdio.h>

#define DATATYPE99_DERIVE_Print_IMPL(name, variants)                                               \
    PRINTER_TEMPLATE(                                                                              \
        v(name),                                                                                   \
        ML99_listMapInPlace(ML99_compose(v(GEN_ARM), v(ML99_untuple)), v(variants)))

#define GEN_ARM_IMPL(tag, sig)                                                                     \
    ML99_TERMS(                                                                                    \
        DATATYPE99_assertAttrIsPresent(v(tag##_Print_fmt)),                                        \
        ARM_TEMPLATE(v(tag), DATATYPE99_of(v(tag), ML99_indexedArgs(ML99_listLen(v(sig))))))
#define GEN_ARM_ARITY 1

#define ARM_TEMPLATE(tag, of)       ML99_call(ARM_TEMPLATE, tag, of)
#define PRINTER_TEMPLATE(name, ...) ML99_call(PRINTER_TEMPLATE, name, __VA_ARGS__)

#define ARM_TEMPLATE_IMPL(tag, of)                                                                 \
    v(of { fprintf(stream, DATATYPE99_ATTR_VALUE(tag##_Print_fmt)); })

// clang-format off
#define PRINTER_TEMPLATE_IMPL(name, ...)                                                            \
    v(inline static void name##_print(name self, FILE *stream) { \
        match(self) { __VA_ARGS__ } \
    })
// clang-format on

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
}
