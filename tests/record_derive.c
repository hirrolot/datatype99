#include <datatype99.h>

#include <metalang99.h>

#include <assert.h>

// `static_assert` is not working on TCC for some reason.
#ifndef __TINYC__

// TestDerive {
#define DATATYPE99_RECORD_DERIVE_TestDerive_IMPL(name, fields)                                     \
    ML99_TERMS(v(static_assert(CMP_IDENTS(MyType, name), "Must be MyType");), ASSERT_FIELDS(fields))

#define ASSERT_FIELDS(fields)                                                                      \
    ML99_TERMS(                                                                                    \
        ML99_invokeStmt(                                                                           \
            v(static_assert),                                                                      \
            ML99_natEq(ML99_listLen(v(fields)), v(3)),                                             \
            v("not 3 fields")),                                                                    \
        ML99_listMapInPlaceI(v(visitField), v(fields)))

#define visitField_IMPL(field, i) ML99_call(assertField_##i, ML99_untuple(v(field)))
#define visitField_ARITY          2

#define assertField_0_IMPL(ty, ident) assertField(ty, ident, A, a)
#define assertField_1_IMPL(ty, ident) assertField(ty, ident, B, b)
#define assertField_2_IMPL(ty, ident) assertField(ty, ident, C, c)
// } (TestDerive)

// assertField {
#define assertField(ty, ident, expected_ty, expected_ident)                                        \
    ML99_TERMS(                                                                                    \
        v(static_assert(CMP_IDENTS(ty, expected_ty), "Non-equal types");),                         \
        v(static_assert(CMP_IDENTS(ident, expected_ident), "Non-equal field names");))
// } (assertField)

// Defined identifiers {
#define CMP_IDENTS(x, y) ML99_IDENT_EQ(CHECK_, x, y)

#define CHECK_MyType_MyType ()

#define CHECK_a_a ()
#define CHECK_b_b ()
#define CHECK_c_c ()

#define CHECK_A_A ()
#define CHECK_B_B ()
#define CHECK_C_C ()
// }

typedef UnitT A;
typedef UnitT B;
typedef UnitT C;

// clang-format off
record(
    derive(TestDerive, dummy),
    MyType,
    (A, a),
    (B, b),
    (C, c)
);
// clang-format on

#endif // __TINYC__

int main(void) {}
