#include <datatype99.h>

#include <metalang99.h>

#include <assert.h>

// DATATYPE99_ATTR_IS_PRESENT {
#define FOO attr(~, ~, ~)

ML99_ASSERT_UNEVAL(DATATYPE99_ATTR_IS_PRESENT(FOO));
ML99_ASSERT_UNEVAL(!DATATYPE99_ATTR_IS_PRESENT(BAR));

#undef FOO
// }

// DATATYPE99_ATTR_VALUE {
#define FOO attr(678)

ML99_ASSERT_UNEVAL(DATATYPE99_ATTR_VALUE(FOO) == 678);

#undef FOO
// }

// DATATYPE99_assertAttrIsPresent {
#define FOO attr(~, ~, ~)

ML99_EVAL(DATATYPE99_assertAttrIsPresent(v(FOO)))

#undef FOO
// }

typedef UnitT A;
typedef UnitT B;
typedef UnitT C;

// TestDeriveWithArgs {
#define DATATYPE99_DERIVE_TestDeriveWithArgs_IMPL(name, variants, a, b, c)                         \
    ML99_TERMS(                                                                                    \
        v(static_assert(a == 1 && b == 2 && c == 3, "Invalid a, b, c args");),                     \
        ML99_call(DATATYPE99_DERIVE_TestDerive, v(name, variants)))
// }

// TestDerive {
#define DATATYPE99_DERIVE_TestDerive_IMPL(name, variants)                                          \
    ML99_TERMS(                                                                                    \
        v(static_assert(CMP_IDENTS(MyType, name), "Must be MyType");),                             \
        ASSERT_VARIANTS(variants))

#define ASSERT_VARIANTS(variants)                                                                  \
    ML99_TERMS(                                                                                    \
        ML99_semicoloned(ML99_invoke(                                                              \
            v(static_assert),                                                                      \
            ML99_natEq(ML99_listLen(v(variants)), v(3)),                                           \
            v("not 3 variants"))),                                                                 \
        ML99_listMapInPlaceI(v(visitVariant), v(variants)))

#define visitVariant_IMPL(variant, i) ML99_call(assertVariant_##i, ML99_untuple(v(variant)))
#define visitVariant_ARITY            2

#define assertVariant_0_IMPL(tag, sig) assertVariant(tag, sig, Foo, A)
#define assertVariant_1_IMPL(tag, sig) assertVariant(tag, sig, Bar, B, C)
#define assertVariant_2_IMPL(tag, sig)                                                             \
    ML99_TERMS(                                                                                    \
        v(static_assert(CMP_IDENTS(tag, Baz), "Variant #3 is not Baz");),                          \
        v(static_assert(ML99_IS_NIL(sig), "Baz is not empty");))
// }

// assertVariant {
#define assertVariant(tag, sig, expected_tag, ...)                                                 \
    ML99_TERMS(                                                                                    \
        v(static_assert(CMP_IDENTS(tag, expected_tag), "Non-equal tags");),                        \
        assertSig(sig, __VA_ARGS__))

#define assertSig(sig, ...)                                                                        \
    ML99_TERMS(                                                                                    \
        ML99_semicoloned(ML99_invoke(                                                              \
            v(static_assert),                                                                      \
            ML99_natEq(ML99_listLen(v(sig)), ML99_variadicsCount(v(__VA_ARGS__))),                 \
            v("Invalid variant arity"))),                                                          \
        ML99_semicoloned(ML99_invoke(                                                              \
            v(static_assert),                                                                      \
            ML99_listEq(ML99_appl(v(ML99_identEq), v(CHECK_)), v(sig), ML99_list(v(__VA_ARGS__))), \
            v("Non-equal types"))))
// }

// Defined identifiers {
#define CMP_IDENTS(x, y) ML99_IDENT_EQ(CHECK_, x, y)

#define CHECK_MyType_MyType ()

#define CHECK_Foo_Foo ()
#define CHECK_Bar_Bar ()
#define CHECK_Baz_Baz ()

#define CHECK_A_A ()
#define CHECK_B_B ()
#define CHECK_C_C ()
// }

// clang-format off
datatype(
    derive(TestDerive, (TestDeriveWithArgs, 1, 2, 3)),
    MyType,
    (Foo, A),
    (Bar, B, C),
    (Baz)
);
// clang-format on

int main(void) {}
