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
    METALANG99_semicolon()

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
    METALANG99_ifPlain(                                                                            \
        DATATYPE99_PRIV_IS_EMPTY_VARIANT(__VA_ARGS__),                                             \
        DATATYPE99_PRIV_OF_EMPTY,                                                                  \
        DATATYPE99_PRIV_OF_NON_EMPTY)                                                              \
    (__VA_ARGS__)

#define DATATYPE99_PRIV_OF_EMPTY(tag) case tag##Tag:

#define DATATYPE99_PRIV_OF_NON_EMPTY(tag, ...)                                                     \
    case tag##Tag:                                                                                 \
        METALANG99_eval(METALANG99_variadicsMapI(                                                  \
            METALANG99_appl(v(DATATYPE99_PRIV_GEN_BOUNDED_VAR), v(tag)),                           \
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
    METALANG99_call(DATATYPE99_PRIV_GEN_CTOR_AUX, name tag_ params __VA_ARGS__)

#define DATATYPE99_PRIV_MAP_VARIANTS(f, ...)                                                       \
    METALANG99_call(DATATYPE99_PRIV_MAP_VARIANTS, f __VA_ARGS__)
#define DATATYPE99_PRIV_MAP_VARIANTS_COMMA_SEP(f, ...)                                             \
    METALANG99_call(DATATYPE99_PRIV_MAP_VARIANTS_COMMA_SEP, f __VA_ARGS__)
// }

// Implementation {

// Generate type definitions for variants {
#define DATATYPE99_PRIV_GEN_TYPEDEFS(name, ...)                                                    \
    METALANG99_eval(DATATYPE99_PRIV_MAP_VARIANTS(                                                  \
        METALANG99_callTrivial(METALANG99_appl, DATATYPE99_PRIV_GEN_TYPEDEFS_MAP, name),           \
        v(__VA_ARGS__)))

#define DATATYPE99_PRIV_GEN_TYPEDEFS_MAP_IMPL(name, ...)                                           \
    METALANG99_call(                                                                               \
        METALANG99_ifPlain(                                                                        \
            DATATYPE99_PRIV_IS_EMPTY_VARIANT(__VA_ARGS__),                                         \
            METALANG99_consume,                                                                    \
            DATATYPE99_PRIV_GEN_TYPEDEFS_MAP_AUX),                                                 \
        v(name, __VA_ARGS__))                                                                      \
                                                                                                   \
        v(typedef struct name METALANG99_catPlain(                                                 \
              METALANG99_variadicsHeadPlain(__VA_ARGS__, ~),                                       \
              SumT);)

#define DATATYPE99_PRIV_GEN_TYPEDEFS_MAP_AUX_IMPL(name, tag, ...)                                  \
    v(typedef struct name##tag)                                                                    \
    METALANG99_braced(DATATYPE99_PRIV_GEN_VARIANT_FIELDS(__VA_ARGS__))                             \
    v(name##tag;)                                                                                  \
    DATATYPE99_PRIV_GEN_TYPEDEF_TO_FIELDS(tag, __VA_ARGS__)

#define DATATYPE99_PRIV_GEN_VARIANT_FIELDS(...)                                                    \
    METALANG99_variadicsMapI(v(DATATYPE99_PRIV_GEN_VARIANT_FIELDS_MAP), v(__VA_ARGS__))
#define DATATYPE99_PRIV_GEN_VARIANT_FIELDS_MAP_IMPL(field_type, i) v(field_type _##i;)

#define DATATYPE99_PRIV_GEN_TYPEDEF_TO_FIELDS(tag, ...)                                            \
    METALANG99_variadicsMapI(                                                                      \
        METALANG99_callTrivial(METALANG99_appl, DATATYPE99_PRIV_GEN_TYPEDEF_TO_FIELDS_MAP, tag),   \
        v(__VA_ARGS__))
#define DATATYPE99_PRIV_GEN_TYPEDEF_TO_FIELDS_MAP_IMPL(tag, field_type, i)                         \
    v(typedef field_type tag##_##i;)
// }

// Generate tags of variants {
#define DATATYPE99_PRIV_GEN_TAGS(...)                                                              \
    METALANG99_eval(                                                                               \
        DATATYPE99_PRIV_MAP_VARIANTS_COMMA_SEP(v(DATATYPE99_PRIV_GEN_TAGS_MAP), v(__VA_ARGS__)))

#define DATATYPE99_PRIV_GEN_TAGS_MAP_IMPL(...)                                                     \
    v(METALANG99_catPlain(METALANG99_variadicsHeadPlain(__VA_ARGS__, ~), Tag))
// }

// Generate a union of fields of possible data {
#define DATATYPE99_PRIV_GEN_UNION_FIELDS(name, ...)                                                \
    METALANG99_eval(DATATYPE99_PRIV_MAP_VARIANTS(                                                  \
        METALANG99_callTrivial(METALANG99_appl, DATATYPE99_PRIV_GEN_UNION_FIELDS_MAP, name),       \
        v(__VA_ARGS__)))

#define DATATYPE99_PRIV_GEN_UNION_FIELDS_MAP_IMPL(name, ...)                                       \
    v(METALANG99_ifPlain(                                                                          \
        DATATYPE99_PRIV_IS_EMPTY_VARIANT(__VA_ARGS__),                                             \
        METALANG99_consumePlain,                                                                   \
        DATATYPE99_PRIV_GEN_UNION_FIELD)(name, __VA_ARGS__))

#define DATATYPE99_PRIV_GEN_UNION_FIELD(name, tag, ...) name##tag tag;
// }

// Generate value constructors {
#define DATATYPE99_PRIV_GEN_CTORS(name, ...)                                                       \
    METALANG99_eval(DATATYPE99_PRIV_MAP_VARIANTS(                                                  \
        METALANG99_callTrivial(METALANG99_appl, DATATYPE99_PRIV_GEN_CTORS_MAP, name),              \
        v(__VA_ARGS__)))

#define DATATYPE99_PRIV_GEN_CTORS_MAP_IMPL(name, ...)                                              \
    METALANG99_ifPlain(                                                                            \
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
        METALANG99_parenthesise(DATATYPE99_PRIV_GEN_CTOR_PARAMS(__VA_ARGS__)),                     \
        DATATYPE99_PRIV_GEN_CTOR_PARAM_NAMES(__VA_ARGS__))

#define DATATYPE99_PRIV_GEN_CTOR_PARAMS(...)                                                       \
    METALANG99_variadicsMapICommaSep(v(DATATYPE99_PRIV_GEN_CTOR_PARAMS_MAP), v(__VA_ARGS__))
#define DATATYPE99_PRIV_GEN_CTOR_PARAMS_MAP_IMPL(type, i) v(type _##i)

#define DATATYPE99_PRIV_GEN_CTOR_PARAM_NAMES(...)                                                  \
    METALANG99_repeat(                                                                             \
        v(DATATYPE99_PRIV_GEN_CTOR_PARAM_NAMES_MAP),                                               \
        METALANG99_variadicsCount(v(__VA_ARGS__)))
#define DATATYPE99_PRIV_GEN_CTOR_PARAM_NAMES_MAP_IMPL(i) v(_##i, )

#define DATATYPE99_PRIV_GEN_CTOR_AUX_IMPL(name, tag_, params, ...)                                 \
    v(inline static name tag_ params {                                                             \
        return ((name){.tag = tag_##Tag, .data.tag_ = {__VA_ARGS__}});                             \
    })
// }

#define DATATYPE99_PRIV_IS_EMPTY_VARIANT(...)                                                      \
    METALANG99_uintEqPlain(METALANG99_variadicsCountPlain(__VA_ARGS__), 1)

#define DATATYPE99_PRIV_MAP_VARIANTS_IMPL(f, ...)                                                  \
    METALANG99_variadicsMap(                                                                       \
        METALANG99_callTrivial(METALANG99_compose, f, METALANG99_unparenthesise),                  \
        v(__VA_ARGS__))

#define DATATYPE99_PRIV_MAP_VARIANTS_COMMA_SEP_IMPL(f, ...)                                        \
    METALANG99_variadicsMapCommaSep(                                                               \
        METALANG99_callTrivial(METALANG99_compose, f, METALANG99_unparenthesise),                  \
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
