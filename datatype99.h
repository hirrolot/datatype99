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

#endif // DATATYPE99_NO_ALIASES

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
    DATATYPE99_PRIV_genTypedefs(name, __VA_ARGS__)                                                 \
                                                                                                   \
    struct name {                                                                                  \
        enum { DATATYPE99_PRIV_genTags(__VA_ARGS__) } tag;                                         \
                                                                                                   \
        union {                                                                                    \
            char dummy;                                                                            \
            DATATYPE99_PRIV_genUnionFields(name, __VA_ARGS__)                                      \
        } data;                                                                                    \
    };                                                                                             \
                                                                                                   \
    DATATYPE99_PRIV_genCtors(name, __VA_ARGS__)                                                    \
                                                                                                   \
    METALANG99_semicolon()

#define match99(val)                                                                               \
    DATATYPE99_PRIV_GCC_PRAGMA("GCC diagnostic push")                                              \
    DATATYPE99_PRIV_GCC_PRAGMA("GCC diagnostic ignored \"-Wmisleading-indentation\"")              \
    DATATYPE99_PRIV_GCC_PRAGMA("GCC diagnostic ignored \"-Wreturn-type\"")                         \
                                                                                                   \
    /* `for` is used to extend the context with a bounded variable (`datatype99_priv_match_expr`   \
     * below). The reason we use `for` is that we can't do the same with a simple variable         \
     * definition because the next use of `match99` will define this variable again, unlike `for`  \
     * which creates a new scope. */                                                               \
    for (const void *datatype99_priv_match_expr = (const void *)&(val);                            \
         datatype99_priv_match_expr != NULL;                                                       \
         datatype99_priv_match_expr = NULL)                                                        \
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
        METALANG99_whenOrConsumePlain(                                                             \
            DATATYPE99_PRIV_isNonEmptyVariant(__VA_ARGS__),                                        \
            DATATYPE99_PRIV_ofNonEmpty)                                                            \
        (__VA_ARGS__)

#define DATATYPE99_PRIV_ofNonEmpty(tag, ...)                                                       \
    METALANG99_eval(METALANG99_variadicsMapI(                                                      \
        METALANG99_appl(v(DATATYPE99_PRIV_genBoundedVar), v(tag)),                                 \
        v(__VA_ARGS__)))

#define DATATYPE99_PRIV_genBoundedVar_IMPL(tag_, x, i)                                             \
    v(for (tag_##_##i *x = &((tag_##SumT *)datatype99_priv_match_expr)->data.tag_._##i; x != NULL; \
           x = NULL))

#define otherwise99                                                                                \
    break;                                                                                         \
    default:                                                                                       \
        DATATYPE99_PRIV_GCC_PRAGMA("GCC diagnostic pop")

#define matches99(val, tag_) ((val).tag == tag_##Tag)

// Desugaring {
#define DATATYPE99_PRIV_genCtorAux(name, tag_, params, ...)                                        \
    METALANG99_call(DATATYPE99_PRIV_genCtorAux, name tag_ params __VA_ARGS__)

#define DATATYPE99_PRIV_mapVariants(f, ...)                                                        \
    METALANG99_call(DATATYPE99_PRIV_mapVariants, f __VA_ARGS__)
#define DATATYPE99_PRIV_mapVariantsCommaSep(f, ...)                                                \
    METALANG99_call(DATATYPE99_PRIV_mapVariantsCommaSep, f __VA_ARGS__)
// }

// Implementation {

// Generate type definitions for variants {
#define DATATYPE99_PRIV_genTypedefs(name, ...)                                                     \
    typedef struct name name;                                                                      \
                                                                                                   \
    METALANG99_eval(DATATYPE99_PRIV_mapVariants(                                                   \
        METALANG99_callTrivial(METALANG99_appl, DATATYPE99_PRIV_genTypedefsMap, name),             \
        v(__VA_ARGS__)))

#define DATATYPE99_PRIV_genTypedefsMap_IMPL(name, ...)                                             \
    METALANG99_whenOrConsumePlain(                                                                 \
        DATATYPE99_PRIV_isNonEmptyVariant(__VA_ARGS__),                                            \
        DATATYPE99_PRIV_genTypedefsMapAux)                                                         \
    (name, __VA_ARGS__)                                                                            \
                                                                                                   \
        /* typedef struct <datatype-name> <variant-name>SumT; */                                   \
        v(typedef struct name METALANG99_catPlain(DATATYPE99_PRIV_extractTag(__VA_ARGS__), SumT);)

#define DATATYPE99_PRIV_genTypedefsMapAux(name, tag, ...)                                          \
    DATATYPE99_PRIV_genStructOfVariantFields(name, tag, __VA_ARGS__)                               \
    DATATYPE99_PRIV_genTypedefsToFields(tag, __VA_ARGS__)

/*
 * typedef struct <datatype-name><variant-name> {
 *     <type>0 _0;
 *     ...
 *     <type>N _N;
 * } <datatype-name><variant-name>;
 */
#define DATATYPE99_PRIV_genStructOfVariantFields(name, tag, ...)                                   \
    v(typedef struct name##tag)                                                                    \
    METALANG99_braced(                                                                             \
        METALANG99_variadicsMapI(v(DATATYPE99_PRIV_genStructOfVariantFieldsMap), v(__VA_ARGS__)))  \
    v(name##tag;)
#define DATATYPE99_PRIV_genStructOfVariantFieldsMap_IMPL(field_type, i) v(field_type _##i;)

/*
 * typedef <type>0 <variant-name>_0;
 * ...
 * typedef <type>N <variant-name>_N;
 */
#define DATATYPE99_PRIV_genTypedefsToFields(tag, ...)                                              \
    METALANG99_variadicsMapI(                                                                      \
        METALANG99_callTrivial(METALANG99_appl, DATATYPE99_PRIV_genTypedefToField, tag),           \
        v(__VA_ARGS__))
#define DATATYPE99_PRIV_genTypedefToField_IMPL(tag, field_type, i) v(typedef field_type tag##_##i;)
// }

// Generate tags of variants {

// <variant-name>0Tag, ..., <variant-name>NTag
#define DATATYPE99_PRIV_genTags(...)                                                               \
    METALANG99_eval(                                                                               \
        DATATYPE99_PRIV_mapVariantsCommaSep(v(DATATYPE99_PRIV_genTagsMap), v(__VA_ARGS__)))

#define DATATYPE99_PRIV_genTagsMap_IMPL(...)                                                       \
    v(METALANG99_catPlain(DATATYPE99_PRIV_extractTag(__VA_ARGS__), Tag))
// }

// Generate a union of fields of possible data {

/*
 * <datatype-name><variant-name>0 <variant-name>0;
 * ...
 * <datatype-name><variant-name>N <variant-name>N;
 */
#define DATATYPE99_PRIV_genUnionFields(name, ...)                                                  \
    METALANG99_eval(DATATYPE99_PRIV_mapVariants(                                                   \
        METALANG99_callTrivial(METALANG99_appl, DATATYPE99_PRIV_genUnionFieldsMap, name),          \
        v(__VA_ARGS__)))

#define DATATYPE99_PRIV_genUnionFieldsMap_IMPL(name, ...)                                          \
    v(METALANG99_whenOrConsumePlain(                                                               \
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
#define DATATYPE99_PRIV_genCtors(name, ...)                                                        \
    METALANG99_eval(DATATYPE99_PRIV_mapVariants(                                                   \
        METALANG99_callTrivial(METALANG99_appl, DATATYPE99_PRIV_genCtorsMap, name),                \
        v(__VA_ARGS__)))

#define DATATYPE99_PRIV_genCtorsMap_IMPL(name, ...)                                                \
    METALANG99_ifPlain(                                                                            \
        DATATYPE99_PRIV_isEmptyVariant(__VA_ARGS__),                                               \
        DATATYPE99_PRIV_genEmptyCtor,                                                              \
        DATATYPE99_PRIV_genNonEmptyCtor)                                                           \
    (name, __VA_ARGS__)

#define DATATYPE99_PRIV_genEmptyCtor(name, tag_)                                                   \
    v(inline static name tag_(void) { return ((name){.tag = tag_##Tag, .data.dummy = '\0'}); })

#define DATATYPE99_PRIV_genNonEmptyCtor(name, tag, ...)                                            \
    DATATYPE99_PRIV_genCtorAux(                                                                    \
        v(name),                                                                                   \
        v(tag),                                                                                    \
        METALANG99_parenthesise(DATATYPE99_PRIV_genCtorParams(__VA_ARGS__)),                       \
        DATATYPE99_PRIV_genCtorParamNames(__VA_ARGS__))

#define DATATYPE99_PRIV_genCtorParams(...)                                                         \
    METALANG99_variadicsMapICommaSep(v(DATATYPE99_PRIV_genCtorParamsMap), v(__VA_ARGS__))
#define DATATYPE99_PRIV_genCtorParamsMap_IMPL(type, i) v(type _##i)

#define DATATYPE99_PRIV_genCtorParamNames(...)                                                     \
    METALANG99_repeat(                                                                             \
        v(DATATYPE99_PRIV_genCtorParamNamesMap),                                                   \
        METALANG99_variadicsCount(v(__VA_ARGS__)))
#define DATATYPE99_PRIV_genCtorParamNamesMap_IMPL(i) v(_##i, )

#define DATATYPE99_PRIV_genCtorAux_IMPL(name, tag_, params, ...)                                   \
    v(inline static name tag_ params {                                                             \
        return ((name){.tag = tag_##Tag, .data.tag_ = {__VA_ARGS__}});                             \
    })
// }

// Auxiliary stuff {
#define DATATYPE99_PRIV_isEmptyVariant(...)                                                        \
    METALANG99_uintEqPlain(METALANG99_variadicsCountPlain(__VA_ARGS__), 1)

#define DATATYPE99_PRIV_isNonEmptyVariant(...)                                                     \
    METALANG99_notPlain(DATATYPE99_PRIV_isEmptyVariant(__VA_ARGS__))

#define DATATYPE99_PRIV_extractTag METALANG99_variadicsHeadPlain

#define DATATYPE99_PRIV_mapVariants_IMPL(f, ...)                                                   \
    METALANG99_variadicsMap(                                                                       \
        METALANG99_callTrivial(METALANG99_compose, f, METALANG99_unparenthesise),                  \
        v(__VA_ARGS__))

#define DATATYPE99_PRIV_mapVariantsCommaSep_IMPL(f, ...)                                           \
    METALANG99_variadicsMapCommaSep(                                                               \
        METALANG99_callTrivial(METALANG99_compose, f, METALANG99_unparenthesise),                  \
        v(__VA_ARGS__))

#if defined(__GNUC__) && !defined(__clang__)
#define DATATYPE99_PRIV_GCC_PRAGMA(str) _Pragma(str)
#else
#define DATATYPE99_PRIV_GCC_PRAGMA(str)
#endif
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
