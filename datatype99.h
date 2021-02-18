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

// Sum type generation {
#define datatype99(name, ...)                                                                      \
    METALANG99_ifPlain(                                                                            \
        DATATYPE99_PRIV_CHECK_VARIANTS(__VA_ARGS__),                                               \
        DATATYPE99_PRIV_genDatatype,                                                               \
        DATATYPE99_PRIV_reportVariantsError)                                                       \
    (name, __VA_ARGS__)

// Checks that all given variants are parenthesised.
#define DATATYPE99_PRIV_CHECK_VARIANTS(...) 1 /* TODO */

#define DATATYPE99_PRIV_reportVariantsError(...) /* TODO */

#define DATATYPE99_PRIV_genDatatype(name, ...)                                                     \
    DATATYPE99_PRIV_genDatatypeAux(name, METALANG99_eval(METALANG99_list(v(__VA_ARGS__))))

#define DATATYPE99_PRIV_genDatatypeAux(name, variants)                                             \
    DATATYPE99_PRIV_genTypedefs(name, variants)                                                    \
                                                                                                   \
    struct name {                                                                                  \
        enum { DATATYPE99_PRIV_genTags(variants) } tag;                                            \
                                                                                                   \
        union {                                                                                    \
            char dummy;                                                                            \
            DATATYPE99_PRIV_genUnionFields(name, variants)                                         \
        } data;                                                                                    \
    };                                                                                             \
                                                                                                   \
    DATATYPE99_PRIV_genCtors(name, variants)                                                       \
                                                                                                   \
    METALANG99_semicolon()
// }

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
    case METALANG99_catPlain(DATATYPE99_PRIV_extractTag(__VA_ARGS__), Tag):                        \
        METALANG99_whenLazyPlain(                                                                  \
            DATATYPE99_PRIV_isNonEmptyVariant(__VA_ARGS__),                                        \
            DATATYPE99_PRIV_ofNonEmpty)                                                            \
        (__VA_ARGS__)

#define DATATYPE99_PRIV_ofNonEmpty(tag, ...)                                                       \
    METALANG99_listEval(METALANG99_listMapI(                                                       \
        METALANG99_appl(v(DATATYPE99_PRIV_genBoundedVar), v(tag)),                                 \
        METALANG99_list(v(__VA_ARGS__))))

#define DATATYPE99_PRIV_genBoundedVar_IMPL(tag_, x, i)                                             \
    v(METALANG99_introduceVarToStmt(                                                               \
        tag_##_##i *x = &((tag_##SumT *)datatype99_priv_match_expr)->data.tag_._##i))

#define otherwise99                                                                                \
    break;                                                                                         \
    default:                                                                                       \
        DATATYPE99_PRIV_GCC_PRAGMA("GCC diagnostic pop")

#define matches99(val, tag_) ((val).tag == tag_##Tag)
// } (Pattern matching)

// Desugaring {
#define DATATYPE99_PRIV_mapVariants(f, variants)                                                   \
    METALANG99_call(DATATYPE99_PRIV_mapVariants, f, variants)
#define DATATYPE99_PRIV_mapVariantsCommaSep(f, variants)                                           \
    METALANG99_call(DATATYPE99_PRIV_mapVariantsCommaSep, f, variants)
// }

// Implementation {

// Generate type definitions for variants {
#define DATATYPE99_PRIV_genTypedefs(name, variants)                                                \
    typedef struct name name;                                                                      \
                                                                                                   \
    METALANG99_eval(DATATYPE99_PRIV_mapVariants(                                                   \
        METALANG99_appl(v(DATATYPE99_PRIV_genTypedefsMap), v(name)),                               \
        v(variants)))

#define DATATYPE99_PRIV_genTypedefsMap_IMPL(name, ...)                                             \
    METALANG99_terms(                                                                              \
        METALANG99_call(                                                                           \
            METALANG99_callTrivial(                                                                \
                METALANG99_whenLazy,                                                               \
                DATATYPE99_PRIV_isNonEmptyVariant(__VA_ARGS__),                                    \
                DATATYPE99_PRIV_genTypedefsMapNonEmpty),                                           \
            v(name, __VA_ARGS__)),                                                                 \
                                                                                                   \
        /* typedef struct <datatype-name> <variant-name>SumT; */                                   \
        v(typedef struct name METALANG99_catPlain(                                                 \
              DATATYPE99_PRIV_extractTag(__VA_ARGS__),                                             \
              SumT);))

#define DATATYPE99_PRIV_genTypedefsMapNonEmpty_IMPL(name, tag, ...)                                \
    METALANG99_call(                                                                               \
        DATATYPE99_PRIV_genTypedefsMapNonEmptyAux,                                                 \
        v(name, tag),                                                                              \
        METALANG99_list(v(__VA_ARGS__)))

#define DATATYPE99_PRIV_genTypedefsMapNonEmptyAux_IMPL(name, tag, types)                           \
    METALANG99_terms(                                                                              \
        DATATYPE99_PRIV_genStructOfVariantFields(name, tag, types),                                \
        DATATYPE99_PRIV_genTypedefsToFields(tag, types))

/*
 * typedef struct <datatype-name><variant-name> {
 *     <type>0 _0;
 *     ...
 *     <type>N _N;
 * } <datatype-name><variant-name>;
 */
#define DATATYPE99_PRIV_genStructOfVariantFields(name, tag, types)                                 \
    METALANG99_call(                                                                               \
        DATATYPE99_PRIV_genStructOfVariantFieldsAux,                                               \
        v(name, tag),                                                                              \
        METALANG99_listUnwrap(METALANG99_callTrivial(                                              \
            METALANG99_listMapI,                                                                   \
            DATATYPE99_PRIV_genStructOfVariantFieldsMap,                                           \
            types)))
#define DATATYPE99_PRIV_genStructOfVariantFieldsMap_IMPL(field_type, i) v(field_type _##i;)

#define DATATYPE99_PRIV_genStructOfVariantFieldsAux_IMPL(name, tag, fields)                        \
    v(typedef struct name##tag{fields} name##tag;)

/*
 * typedef <type>0 <variant-name>_0;
 * ...
 * typedef <type>N <variant-name>_N;
 */
#define DATATYPE99_PRIV_genTypedefsToFields(tag, types)                                            \
    METALANG99_listUnwrap(METALANG99_listMapI(                                                     \
        METALANG99_callTrivial(METALANG99_appl, DATATYPE99_PRIV_genTypedefToField, tag),           \
        v(types)))
#define DATATYPE99_PRIV_genTypedefToField_IMPL(tag, field_type, i) v(typedef field_type tag##_##i;)
// }

// Generate tags of variants {

// <variant-name>0Tag, ..., <variant-name>NTag
#define DATATYPE99_PRIV_genTags(variants)                                                          \
    METALANG99_eval(DATATYPE99_PRIV_mapVariantsCommaSep(v(DATATYPE99_PRIV_genTagsMap), v(variants)))

#define DATATYPE99_PRIV_genTagsMap_IMPL(...)                                                       \
    v(METALANG99_catPlain(DATATYPE99_PRIV_extractTag(__VA_ARGS__), Tag))
// }

// Generate a union of fields of possible data {

/*
 * <datatype-name><variant-name>0 <variant-name>0;
 * ...
 * <datatype-name><variant-name>N <variant-name>N;
 */
#define DATATYPE99_PRIV_genUnionFields(name, variants)                                             \
    METALANG99_eval(DATATYPE99_PRIV_mapVariants(                                                   \
        METALANG99_appl(v(DATATYPE99_PRIV_genUnionFieldsMap), v(name)),                            \
        v(variants)))

#define DATATYPE99_PRIV_genUnionFieldsMap_IMPL(name, ...)                                          \
    v(METALANG99_whenLazyPlain(                                                                    \
        DATATYPE99_PRIV_isNonEmptyVariant(__VA_ARGS__),                                            \
        DATATYPE99_PRIV_genUnionField)(name, __VA_ARGS__))

#define DATATYPE99_PRIV_genUnionField(name, tag, ...) name##tag tag;
// }

// Generate value constructors {

/*
 * inline static <datatype99-name> <variant-name>0(...) { ... }
 * ...
 * inline static <datatype99-name> <variant-name>N(...) { ... }
 */
#define DATATYPE99_PRIV_genCtors(name, variants)                                                   \
    METALANG99_eval(DATATYPE99_PRIV_mapVariants(                                                   \
        METALANG99_appl(v(DATATYPE99_PRIV_genCtorsMap), v(name)),                                  \
        v(variants)))

#define DATATYPE99_PRIV_genCtorsMap_IMPL(name, ...)                                                \
    METALANG99_ifPlain(                                                                            \
        DATATYPE99_PRIV_isEmptyVariant(__VA_ARGS__),                                               \
        DATATYPE99_PRIV_genEmptyCtor,                                                              \
        DATATYPE99_PRIV_genNonEmptyCtor)                                                           \
    (name, __VA_ARGS__)

#define DATATYPE99_PRIV_genEmptyCtor(name, tag_)                                                   \
    v(inline static DATATYPE99_PRIV_WARN_UNUSED_RESULT DATATYPE99_PRIV_CONST name tag_(void) {     \
        return ((name){.tag = tag_##Tag, .data.dummy = '\0'});                                     \
    })

#define DATATYPE99_PRIV_genNonEmptyCtor(name, tag, ...)                                            \
    METALANG99_call(                                                                               \
        DATATYPE99_PRIV_genCtorAux,                                                                \
        v(name, tag),                                                                              \
        METALANG99_parenthesise(DATATYPE99_PRIV_genCtorParams(__VA_ARGS__)),                       \
        DATATYPE99_PRIV_genCtorParamNames(METALANG99_variadicsCountPlain(__VA_ARGS__)))

#define DATATYPE99_PRIV_genCtorParams(...)                                                         \
    METALANG99_listUnwrapCommaSep(METALANG99_call(                                                 \
        METALANG99_listMapI,                                                                       \
        v(DATATYPE99_PRIV_genCtorParamsMap),                                                       \
        METALANG99_callTrivial(METALANG99_list, __VA_ARGS__)))
#define DATATYPE99_PRIV_genCtorParamsMap_IMPL(type, i) v(type _##i)

#define DATATYPE99_PRIV_genCtorParamNames(types_count)                                             \
    METALANG99_callTrivial(METALANG99_repeat, DATATYPE99_PRIV_genCtorParamNamesMap, types_count)
#define DATATYPE99_PRIV_genCtorParamNamesMap_IMPL(i) v(_##i, )

#define DATATYPE99_PRIV_genCtorAux_IMPL(name, tag_, params, ...)                                   \
    v(inline static DATATYPE99_PRIV_WARN_UNUSED_RESULT DATATYPE99_PRIV_CONST name tag_ params {    \
        return ((name){.tag = tag_##Tag, .data.tag_ = {__VA_ARGS__}});                             \
    })
// }

// Auxiliary stuff {
#define DATATYPE99_PRIV_isEmptyVariant(...)                                                        \
    METALANG99_uintEqPlain(METALANG99_variadicsCountPlain(__VA_ARGS__), 1)

#define DATATYPE99_PRIV_isNonEmptyVariant(...)                                                     \
    METALANG99_notPlain(DATATYPE99_PRIV_isEmptyVariant(__VA_ARGS__))

#define DATATYPE99_PRIV_extractTag METALANG99_variadicsHeadPlain

#define DATATYPE99_PRIV_mapVariants_IMPL(f, variants)                                              \
    METALANG99_listUnwrap(METALANG99_listMap(                                                      \
        METALANG99_callTrivial(METALANG99_compose, f, METALANG99_unparenthesise),                  \
        v(variants)))

#define DATATYPE99_PRIV_mapVariantsCommaSep_IMPL(f, variants)                                      \
    METALANG99_listUnwrapCommaSep(METALANG99_listMap(                                              \
        METALANG99_callTrivial(METALANG99_compose, f, METALANG99_unparenthesise),                  \
        v(variants)))
// }

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
// }

// }

// } (Implementation)

// Arity specifiers {
#define DATATYPE99_PRIV_genBoundedVar_ARITY               3
#define DATATYPE99_PRIV_genTypedefsMap_ARITY              2
#define DATATYPE99_PRIV_genStructOfVariantFieldsMap_ARITY 2
#define DATATYPE99_PRIV_genTypedefToField_ARITY           3
#define DATATYPE99_PRIV_genTagsMap_ARITY                  1
#define DATATYPE99_PRIV_genUnionFieldsMap_ARITY           2
#define DATATYPE99_PRIV_genCtorsMap_ARITY                 2
#define DATATYPE99_PRIV_genCtorParamsMap_ARITY            2
#define DATATYPE99_PRIV_genCtorParamNamesMap_ARITY        1
// }

#endif // DATATYPE99_H
