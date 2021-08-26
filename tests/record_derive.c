#include <datatype99.h>

#include <metalang99.h>

#include <assert.h>

// `static_assert` is not working on TCC for some reason.
#ifndef __TINYC__

// Common {

#define assertFields(fields, n)                                                                    \
    ML99_TERMS(assertFieldsCount(fields, n), ML99_listMapInPlaceI(v(visitField), v(fields)))

#define assertFieldsCount(fields, n)                                                               \
    ML99_invokeStmt(                                                                               \
        v(static_assert),                                                                          \
        ML99_natEq(ML99_listLen(v(fields)), v(n)),                                                 \
        v("expected " #n " fields"))

#define visitField_IMPL(field, i) ML99_call(assertField_##i, ML99_untuple(v(field)))
#define visitField_ARITY          2

#define assertField(ty, ident, expected_ty, expected_ident)                                        \
    ML99_TERMS(assertIdentsEq(ty, expected_ty), assertIdentsEq(ident, expected_ident))

#define assertIdentsEq(x, y) v(static_assert(ML99_IDENT_EQ(CHECK_, x, y), #x " must be " #y);)
// } (Common)

// TestDerive {

#define DATATYPE99_RECORD_DERIVE_TestDerive_IMPL(name, fields)                                     \
    ML99_TERMS(assertIdentsEq(name, MyType), assertFields(fields, 3))

#define CHECK_MyType_MyType ()

#define CHECK_a_a ()
#define CHECK_b_b ()
#define CHECK_c_c ()

#define CHECK_A_A ()
#define CHECK_B_B ()
#define CHECK_C_C ()

typedef UnitT A;
typedef UnitT B;
typedef UnitT C;

#define assertField_0_IMPL(ty, ident) assertField(ty, ident, A, a)
#define assertField_1_IMPL(ty, ident) assertField(ty, ident, B, b)
#define assertField_2_IMPL(ty, ident) assertField(ty, ident, C, c)

// clang-format off
record(
    derive(TestDerive, dummy),
    MyType,
    (A, a),
    (B, b),
    (C, c)
);
// clang-format on

#undef DATATYPE99_RECORD_DERIVE_TestDerive_IMPL

#undef CHECK_MyType_MyType
#undef CHECK_a_a
#undef CHECK_b_b
#undef CHECK_c_c

#undef CHECK_A_A
#undef CHECK_B_B
#undef CHECK_C_C

#undef assertField_0_IMPL
#undef assertField_1_IMPL
#undef assertField_2_IMPL
// } (TestDerive)

// TestEmptyDerive {

#define DATATYPE99_RECORD_DERIVE_TestEmptyDerive_IMPL(name, fields)                                \
    ML99_TERMS(assertIdentsEq(name, MyEmptyType), assertFields(fields, 1))

#define CHECK_MyEmptyType_MyEmptyType ()
#define CHECK_char_char               ()
#define CHECK_dummy_dummy             ()

#define assertField_0_IMPL(ty, ident) assertField(ty, ident, char, dummy)

record(derive(TestEmptyDerive), MyEmptyType);

#undef DATATYPE99_RECORD_DERIVE_TestEmptyDerive_IMPL

#undef CHECK_MyEmptyType_MyEmptyType
#undef CHECK_char_char
#undef CHECK_dummy_dummy

#undef assertField_0_IMPL
// } (TestEmptyDerive)

#endif // __TINYC__

int main(void) {}
