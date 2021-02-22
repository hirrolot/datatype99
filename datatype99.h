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

// The official repository: https://github.com/Hirrolot/datatype99.

#ifndef DATATYPE99_H
#define DATATYPE99_H

#include <stddef.h>

#include <metalang99.h>

#ifndef DATATYPE99_NO_ALIASES

#define datatype  datatype99
#define match     match99
#define matches   matches99
#define of        of99
#define otherwise otherwise99

#define Unit Unit99
#define unit unit99

#define Result      Result99
#define isResultOk  isResultOk99
#define isResultErr isResultErr99
#define tryResult   tryResult99

#endif // DATATYPE99_NO_ALIASES

// Unit type {
typedef char Unit99;
static const Unit99 unit99 = '\0';
// }

// Result<T, E> {
#define Result99(name, T, E) datatype99(name##Result, (name##Ok, T), (name##Err, E))

#define isResultOk99(result)  ((int)(result).tag == 0)
#define isResultErr99(result) ((int)(result).tag == 1)
// }

// Parsing {
#define DATATYPE99_PRIV_parse_IMPL(...)                                                            \
    METALANG99_listMap(v(DATATYPE99_PRIV_parseMap), METALANG99_list(v(__VA_ARGS__)))

#define DATATYPE99_PRIV_parseMap_IMPL(variant)                                                     \
    METALANG99_ifPlain(                                                                            \
        METALANG99_isParenthesizedPlain(variant),                                                  \
        METALANG99_call(DATATYPE99_PRIV_parseVariant, METALANG99_unparenthesize(v(variant))),      \
        METALANG99_fatal(datatype99, variant is unparenthesized))

#define DATATYPE99_PRIV_parseVariant_IMPL(...)                                                     \
    METALANG99_catPlain(                                                                           \
        DATATYPE99_PRIV_parseVariantIsEmpty_,                                                      \
        DATATYPE99_PRIV_isEmptyVariantPlain(__VA_ARGS__))(__VA_ARGS__)

#define DATATYPE99_PRIV_parseVariantIsEmpty_1(tag) DATATYPE99_PRIV_variant(v(tag), METALANG99_nil())

#define DATATYPE99_PRIV_parseVariantIsEmpty_0(tag, ...)                                            \
    DATATYPE99_PRIV_variant(v(tag), METALANG99_list(v(__VA_ARGS__)))
// } (Parsing)

// A variant representation {
#define DATATYPE99_PRIV_variant(tag, variant_params) METALANG99_parenthesize(tag, variant_params)

#define DATATYPE99_PRIV_variantTag    METALANG99_parenthesizedVariadicsHead
#define DATATYPE99_PRIV_variantParams METALANG99_parenthesizedVariadicsTail

#define DATATYPE99_PRIV_isEmptyVariantPlain(...)                                                   \
    METALANG99_uintEqPlain(METALANG99_variadicsCountPlain(__VA_ARGS__), 1)

#define DATATYPE99_PRIV_mapVariants(f, variants)                                                   \
    METALANG99_listUnwrap(                                                                         \
        METALANG99_listMap(METALANG99_compose(f, v(METALANG99_unparenthesize)), variants))

#define DATATYPE99_PRIV_mapVariantsCommaSep(f, variants)                                           \
    METALANG99_listUnwrapCommaSep(                                                                 \
        METALANG99_listMap(METALANG99_compose(f, v(METALANG99_unparenthesize)), variants))
// } (A variant representation)

// Sum type generation {
#define datatype99(name, ...)                                                                      \
    METALANG99_eval(METALANG99_call(                                                               \
        DATATYPE99_PRIV_genDatatypeAux,                                                            \
        v(name),                                                                                   \
        METALANG99_callTrivial(DATATYPE99_PRIV_parse, __VA_ARGS__)))                               \
                                                                                                   \
    METALANG99_semicolon()

#define DATATYPE99_PRIV_genDatatypeAux_IMPL(name, variants)                                        \
    METALANG99_call(                                                                               \
        DATATYPE99_PRIV_genDatatypeAuxAux,                                                         \
        v(name),                                                                                   \
        DATATYPE99_PRIV_genTypedefs(name, variants),                                               \
        METALANG99_parenthesize(DATATYPE99_PRIV_genTags(variants)),                                \
        DATATYPE99_PRIV_genUnionFields(name, variants),                                            \
        DATATYPE99_PRIV_genCtors(name, variants))

#define DATATYPE99_PRIV_genDatatypeAuxAux_IMPL(name, typedefs, tags, union_fields, ctors)          \
    v(typedef struct name name;                                                                    \
                                                                                                   \
      typedefs                                                                                     \
                                                                                                   \
      struct name {                                                                                \
          enum { METALANG99_unparenthesizePlain(tags) } tag;                                       \
          union {                                                                                  \
              char dummy;                                                                          \
              union_fields                                                                         \
          } data;                                                                                  \
      };                                                                                           \
                                                                                                   \
      ctors)
// } (Sum type generation)

// Pattern matching {
#define match99(val)                                                                               \
    DATATYPE99_PRIV_GCC_PRAGMA("GCC diagnostic push")                                              \
    DATATYPE99_PRIV_GCC_PRAGMA("GCC diagnostic ignored \"-Wmisleading-indentation\"")              \
    DATATYPE99_PRIV_GCC_PRAGMA("GCC diagnostic ignored \"-Wreturn-type\"")                         \
                                                                                                   \
    /* `for` is used to extend the context with a bounded variable (`datatype99_priv_match_expr`   \
     * below). The reason we use `for` is that we can't do the same with a simple variable         \
     * definition because the next use of `match99` will define this variable again, unlike `for`  \
     * which creates a new scope. */                                                               \
    METALANG99_introduceVarToStmt(                                                                 \
        const void *DATATYPE99_PRIV_POSSIBLY_UNUSED datatype99_priv_match_expr =                   \
            (const void *)&(val))                                                                  \
                                                                                                   \
        switch ((val).tag)

#define of99(...)                                                                                  \
    DATATYPE99_PRIV_GCC_PRAGMA("GCC diagnostic pop")                                               \
    DATATYPE99_PRIV_GCC_PRAGMA("GCC diagnostic push")                                              \
    DATATYPE99_PRIV_GCC_PRAGMA("GCC diagnostic ignored \"-Wmisleading-indentation\"")              \
    DATATYPE99_PRIV_GCC_PRAGMA("GCC diagnostic ignored \"-Wreturn-type\"")                         \
                                                                                                   \
    break;                                                                                         \
    METALANG99_ifPlain(                                                                            \
        DATATYPE99_PRIV_isEmptyVariantPlain(__VA_ARGS__),                                          \
        DATATYPE99_PRIV_ofEmpty,                                                                   \
        DATATYPE99_PRIV_ofNonEmpty)(__VA_ARGS__)

#define DATATYPE99_PRIV_ofEmpty(tag) case tag##Tag:

#define DATATYPE99_PRIV_ofNonEmpty(tag, ...)                                                       \
    case tag##Tag:                                                                                 \
        METALANG99_listEval(METALANG99_listMapI(                                                   \
            METALANG99_appl(v(DATATYPE99_PRIV_genBinding), v(tag)),                                \
            METALANG99_list(v(__VA_ARGS__))))

#define DATATYPE99_PRIV_genBinding_IMPL(tag_, x, i)                                                \
    v(METALANG99_introduceVarToStmt(                                                               \
        tag_##_##i *x = &((tag_##SumT *)datatype99_priv_match_expr)->data.tag_._##i))

#define otherwise99                                                                                \
    break;                                                                                         \
    default:                                                                                       \
        DATATYPE99_PRIV_GCC_PRAGMA("GCC diagnostic pop")

#define matches99(val, tag_) ((val).tag == tag_##Tag)
// } (Pattern matching)

#define DATATYPE99_PRIV_genTypedefs(name, variants)                                                \
    DATATYPE99_PRIV_mapVariants(                                                                   \
        METALANG99_appl(v(DATATYPE99_PRIV_genTypedefsMap), v(name)),                               \
        v(variants))

#define DATATYPE99_PRIV_genTypedefsMap_IMPL(name, tag, variant_params)                             \
    METALANG99_call(                                                                               \
        DATATYPE99_PRIV_genTypedefsTemplate,                                                       \
        v(typedef struct name tag##SumT;),                                                         \
        DATATYPE99_PRIV_genVariantStructs(name, tag, variant_params),                              \
        DATATYPE99_PRIV_genVariantParamsTypedefs(tag, variant_params))

#define DATATYPE99_PRIV_genTypedefsTemplate_IMPL(                                                  \
    sum_t_typedef,                                                                                 \
    variant_structs,                                                                               \
    variant_params_typedefs)                                                                       \
    v(sum_t_typedef variant_structs variant_params_typedefs)

/*
 * typedef struct <datatype-name><variant-name> {
 *     <type>0 _0;
 *     ...
 *     <type>N _N;
 * } <datatype-name><variant-name>;
 */
#define DATATYPE99_PRIV_genVariantStructs(name, tag, variant_params)                               \
    METALANG99_ifPlain(                                                                            \
        METALANG99_isConsPlain(variant_params),                                                    \
        METALANG99_typedef(                                                                        \
            v(name##tag),                                                                          \
            METALANG99_struct(v(name##tag), METALANG99_indexedFields(v(variant_params)))),         \
        METALANG99_empty())

/*
 * typedef <type>0 <variant-name>_0;
 * ...
 * typedef <type>N <variant-name>_N;
 */
#define DATATYPE99_PRIV_genVariantParamsTypedefs(tag, variant_params)                              \
    METALANG99_ifPlain(                                                                            \
        METALANG99_isConsPlain(variant_params),                                                    \
        METALANG99_listUnwrap(METALANG99_listMapI(                                                 \
            METALANG99_appl(v(DATATYPE99_PRIV_genVariantParamTypedef), v(tag)),                    \
            v(variant_params))),                                                                   \
        METALANG99_empty())

#define DATATYPE99_PRIV_genVariantParamTypedef_IMPL(tag, type, i) v(typedef type tag##_##i;)

/*
 * <variant-name>0Tag, ..., <variant-name>NTag
 */
#define DATATYPE99_PRIV_genTags(variants)                                                          \
    DATATYPE99_PRIV_mapVariantsCommaSep(v(DATATYPE99_PRIV_genTag), v(variants))

#define DATATYPE99_PRIV_genTag_IMPL(tag, _variant_params) v(tag##Tag)

/*
 * <datatype-name><variant-name>0 <variant-name>0;
 * ...
 * <datatype-name><variant-name>N <variant-name>N;
 */
#define DATATYPE99_PRIV_genUnionFields(name, variants)                                             \
    DATATYPE99_PRIV_mapVariants(                                                                   \
        METALANG99_appl(v(DATATYPE99_PRIV_genUnionField), v(name)),                                \
        v(variants))

#define DATATYPE99_PRIV_genUnionField_IMPL(name, tag, variant_params)                              \
    METALANG99_ifPlain(                                                                            \
        METALANG99_isConsPlain(variant_params),                                                    \
        v(name##tag tag;),                                                                         \
        METALANG99_empty())

/*
 * inline static <datatype99-name> <variant-name>0(...) { ... }
 * ...
 * inline static <datatype99-name> <variant-name>N(...) { ... }
 */
#define DATATYPE99_PRIV_genCtors(name, variants)                                                   \
    DATATYPE99_PRIV_mapVariants(                                                                   \
        METALANG99_appl(v(DATATYPE99_PRIV_genCtorsMap), v(name)),                                  \
        v(variants))

#define DATATYPE99_PRIV_genCtorsMap_IMPL(name, tag, variant_params)                                \
    METALANG99_call(                                                                               \
        METALANG99_catPlain(                                                                       \
            DATATYPE99_PRIV_genCtorIsEmpty_,                                                       \
            METALANG99_isNilPlain(variant_params)),                                                \
        v(name, tag),                                                                              \
        METALANG99_indexedParams(v(variant_params)),                                               \
        METALANG99_indexedInitializerList(METALANG99_listLen(v(variant_params))))

#define DATATYPE99_PRIV_genCtorIsEmpty_1_IMPL(name, tag_, params, ...)                             \
    v(inline static DATATYPE99_PRIV_WARN_UNUSED_RESULT DATATYPE99_PRIV_CONST name tag_ params {    \
        return ((name){.tag = tag_##Tag});                                                         \
    })

#define DATATYPE99_PRIV_genCtorIsEmpty_0_IMPL(name, tag_, params, ...)                             \
    v(inline static DATATYPE99_PRIV_WARN_UNUSED_RESULT DATATYPE99_PRIV_CONST name tag_ params {    \
        return ((name){.tag = tag_##Tag, .data.tag_ = __VA_ARGS__});                               \
    })

// Compiler-specific stuff {
#if defined(__GNUC__) && !defined(__clang__)
#define DATATYPE99_PRIV_GCC_PRAGMA(str) _Pragma(str)
#else
#define DATATYPE99_PRIV_GCC_PRAGMA(str)
#endif

#ifdef __GNUC__
#define DATATYPE99_PRIV_POSSIBLY_UNUSED __attribute__((unused))
#else
#define DATATYPE99_PRIV_POSSIBLY_UNUSED
#endif

#ifdef __GNUC__
#define DATATYPE99_PRIV_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#else
#define DATATYPE99_PRIV_WARN_UNUSED_RESULT
#endif // __GNU__

#if defined(__GNUC__) && !defined(__clang__)
#define DATATYPE99_PRIV_CONST __attribute__((const))
#else
#define DATATYPE99_PRIV_CONST
#endif
// } (Compiler-specific stuff)

// Arity specifiers {
#define DATATYPE99_PRIV_parseMap_ARITY               1
#define DATATYPE99_PRIV_genBinding_ARITY             3
#define DATATYPE99_PRIV_genTypedefsMap_ARITY         2
#define DATATYPE99_PRIV_genVariantStructsMap_ARITY   2
#define DATATYPE99_PRIV_genVariantParamTypedef_ARITY 3
#define DATATYPE99_PRIV_genTag_ARITY                 1
#define DATATYPE99_PRIV_genUnionField_ARITY          2
#define DATATYPE99_PRIV_genCtorsMap_ARITY            2
#define DATATYPE99_PRIV_genCtorParamsMap_ARITY       2
#define DATATYPE99_PRIV_genCtorParamNamesMap_ARITY   1
// }

#endif // DATATYPE99_H
