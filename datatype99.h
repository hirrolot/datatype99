#ifndef DATATYPE99_H
#define DATATYPE99_H

#include <stddef.h>

#include <epilepsy.h>

#ifndef DATATYPE99_NO_ALIASES

#define datatype  datatype99
#define match     match99
#define matches   matches99
#define of        of99
#define otherwise otherwise99

#endif // DATATYPE99_NO_ALIASES

#define datatype99(name, ...)                                                                      \
    DATATYPE99_PRIV_GEN_TYPEDEFS(name, __VA_ARGS__)                                                \
                                                                                                   \
    typedef struct name {                                                                          \
        enum { DATATYPE99_PRIV_GEN_TAGS(__VA_ARGS__) } tag;                                        \
                                                                                                   \
        union {                                                                                    \
            char dummy;                                                                            \
            DATATYPE99_PRIV_GEN_UNION_FIELDS(name, __VA_ARGS__)                                    \
        } data;                                                                                    \
    } name;                                                                                        \
                                                                                                   \
    DATATYPE99_PRIV_GEN_CTORS(name, __VA_ARGS__)                                                   \
    EPILEPSY_semicolon()

#define match99(val)                                                                               \
    _Pragma("GCC diagnostic push")                                                                 \
    _Pragma("GCC diagnostic ignored \"-Wmisleading-indentation\"")                                 \
    _Pragma("GCC diagnostic ignored \"-Wreturn-type\"")                                            \
                                                                                                   \
    for (const void *datatype99_priv_match_expr = (const void *)&(val);                            \
         datatype99_priv_match_expr != NULL;                                                       \
         datatype99_priv_match_expr = NULL)                                                        \
                                                                                                   \
        switch ((val).tag)

#define of99(...)                                                                                  \
    break;                                                                                         \
    _Pragma("GCC diagnostic pop")                                                                  \
    _Pragma("GCC diagnostic push")                                                                 \
    _Pragma("GCC diagnostic ignored \"-Wmisleading-indentation\"")                                 \
    _Pragma("GCC diagnostic ignored \"-Wreturn-type\"")                                            \
                                                                                                   \
    EPILEPSY_ifPlain(                                                                              \
        DATATYPE99_PRIV_IS_EMPTY_VARIANT(__VA_ARGS__),                                             \
        DATATYPE99_PRIV_OF_EMPTY,                                                                  \
        DATATYPE99_PRIV_OF_NON_EMPTY)                                                              \
    (__VA_ARGS__)

#define DATATYPE99_PRIV_OF_EMPTY(tag) case tag##Tag:

#define DATATYPE99_PRIV_OF_NON_EMPTY(tag, ...)                                                     \
    case tag##Tag:                                                                                 \
        EPILEPSY_eval(EPILEPSY_variadicsMapI(                                                      \
            EPILEPSY_appl(v(DATATYPE99_PRIV_GEN_BOUNDED_VAR), v(tag)),                             \
            v(__VA_ARGS__)))

#define DATATYPE99_PRIV_GEN_BOUNDED_VAR_IMPL(tag_, x, i)                                           \
    v(for (tag_##_##i *x = &((tag_##SumT *)datatype99_priv_match_expr)->data.tag_._##i; x != NULL; \
           x = NULL))

#define otherwise99                                                                                \
    break;                                                                                         \
    default:                                                                                       \
        _Pragma("GCC diagnostic pop")

#define matches99(val, tag_) ((val).tag == tag_##Tag)

// Desugaring {
#define DATATYPE99_PRIV_GEN_CTOR_AUX(name, tag_, params, ...)                                      \
    EPILEPSY_call(DATATYPE99_PRIV_GEN_CTOR_AUX, name tag_ params __VA_ARGS__)

#define DATATYPE99_PRIV_MAP_VARIANTS(f, ...)                                                       \
    EPILEPSY_call(DATATYPE99_PRIV_MAP_VARIANTS, f __VA_ARGS__)
#define DATATYPE99_PRIV_MAP_VARIANTS_COMMA_SEP(f, ...)                                             \
    EPILEPSY_call(DATATYPE99_PRIV_MAP_VARIANTS_COMMA_SEP, f __VA_ARGS__)
// }

// Implementation {

// Generate type definitions for variants {
#define DATATYPE99_PRIV_GEN_TYPEDEFS(name, ...)                                                    \
    EPILEPSY_eval(DATATYPE99_PRIV_MAP_VARIANTS(                                                    \
        EPILEPSY_callTrivial(EPILEPSY_appl, DATATYPE99_PRIV_GEN_TYPEDEFS_MAP, name),               \
        v(__VA_ARGS__)))

#define DATATYPE99_PRIV_GEN_TYPEDEFS_MAP_IMPL(name, ...)                                           \
    EPILEPSY_call(                                                                                 \
        EPILEPSY_ifPlain(                                                                          \
            DATATYPE99_PRIV_IS_EMPTY_VARIANT(__VA_ARGS__),                                         \
            EPILEPSY_consume,                                                                      \
            DATATYPE99_PRIV_GEN_TYPEDEFS_MAP_AUX),                                                 \
        v(name, __VA_ARGS__))                                                                      \
                                                                                                   \
        v(typedef struct name EPILEPSY_catPlain(                                                   \
              EPILEPSY_variadicsHeadPlain(__VA_ARGS__, ~),                                         \
              SumT);)

#define DATATYPE99_PRIV_GEN_TYPEDEFS_MAP_AUX_IMPL(name, tag, ...)                                  \
    v(typedef struct name##tag)                                                                    \
    EPILEPSY_braced(DATATYPE99_PRIV_GEN_VARIANT_FIELDS(__VA_ARGS__))                               \
    v(name##tag;)                                                                                  \
    DATATYPE99_PRIV_GEN_TYPEDEF_TO_FIELDS(tag, __VA_ARGS__)

#define DATATYPE99_PRIV_GEN_VARIANT_FIELDS(...)                                                    \
    EPILEPSY_variadicsMapI(v(DATATYPE99_PRIV_GEN_VARIANT_FIELDS_MAP), v(__VA_ARGS__))
#define DATATYPE99_PRIV_GEN_VARIANT_FIELDS_MAP_IMPL(field_type, i) v(field_type _##i;)

#define DATATYPE99_PRIV_GEN_TYPEDEF_TO_FIELDS(tag, ...)                                            \
    EPILEPSY_variadicsMapI(                                                                        \
        EPILEPSY_callTrivial(EPILEPSY_appl, DATATYPE99_PRIV_GEN_TYPEDEF_TO_FIELDS_MAP, tag),       \
        v(__VA_ARGS__))
#define DATATYPE99_PRIV_GEN_TYPEDEF_TO_FIELDS_MAP_IMPL(tag, field_type, i)                         \
    v(typedef field_type tag##_##i;)
// }

// Generate tags of variants {
#define DATATYPE99_PRIV_GEN_TAGS(...)                                                              \
    EPILEPSY_eval(                                                                                 \
        DATATYPE99_PRIV_MAP_VARIANTS_COMMA_SEP(v(DATATYPE99_PRIV_GEN_TAGS_MAP), v(__VA_ARGS__)))

#define DATATYPE99_PRIV_GEN_TAGS_MAP_IMPL(...)                                                     \
    v(EPILEPSY_catPlain(EPILEPSY_variadicsHeadPlain(__VA_ARGS__, ~), Tag))
// }

// Generate a union of fields of possible data {
#define DATATYPE99_PRIV_GEN_UNION_FIELDS(name, ...)                                                \
    EPILEPSY_eval(DATATYPE99_PRIV_MAP_VARIANTS(                                                    \
        EPILEPSY_callTrivial(EPILEPSY_appl, DATATYPE99_PRIV_GEN_UNION_FIELDS_MAP, name),           \
        v(__VA_ARGS__)))

#define DATATYPE99_PRIV_GEN_UNION_FIELDS_MAP_IMPL(name, ...)                                       \
    v(EPILEPSY_ifPlain(                                                                            \
        DATATYPE99_PRIV_IS_EMPTY_VARIANT(__VA_ARGS__),                                             \
        EPILEPSY_consumePlain,                                                                     \
        DATATYPE99_PRIV_GEN_UNION_FIELD)(name, __VA_ARGS__))

#define DATATYPE99_PRIV_GEN_UNION_FIELD(name, tag, ...) name##tag tag;
// }

// Generate value constructors {
#define DATATYPE99_PRIV_GEN_CTORS(name, ...)                                                       \
    EPILEPSY_eval(DATATYPE99_PRIV_MAP_VARIANTS(                                                    \
        EPILEPSY_callTrivial(EPILEPSY_appl, DATATYPE99_PRIV_GEN_CTORS_MAP, name),                  \
        v(__VA_ARGS__)))

#define DATATYPE99_PRIV_GEN_CTORS_MAP_IMPL(name, ...)                                              \
    EPILEPSY_ifPlain(                                                                              \
        DATATYPE99_PRIV_IS_EMPTY_VARIANT(__VA_ARGS__),                                             \
        DATATYPE99_PRIV_GEN_EMPTY_CTOR,                                                            \
        DATATYPE99_PRIV_GEN_NON_EMPTY_CTOR)                                                        \
    (name, __VA_ARGS__)

#define DATATYPE99_PRIV_GEN_EMPTY_CTOR(name, tag_)                                                 \
    v(inline static name tag_(void) { return ((name){.tag = tag_##Tag, .data.dummy = '\0'}); })

#define DATATYPE99_PRIV_GEN_NON_EMPTY_CTOR(name, tag, ...)                                         \
    DATATYPE99_PRIV_GEN_CTOR_AUX(                                                                  \
        v(name),                                                                                   \
        v(tag),                                                                                    \
        EPILEPSY_parenthesise(DATATYPE99_PRIV_GEN_CTOR_PARAMS(__VA_ARGS__)),                       \
        DATATYPE99_PRIV_GEN_CTOR_PARAM_NAMES(__VA_ARGS__))

#define DATATYPE99_PRIV_GEN_CTOR_PARAMS(...)                                                       \
    EPILEPSY_variadicsMapICommaSep(v(DATATYPE99_PRIV_GEN_CTOR_PARAMS_MAP), v(__VA_ARGS__))
#define DATATYPE99_PRIV_GEN_CTOR_PARAMS_MAP_IMPL(type, i) v(type _##i)

#define DATATYPE99_PRIV_GEN_CTOR_PARAM_NAMES(...)                                                  \
    EPILEPSY_repeat(                                                                               \
        v(DATATYPE99_PRIV_GEN_CTOR_PARAM_NAMES_MAP),                                               \
        EPILEPSY_variadicsCount(v(__VA_ARGS__)))
#define DATATYPE99_PRIV_GEN_CTOR_PARAM_NAMES_MAP_IMPL(i) v(_##i, )

#define DATATYPE99_PRIV_GEN_CTOR_AUX_IMPL(name, tag_, params, ...)                                 \
    v(inline static name tag_ params {                                                             \
        return ((name){.tag = tag_##Tag, .data.tag_ = {__VA_ARGS__}});                             \
    })
// }

#define DATATYPE99_PRIV_IS_EMPTY_VARIANT(...)                                                      \
    EPILEPSY_uintEqPlain(EPILEPSY_variadicsCountPlain(__VA_ARGS__), 1)

#define DATATYPE99_PRIV_MAP_VARIANTS_IMPL(f, ...)                                                  \
    EPILEPSY_variadicsMap(                                                                         \
        EPILEPSY_callTrivial(EPILEPSY_compose, f, EPILEPSY_unparenthesise),                        \
        v(__VA_ARGS__))

#define DATATYPE99_PRIV_MAP_VARIANTS_COMMA_SEP_IMPL(f, ...)                                        \
    EPILEPSY_variadicsMapCommaSep(                                                                 \
        EPILEPSY_callTrivial(EPILEPSY_compose, f, EPILEPSY_unparenthesise),                        \
        v(__VA_ARGS__))
// } (Implementation)

// Arity specifiers {
#define DATATYPE99_PRIV_GEN_TYPEDEFS_MAP_ARITY          2
#define DATATYPE99_PRIV_GEN_TAGS_MAP_ARITY              1
#define DATATYPE99_PRIV_GEN_UNION_FIELDS_MAP_ARITY      2
#define DATATYPE99_PRIV_GEN_CTORS_MAP_ARITY             2
#define DATATYPE99_PRIV_GEN_BOUNDED_VAR_ARITY           3
#define DATATYPE99_PRIV_GEN_VARIANT_FIELDS_MAP_ARITY    2
#define DATATYPE99_PRIV_GEN_TYPEDEF_TO_FIELDS_MAP_ARITY 3
#define DATATYPE99_PRIV_GEN_CTOR_PARAMS_MAP_ARITY       2

#define DATATYPE99_PRIV_GEN_CTOR_PARAM_NAMES_MAP_ARITY 1
// }

#endif // DATATYPE99_H
