/*
 * MIT License
 *
 * Copyright (c) 2020 Temirkhan Myrzamadi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

// The official repository: <https://github.com/Hirrolot/datatype99>.

#ifndef DATATYPE99_H
#define DATATYPE99_H

#include <stddef.h>

#include <metalang99.h>

#if ML99_MAJOR != 0 || ML99_MINOR != 5
#error "Please, update Metalang99 to v0.5.0 or later"
#endif

#ifndef DATATYPE99_NO_ALIASES

#define datatype  datatype99
#define match     match99
#define matches   matches99
#define ifLet     ifLet99
#define of        of99
#define otherwise otherwise99

#define Unit Unit99
#define unit unit99

#endif // DATATYPE99_NO_ALIASES

// Unit type {
typedef char Unit99;
static const Unit99 unit99 = '\0';
// }

// Parsing {
#define DATATYPE99_PRIV_parse(...)                                                                 \
    ML99_listMap(v(DATATYPE99_PRIV_parseMap), ML99_list(v(__VA_ARGS__)))

#define DATATYPE99_PRIV_parseMap_IMPL(variant)                                                     \
    ML99_IF(                                                                                       \
        ML99_IS_TUPLE(variant),                                                                    \
        ML99_call(DATATYPE99_PRIV_parseVariant, ML99_untuple(v(variant))),                         \
        ML99_fatal(datatype99, variant is unparenthesised))

#define DATATYPE99_PRIV_parseVariant_IMPL(...)                                                     \
    ML99_CAT(DATATYPE99_PRIV_parseVariantIsEmpty_, DATATYPE99_PRIV_IS_EMPTY_VARIANT(__VA_ARGS__))  \
    (__VA_ARGS__)

#define DATATYPE99_PRIV_parseVariantIsEmpty_1(tag) DATATYPE99_PRIV_variant(v(tag), ML99_nil())
#define DATATYPE99_PRIV_parseVariantIsEmpty_0(tag, ...)                                            \
    DATATYPE99_PRIV_variant(v(tag), ML99_list(v(__VA_ARGS__)))
// } (Parsing)

// A variant representation {
#define DATATYPE99_PRIV_variant(tag, sig) ML99_tuple(tag, sig)

#define DATATYPE99_PRIV_variantTag    ML99_TUPLE_GET(0)
#define DATATYPE99_PRIV_variantParams ML99_TUPLE_GET(1)

#define DATATYPE99_PRIV_IS_EMPTY_VARIANT(...) ML99_NAT_EQ(ML99_VARIADICS_COUNT(__VA_ARGS__), 1)

#define DATATYPE99_PRIV_mapVariants(f, variants)                                                   \
    ML99_listMapInPlace(ML99_compose(f, v(ML99_untuple)), variants)
// } (A variant representation)

// Sum type generation {

#define datatype99(name, ...)                                                                      \
    ML99_EVAL(ML99_call(DATATYPE99_PRIV_genDatatype, v(name), DATATYPE99_PRIV_parse(__VA_ARGS__))) \
    struct name

#define DATATYPE99_PRIV_genDatatype_IMPL(name, variants)                                           \
    ML99_TERMS(                                                                                    \
        v(typedef struct name name;),                                                              \
        DATATYPE99_PRIV_genTypedefs(name, variants),                                               \
        ML99_typedef(v(name##Tag), ML99_enum(v(name##Tag), DATATYPE99_PRIV_genTags(variants))),    \
        ML99_typedef(                                                                              \
            v(name##Variants),                                                                     \
            ML99_union(v(name##Variants), DATATYPE99_PRIV_genUnionFields(v(name), v(variants)))),  \
        v(struct name {                                                                            \
            name##Tag tag;                                                                         \
            name##Variants data;                                                                   \
        };),                                                                                       \
        DATATYPE99_PRIV_genCtors(name, variants))
// } (Sum type generation)

// Pattern matching {

// clang-format off

#define match99(val)                                                                               \
    DATATYPE99_PRIV_DIAGNOSTIC_PUSH                                                                \
    DATATYPE99_PRIV_SUPPRESS_W_MISLEADING_INDENTATION                                              \
    DATATYPE99_PRIV_SUPPRESS_W_RETURN_TYPE                                                         \
                                                                                                   \
    ML99_INTRODUCE_VAR_TO_STMT(void *datatype99_priv_match_expr = (void *)&(val))                  \
        ML99_SUPPRESS_UNUSED_BEFORE_STMT(datatype99_priv_match_expr)                               \
            switch ((val).tag)
// clang-format on

#define of99(...)                                                                                  \
    DATATYPE99_PRIV_DIAGNOSTIC_POP                                                                 \
    DATATYPE99_PRIV_DIAGNOSTIC_PUSH                                                                \
    DATATYPE99_PRIV_SUPPRESS_W_MISLEADING_INDENTATION                                              \
    DATATYPE99_PRIV_SUPPRESS_W_RETURN_TYPE                                                         \
                                                                                                   \
    break;                                                                                         \
    ML99_IF(                                                                                       \
        DATATYPE99_PRIV_IS_EMPTY_VARIANT(__VA_ARGS__),                                             \
        DATATYPE99_PRIV_ofEmpty,                                                                   \
        DATATYPE99_PRIV_ofNonEmpty)                                                                \
    (__VA_ARGS__)

#define DATATYPE99_PRIV_ofEmpty(tag) case tag##Tag:
#define DATATYPE99_PRIV_ofNonEmpty(tag, ...)                                                       \
    case tag##Tag:                                                                                 \
        ML99_EVAL(DATATYPE99_PRIV_genBindings(v(tag), v(__VA_ARGS__)))

#define otherwise99                                                                                \
    break;                                                                                         \
    default:                                                                                       \
        DATATYPE99_PRIV_DIAGNOSTIC_POP

// clang-format off

#define ifLet99(val, tag_, ...)                                                                    \
    if (tag_##Tag == (val).tag)                                                                    \
        ML99_INTRODUCE_VAR_TO_STMT(void *datatype99_priv_match_expr = (void *)&(val))              \
            ML99_SUPPRESS_UNUSED_BEFORE_STMT(datatype99_priv_match_expr)                           \
                ML99_EVAL(DATATYPE99_PRIV_genBindings(v(tag_), v(__VA_ARGS__)))
// clang-format on

#define matches99(val, tag_) ((val).tag == tag_##Tag)

#define DATATYPE99_PRIV_genBindings(tag, ...)                                                      \
    ML99_variadicsForEachI(ML99_appl(v(DATATYPE99_PRIV_genBinding), tag), __VA_ARGS__)

#define DATATYPE99_PRIV_genBinding_IMPL(tag_, x, i)                                                \
    ML99_IF(                                                                                       \
        ML99_DETECT_IDENT(DATATYPE99_PRIV_isUnderscore_, x),                                       \
        ML99_empty(),                                                                              \
        v(ML99_INTRODUCE_VAR_TO_STMT(                                                              \
            tag_##_##i *x = &((tag_##SumT *)datatype99_priv_match_expr)->data.tag_._##i)))

#define DATATYPE99_PRIV_isUnderscore__ ()
// } (Pattern matching)

#define DATATYPE99_PRIV_genTypedefs(name, variants)                                                \
    DATATYPE99_PRIV_mapVariants(                                                                   \
        ML99_appl(v(DATATYPE99_PRIV_genTypedefsForVariant), v(name)),                              \
        v(variants))

#define DATATYPE99_PRIV_genTypedefsForVariant_IMPL(name, tag, sig)                                 \
    ML99_TERMS(                                                                                    \
        v(typedef struct name tag##SumT;),                                                         \
        ML99_IF(                                                                                   \
            ML99_IS_CONS(sig),                                                                     \
            DATATYPE99_PRIV_genVariantStruct(name, tag, sig),                                      \
            ML99_empty()),                                                                         \
        DATATYPE99_PRIV_genVariantParamsTypedefs(tag, sig))

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
#define DATATYPE99_PRIV_genVariantParamsTypedefs(tag, sig)                                         \
    ML99_listUnwrap(                                                                               \
        ML99_listMapI(ML99_appl(v(DATATYPE99_PRIV_genVariantParamTypedef), v(tag)), v(sig)))

#define DATATYPE99_PRIV_genVariantParamTypedef_IMPL(tag, type, i) v(typedef type tag##_##i;)

/*
 * <variant-name>0Tag, ..., <variant-name>NTag
 */
#define DATATYPE99_PRIV_genTags(variants)                                                          \
    DATATYPE99_PRIV_mapVariants(v(DATATYPE99_PRIV_genTag), v(variants))

#define DATATYPE99_PRIV_genTag_IMPL(tag, _sig) v(tag##Tag, )

/*
 * <datatype-name><variant-name>0 <variant-name>0;
 * ...
 * <datatype-name><variant-name>N <variant-name>N;
 */
#define DATATYPE99_PRIV_genUnionFields(name, variants)                                             \
    ML99_call(DATATYPE99_PRIV_genUnionFields, name, variants)

#define DATATYPE99_PRIV_genUnionFields_IMPL(name, variants)                                        \
    ML99_TERMS(                                                                                    \
        v(char dummy;),                                                                            \
        DATATYPE99_PRIV_mapVariants(                                                               \
            ML99_appl(v(DATATYPE99_PRIV_genUnionField), v(name)),                                  \
            v(variants)))

#define DATATYPE99_PRIV_genUnionField_IMPL(name, tag, sig)                                         \
    ML99_IF(ML99_IS_CONS(sig), v(name##tag tag;), ML99_empty())

/*
 * inline static <datatype99-name> <variant-name>0(...) { ... }
 * ...
 * inline static <datatype99-name> <variant-name>N(...) { ... }
 */
#define DATATYPE99_PRIV_genCtors(name, variants)                                                   \
    DATATYPE99_PRIV_mapVariants(ML99_appl(v(DATATYPE99_PRIV_genCtor), v(name)), v(variants))

#define DATATYPE99_PRIV_genCtor_IMPL(name, tag, sig)                                               \
    ML99_call(                                                                                     \
        DATATYPE99_PRIV_genCtorTemplate,                                                           \
        v(name, tag),                                                                              \
        ML99_indexedParams(v(sig)),                                                                \
        ML99_repeat(ML99_listLen(v(sig)), ML99_appl(v(DATATYPE99_PRIV_assignResult), v(tag))))

#define DATATYPE99_PRIV_assignResult_IMPL(tag, i) v(result.data.tag._##i = _##i;)

// clang-format off
#define DATATYPE99_PRIV_genCtorTemplate_IMPL(name, tag_, params, assigned_fields)                  \
    v(inline static DATATYPE99_PRIV_CTOR_ATTRS name tag_ params {                                  \
        name result;                                                                               \
        result.tag = tag_##Tag;                                                                    \
        assigned_fields                                                                            \
        return result;                                                                             \
    })
// clang-format on

// Compiler-specific stuff {
#if defined(__clang__)

#define DATATYPE99_PRIV_DIAGNOSTIC_PUSH        _Pragma("clang diagnostic push")
#define DATATYPE99_PRIV_DIAGNOSTIC_POP         _Pragma("clang diagnostic pop")
#define DATATYPE99_PRIV_SUPPRESS_W_RETURN_TYPE _Pragma("clang diagnostic ignored \"-Wreturn-type\"")
#define DATATYPE99_PRIV_CONST

#elif defined(__GNUC__)

#define DATATYPE99_PRIV_DIAGNOSTIC_PUSH        _Pragma("GCC diagnostic push")
#define DATATYPE99_PRIV_DIAGNOSTIC_POP         _Pragma("GCC diagnostic pop")
#define DATATYPE99_PRIV_SUPPRESS_W_RETURN_TYPE _Pragma("GCC diagnostic ignored \"-Wreturn-type\"")
#define DATATYPE99_PRIV_CONST                  __attribute__((const))

#else

#define DATATYPE99_PRIV_DIAGNOSTIC_PUSH
#define DATATYPE99_PRIV_DIAGNOSTIC_POP
#define DATATYPE99_PRIV_SUPPRESS_W_RETURN_TYPE
#define DATATYPE99_PRIV_CONST

#endif

#ifdef __GNUC__
#define DATATYPE99_PRIV_UNUSED             __attribute__((unused))
#define DATATYPE99_PRIV_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#else
#define DATATYPE99_PRIV_UNUSED
#define DATATYPE99_PRIV_WARN_UNUSED_RESULT
#endif

#if !defined(__clang__) && __GNUC__ >= 6
#define DATATYPE99_PRIV_SUPPRESS_W_MISLEADING_INDENTATION                                          \
    _Pragma("GCC diagnostic ignored \"-Wmisleading-indentation\"")
#else
#define DATATYPE99_PRIV_SUPPRESS_W_MISLEADING_INDENTATION
#endif

#define DATATYPE99_PRIV_CTOR_ATTRS DATATYPE99_PRIV_WARN_UNUSED_RESULT DATATYPE99_PRIV_CONST
// } (Compiler-specific stuff)

// Arity specifiers {
#define DATATYPE99_PRIV_parseMap_ARITY               1
#define DATATYPE99_PRIV_genBinding_ARITY             3
#define DATATYPE99_PRIV_genTypedefsForVariant_ARITY  2
#define DATATYPE99_PRIV_genVariantParamTypedef_ARITY 3
#define DATATYPE99_PRIV_genTag_ARITY                 1
#define DATATYPE99_PRIV_genUnionField_ARITY          2
#define DATATYPE99_PRIV_genCtor_ARITY                2
#define DATATYPE99_PRIV_assignResult_ARITY           2
// }

#endif // DATATYPE99_H
