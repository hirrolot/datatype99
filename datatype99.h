/*
MIT License

Copyright (c) 2020 Temirkhan Myrzamadi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// The official repository: <https://github.com/Hirrolot/datatype99>.

#ifndef DATATYPE99_H
#define DATATYPE99_H

#include <metalang99.h>

#if ML99_MAJOR != 1 && ML99_MINOR < 1
#error "Please, update Metalang99 to v1.1.0 or later"
#endif

#ifndef DATATYPE99_NO_ALIASES

#define datatype  datatype99
#define record    record99
#define match     match99
#define matches   matches99
#define ifLet     ifLet99
#define of        of99
#define otherwise otherwise99

#define UnitT  UnitT99
#define unit_v unit_v99

#endif // DATATYPE99_NO_ALIASES

// Various public stuff {

// Metalang99-compliant `datatype`, `of`, and `ifLet` {

#define DATATYPE99_datatype(...)        ML99_call(DATATYPE99_datatype, __VA_ARGS__)
#define DATATYPE99_record(...)          ML99_call(DATATYPE99_record, __VA_ARGS__)
#define DATATYPE99_of(...)              ML99_call(DATATYPE99_of, __VA_ARGS__)
#define DATATYPE99_ifLet(val, tag, ...) ML99_call(DATATYPE99_ifLet, val, tag, __VA_ARGS__)

#define datatype99(...)        ML99_EVAL(DATATYPE99_datatype_IMPL(__VA_ARGS__))
#define record99(...)          ML99_EVAL(DATATYPE99_record_IMPL(__VA_ARGS__))
#define of99(...)              ML99_EVAL(DATATYPE99_of_IMPL(__VA_ARGS__))
#define ifLet99(val, tag, ...) ML99_EVAL(DATATYPE99_ifLet_IMPL(val, tag, __VA_ARGS__))
// }

// Attributes manipulation {

#define DATATYPE99_attrIsPresent(attr)       ML99_call(DATATYPE99_attrIsPresent, attr)
#define DATATYPE99_attrValue(attr)           ML99_call(DATATYPE99_attrValue, attr)
#define DATATYPE99_assertAttrIsPresent(attr) ML99_call(DATATYPE99_assertAttrIsPresent, attr)

#define DATATYPE99_attrIsPresent_IMPL(attr) v(DATATYPE99_ATTR_IS_PRESENT(attr))
#define DATATYPE99_attrValue_IMPL(attr)     v(DATATYPE99_ATTR_VALUE(attr))
#define DATATYPE99_assertAttrIsPresent_IMPL(attr)                                                  \
    ML99_IF(                                                                                       \
        DATATYPE99_ATTR_IS_PRESENT(attr),                                                          \
        ML99_empty(),                                                                              \
        ML99_fatal(DATATYPE99_assertAttrIsPresent, attr must be defined))

#define DATATYPE99_ATTR_IS_PRESENT(attr)                                                           \
    ML99_IS_TUPLE(ML99_CAT(DATATYPE99_PRIV_ATTR_IS_PRESENT_, attr))
#define DATATYPE99_PRIV_ATTR_IS_PRESENT_attr(...) ()

#define DATATYPE99_ATTR_VALUE(attr)          ML99_CAT(DATATYPE99_PRIV_ATTR_VALUE_, attr)
#define DATATYPE99_PRIV_ATTR_VALUE_attr(...) __VA_ARGS__
// }

#define DATATYPE99_DERIVE_dummy_IMPL(...)        ML99_empty()
#define DATATYPE99_RECORD_DERIVE_dummy_IMPL(...) ML99_empty()

#define DATATYPE99_MAJOR 1
#define DATATYPE99_MINOR 1
#define DATATYPE99_PATCH 0

// } (Various public stuff)

// Unit type {

typedef char UnitT99;
static const UnitT99 unit_v99 = '\0';
// }

// Sum type generation {

#define DATATYPE99_datatype_IMPL(x, ...)                                                           \
    ML99_TERMS(                                                                                    \
        ML99_CAT(DATATYPE99_PRIV_WITH_DERIVE_, DATATYPE99_PRIV_IS_DERIVE(x))(x, __VA_ARGS__),      \
        v(ML99_TRAILING_SEMICOLON()))

#define DATATYPE99_PRIV_WITH_DERIVE_0(name, ...)                                                   \
    DATATYPE99_PRIV_WITH_DERIVE_1(derive(dummy), name, __VA_ARGS__)

#define DATATYPE99_PRIV_WITH_DERIVE_1(derivers, name, ...)                                         \
    ML99_call(                                                                                     \
        DATATYPE99_PRIV_genDatatype,                                                               \
        v(name),                                                                                   \
        DATATYPE99_PRIV_parseVariants(__VA_ARGS__),                                                \
        v(DATATYPE99_PRIV_ELIM_##derivers))

#define DATATYPE99_PRIV_genDatatype_IMPL(name, variants, ...)                                      \
    ML99_TERMS(                                                                                    \
        v(typedef struct name name;),                                                              \
        DATATYPE99_PRIV_genVariantTypedefsForEach(name, variants),                                 \
        ML99_typedef(                                                                              \
            v(name##Tag),                                                                          \
            ML99_enum(v(name##Tag), DATATYPE99_PRIV_genTagForEach(variants))),                     \
        ML99_typedef(                                                                              \
            v(name##Variants),                                                                     \
            ML99_union(v(name##Variants), DATATYPE99_PRIV_genUnionFieldForEach(name, variants))),  \
        v(struct name {                                                                            \
            name##Tag tag;                                                                         \
            name##Variants data;                                                                   \
        };),                                                                                       \
        DATATYPE99_PRIV_genCtorForEach(name, variants),                                            \
        DATATYPE99_PRIV_invokeDeriverForEach(DATATYPE99_DERIVE_, name, variants, __VA_ARGS__))
// } (Sum type generation)

// Record type generation {
#define DATATYPE99_record_IMPL(x, ...)                                                             \
    ML99_TERMS(                                                                                    \
        ML99_CAT(DATATYPE99_PRIV_RECORD_WITH_DERIVE_, DATATYPE99_PRIV_IS_DERIVE(x))(               \
            x,                                                                                     \
            __VA_ARGS__),                                                                          \
        v(ML99_TRAILING_SEMICOLON()))

#define DATATYPE99_PRIV_RECORD_WITH_DERIVE_0(name, ...)                                            \
    DATATYPE99_PRIV_RECORD_WITH_DERIVE_1(derive(dummy), name, __VA_ARGS__)

#define DATATYPE99_PRIV_RECORD_WITH_DERIVE_1(derivers, name, ...)                                  \
    ML99_call(                                                                                     \
        DATATYPE99_PRIV_genRecord,                                                                 \
        v(name),                                                                                   \
        DATATYPE99_PRIV_parseFields(__VA_ARGS__),                                                  \
        v(DATATYPE99_PRIV_ELIM_##derivers))

#define DATATYPE99_PRIV_genRecord_IMPL(name, fields, ...)                                          \
    ML99_TERMS(                                                                                    \
        ML99_typedef(                                                                              \
            v(name),                                                                               \
            ML99_struct(v(name), DATATYPE99_PRIV_genRecordFieldForEach(fields))),                  \
        DATATYPE99_PRIV_invokeDeriverForEach(                                                      \
            DATATYPE99_RECORD_DERIVE_,                                                             \
            name,                                                                                  \
            fields,                                                                                \
            __VA_ARGS__))
// } (Record type generation)

// Parse variants {
#define DATATYPE99_PRIV_parseVariants(...)                                                         \
    ML99_listFromTuples(v(DATATYPE99_PRIV_parseVariant), v(__VA_ARGS__))

#define DATATYPE99_PRIV_parseVariant_IMPL(...)                                                     \
    ML99_CAT(DATATYPE99_PRIV_parseVariantIsEmpty_, ML99_VARIADICS_IS_SINGLE(__VA_ARGS__))          \
    (__VA_ARGS__)

#define DATATYPE99_PRIV_parseVariantIsEmpty_1(tag) DATATYPE99_PRIV_variant(v(tag), ML99_nil())
#define DATATYPE99_PRIV_parseVariantIsEmpty_0(tag, ...)                                            \
    DATATYPE99_PRIV_variant(v(tag), ML99_list(v(__VA_ARGS__)))
// } (Parse variants)

// Parse fields {
#define DATATYPE99_PRIV_parseFields(...)                                                           \
    ML99_listFromTuples(v(DATATYPE99_PRIV_parseField), v(__VA_ARGS__))

#define DATATYPE99_PRIV_parseField_IMPL(ty, ident) v((ty, ident))
// } (Parse fields)

// Variant {
#define DATATYPE99_PRIV_variant(tag, sig) ML99_tuple(tag, sig)

#define DATATYPE99_PRIV_forEachVariant(f, variants)                                                \
    ML99_listMapInPlace(ML99_compose(f, v(ML99_untuple)), variants)
// } (Variant)

// Derivation {
#define DATATYPE99_PRIV_IS_DERIVE(x)          ML99_IS_TUPLE(DATATYPE99_PRIV_IS_DERIVE_##x)
#define DATATYPE99_PRIV_IS_DERIVE_derive(...) ()

#define DATATYPE99_PRIV_ELIM_derive(...) __VA_ARGS__

#define DATATYPE99_PRIV_invokeDeriverForEach(prefix, name, repr, ...)                              \
    ML99_variadicsForEach(                                                                         \
        ML99_appl(v(DATATYPE99_PRIV_invokeDeriver), v(prefix, name, repr)),                        \
        v(__VA_ARGS__))

#define DATATYPE99_PRIV_invokeDeriver_IMPL(prefix, name, repr, deriver)                            \
    ML99_callUneval(prefix##deriver, name, repr)
// } (Derivation)

// Pattern matching {

// clang-format off

#define match99(val) \
    ML99_CLANG_PRAGMA("clang diagnostic push") \
    ML99_CLANG_PRAGMA("clang diagnostic ignored \"-Wcast-qual\"") \
    ML99_INTRODUCE_NON_NULL_PTR_TO_STMT(void, datatype99_priv_matched_val, (void *)&(val)) \
    ML99_CLANG_PRAGMA("clang diagnostic pop") \
        switch ((val).tag)

#define DATATYPE99_of_IMPL(...) \
    ML99_TERMS( \
        v(break; case ML99_CAT(ML99_VARIADICS_GET(0)(__VA_ARGS__), Tag):),                                                                                      \
        ML99_IF( \
            ML99_VARIADICS_IS_SINGLE(__VA_ARGS__), \
            ML99_empty(), \
            DATATYPE99_PRIV_genBindingForEach(v(__VA_ARGS__))))
// clang-format on

#define otherwise99                                                                                \
    break;                                                                                         \
    default:

// clang-format off
#define DATATYPE99_ifLet_IMPL(val, tag_, ...) \
    ML99_TERMS( \
        v(if (tag_##Tag == (val).tag) \
            ML99_CLANG_PRAGMA("clang diagnostic push") \
            ML99_CLANG_PRAGMA("clang diagnostic ignored \"-Wcast-qual\"") \
            ML99_INTRODUCE_NON_NULL_PTR_TO_STMT(void, datatype99_priv_matched_val, (void *)&(val)) \
            ML99_CLANG_PRAGMA("clang diagnostic pop")), \
        DATATYPE99_PRIV_genBindingForEach(v(tag_, __VA_ARGS__)))
// clang-format on

#define matches99(val, tag_) ((val).tag == tag_##Tag)

#define DATATYPE99_PRIV_genBindingForEach(...)                                                     \
    ML99_call(DATATYPE99_PRIV_genBindingForEach, __VA_ARGS__)
#define DATATYPE99_PRIV_genBindingForEach_IMPL(tag, ...)                                           \
    ML99_variadicsForEachI(ML99_appl(v(DATATYPE99_PRIV_genBinding), v(tag)), v(__VA_ARGS__))

#define DATATYPE99_PRIV_genBinding_IMPL(tag_, x, i)                                                \
    ML99_IF(                                                                                       \
        ML99_DETECT_IDENT(ML99_UNDERSCORE_DETECTOR, x),                                            \
        ML99_empty(),                                                                              \
        v(ML99_INTRODUCE_VAR_TO_STMT(                                                              \
            tag_##_##i *x = &((tag_##SumT *)datatype99_priv_matched_val)->data.tag_._##i)))
// } (Pattern matching)

/*
 * For each variant:
 *
 * typedef struct <datatype-name> <variant-name>SumT;
 *
 * (For a non-empty variant:)
 * typedef struct <datatype-name><variant-name> {
 *     <type>0 _0;
 *     ...
 *     <type>N _N;
 * } <datatype-name><variant-name>;
 *
 * typedef <type>0 <variant-name>_0;
 * ...
 * typedef <type>N <variant-name>_N;
 */
#define DATATYPE99_PRIV_genVariantTypedefsForEach(name, variants)                                  \
    DATATYPE99_PRIV_forEachVariant(                                                                \
        ML99_appl(v(DATATYPE99_PRIV_genVariantTypedefs), v(name)),                                 \
        v(variants))

#define DATATYPE99_PRIV_genVariantTypedefs_IMPL(name, tag, sig)                                    \
    ML99_TERMS(                                                                                    \
        v(typedef struct name tag##SumT;),                                                         \
        ML99_IF(                                                                                   \
            ML99_IS_CONS(sig),                                                                     \
            DATATYPE99_PRIV_genVariantStruct(name, tag, sig),                                      \
            ML99_empty()),                                                                         \
        DATATYPE99_PRIV_genParamTypedefForEach(tag, sig))

/*
 * typedef struct <datatype-name><variant-name> {
 *     <type>0 _0;
 *     ...
 *     <type>N _N;
 * } <datatype-name><variant-name>;
 */
#define DATATYPE99_PRIV_genVariantStruct(name, tag, sig)                                           \
    ML99_typedef(v(name##tag), ML99_struct(v(name##tag), ML99_indexedFields(v(sig))))

/*
 * typedef <type>0 <variant-name>_0;
 * ...
 * typedef <type>N <variant-name>_N;
 */
#define DATATYPE99_PRIV_genParamTypedefForEach(tag, sig)                                           \
    ML99_listMapInPlaceI(ML99_appl(v(DATATYPE99_PRIV_genParamTypedef), v(tag)), v(sig))

#define DATATYPE99_PRIV_genParamTypedef_IMPL(tag, type, i) v(typedef type tag##_##i;)

/*
 * <variant-name>0Tag, ..., <variant-name>NTag
 */
#define DATATYPE99_PRIV_genTagForEach(variants)                                                    \
    DATATYPE99_PRIV_forEachVariant(v(DATATYPE99_PRIV_genTag), v(variants))

#define DATATYPE99_PRIV_genTag_IMPL(tag, _sig) v(tag##Tag, )

/*
 * <datatype-name><variant-name>0 <variant-name>0;
 * ...
 * <datatype-name><variant-name>N <variant-name>N;
 */
#define DATATYPE99_PRIV_genUnionFieldForEach(name, variants)                                       \
    ML99_uncomma(ML99_QUOTE(                                                                       \
        v(char dummy;),                                                                            \
        DATATYPE99_PRIV_forEachVariant(                                                            \
            ML99_appl(v(DATATYPE99_PRIV_genUnionField), v(name)),                                  \
            v(variants))))

#define DATATYPE99_PRIV_genUnionField_IMPL(name, tag, sig)                                         \
    ML99_IF(ML99_IS_CONS(sig), v(name##tag tag;), ML99_empty())

/*
 * inline static <datatype99-name> <variant-name>0(...) { ... }
 * ...
 * inline static <datatype99-name> <variant-name>N(...) { ... }
 */
#define DATATYPE99_PRIV_genCtorForEach(name, variants)                                             \
    DATATYPE99_PRIV_forEachVariant(ML99_appl(v(DATATYPE99_PRIV_genCtor), v(name)), v(variants))

#define DATATYPE99_PRIV_genCtor_IMPL(name, tag, sig)                                               \
    ML99_call(                                                                                     \
        DATATYPE99_PRIV_genCtorTemplate,                                                           \
        v(name, tag),                                                                              \
        ML99_indexedParams(v(sig)),                                                                \
        ML99_repeat(ML99_listLen(v(sig)), ML99_appl(v(DATATYPE99_PRIV_assignResult), v(tag))))

#define DATATYPE99_PRIV_assignResult_IMPL(tag, i) v(result.data.tag._##i = _##i;)

#define DATATYPE99_PRIV_genCtorTemplate_IMPL(name, tag_, params, assigned_fields)                  \
    v(inline static DATATYPE99_PRIV_CTOR_ATTRS name tag_ params {                                  \
        name result;                                                                               \
        result.tag = tag_##Tag;                                                                    \
        { assigned_fields }                                                                        \
        return result;                                                                             \
    })

/*
 * <field-type>0 <field-name>0;
 * ...
 * <field-type>N <field-name>N;
 */
#define DATATYPE99_PRIV_genRecordFieldForEach(fields)                                              \
    ML99_listMapInPlace(ML99_compose(v(DATATYPE99_PRIV_genRecordField), v(ML99_untuple)), v(fields))

#define DATATYPE99_PRIV_genRecordField_IMPL(ty, ident) v(ty ident;)

// Compiler-specific stuff {
#if defined(__GNUC__)
#define DATATYPE99_PRIV_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#endif

#if defined(__GNUC__) && !defined(__clang__)
#define DATATYPE99_PRIV_CONST __attribute__((const))
#endif

#define DATATYPE99_PRIV_CTOR_ATTRS DATATYPE99_PRIV_WARN_UNUSED_RESULT DATATYPE99_PRIV_CONST

#ifndef DATATYPE99_PRIV_WARN_UNUSED_RESULT
#define DATATYPE99_PRIV_WARN_UNUSED_RESULT
#endif

#ifndef DATATYPE99_PRIV_CONST
#define DATATYPE99_PRIV_CONST
#endif
// } (Compiler-specific stuff)

// Arity specifiers {
#define DATATYPE99_PRIV_parseVariant_ARITY       1
#define DATATYPE99_PRIV_parseField_ARITY         1
#define DATATYPE99_PRIV_invokeDeriver_ARITY      2
#define DATATYPE99_PRIV_genBinding_ARITY         3
#define DATATYPE99_PRIV_genVariantTypedefs_ARITY 2
#define DATATYPE99_PRIV_genParamTypedef_ARITY    3
#define DATATYPE99_PRIV_genTag_ARITY             1
#define DATATYPE99_PRIV_genUnionField_ARITY      2
#define DATATYPE99_PRIV_genCtor_ARITY            2
#define DATATYPE99_PRIV_assignResult_ARITY       2
#define DATATYPE99_PRIV_genRecordField_ARITY     1

// Public:
#define DATATYPE99_datatype_ARITY            1
#define DATATYPE99_record_ARITY              1
#define DATATYPE99_of_ARITY                  1
#define DATATYPE99_ifLet_ARITY               3
#define DATATYPE99_attrIsPresent_ARITY       1
#define DATATYPE99_attrValue_ARITY           1
#define DATATYPE99_assertAttrIsPresent_ARITY 1
// }

#endif // DATATYPE99_H
