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
    EPILEPSY_eval(DATATYPE99_PRIV_GEN_TYPEDEFS(v(name), v(__VA_ARGS__)))                           \
                                                                                                   \
        typedef struct name {                                                                      \
        enum { EPILEPSY_eval(DATATYPE99_PRIV_GEN_TAGS(v(name), v(__VA_ARGS__))) } tag;             \
                                                                                                   \
        union {                                                                                    \
            char dummy;                                                                            \
            EPILEPSY_eval(DATATYPE99_PRIV_GEN_UNION_FIELDS(v(name), v(__VA_ARGS__)))               \
        } data;                                                                                    \
    } name;                                                                                        \
                                                                                                   \
    EPILEPSY_eval(DATATYPE99_PRIV_GEN_CONSTRUCTORS(v(name), v(__VA_ARGS__)))                       \
                                                                                                   \
        EPILEPSY_semicolon()

#define match99(val, ...)                                                                          \
    for (const void *datatype99_priv_match_expr = (const void *)&(val);                            \
         datatype99_priv_match_expr != NULL;                                                       \
         datatype99_priv_match_expr = NULL)                                                        \
                                                                                                   \
        switch ((val).tag)

#define of99(tag, ...)                                                                             \
    break;                                                                                         \
    case tag##Tag:                                                                                 \
        EPILEPSY_eval(EPILEPSY_variadicsMapI(                                                      \
            EPILEPSY_callTrivial(EPILEPSY_appl, DATATYPE99_PRIV_GEN_BOUNDED_VAR, tag),             \
            v(__VA_ARGS__)))

#define DATATYPE99_PRIV_GEN_BOUNDED_VAR_IMPL(tag_, x, i)                                           \
    v(for (tag_##i *x = &((tag_##ChoiceT *)datatype99_priv_match_expr)->data.tag_._##i; x != NULL; \
           x = NULL))

#define otherwise99                                                                                \
    break;                                                                                         \
    default:

#define matches99(val, tag_) ((val).tag == tag_##Tag)

// Desugaring {
#define DATATYPE99_PRIV_GEN_TYPEDEFS(name, ...)                                                    \
    EPILEPSY_call(DATATYPE99_PRIV_GEN_TYPEDEFS, name __VA_ARGS__)
#define DATATYPE99_PRIV_GEN_TAGS(name, ...)                                                        \
    EPILEPSY_call(DATATYPE99_PRIV_GEN_TAGS, name __VA_ARGS__)
#define DATATYPE99_PRIV_GEN_UNION_FIELDS(name, ...)                                                \
    EPILEPSY_call(DATATYPE99_PRIV_GEN_UNION_FIELDS, name __VA_ARGS__)
#define DATATYPE99_PRIV_GEN_CONSTRUCTORS(name, ...)                                                \
    EPILEPSY_call(DATATYPE99_PRIV_GEN_CONSTRUCTORS, name __VA_ARGS__)

#define DATATYPE99_PRIV_MAP_VARIANTS(f, ...)                                                       \
    EPILEPSY_call(DATATYPE99_PRIV_MAP_VARIANTS, f __VA_ARGS__)
#define DATATYPE99_PRIV_MAP_VARIANTS_COMMA_SEP(f, ...)                                             \
    EPILEPSY_call(DATATYPE99_PRIV_MAP_VARIANTS_COMMA_SEP, f __VA_ARGS__)
// }

// Implementation {

// Generate type definitions for variants {
#define DATATYPE99_PRIV_GEN_TYPEDEFS_IMPL(name, ...)                                               \
    DATATYPE99_PRIV_MAP_VARIANTS(                                                                  \
        EPILEPSY_callTrivial(EPILEPSY_appl, DATATYPE99_PRIV_GEN_TYPEDEFS_MAP, name),               \
        v(__VA_ARGS__))

#define DATATYPE99_PRIV_GEN_TYPEDEFS_MAP_IMPL(name, ...)                                           \
    EPILEPSY_call(                                                                                 \
        EPILEPSY_ifPlain(                                                                          \
            DATATYPE99_PRIV_IS_EMPTY_VARIANT(__VA_ARGS__),                                         \
            EPILEPSY_consume,                                                                      \
            DATATYPE99_PRIV_GEN_TYPEDEFS_MAP_AUX),                                                 \
        v(name, __VA_ARGS__))

#define DATATYPE99_PRIV_GEN_TYPEDEFS_MAP_AUX_IMPL(name, tag, ...)                                  \
    EPILEPSY_call(                                                                                 \
        DATATYPE99_PRIV_GEN_TYPEDEFS_MAP_AUX_AUX,                                                  \
        v(name, tag) EPILEPSY_variadicsMapI(v(DATATYPE99_PRIV_GEN_VARIANT_FIELD), v(__VA_ARGS__))  \
            EPILEPSY_variadicsMapI(                                                                \
                EPILEPSY_callTrivial(EPILEPSY_appl, DATATYPE99_PRIV_GEN_FIELD_TYPEDEF, tag),       \
                v(__VA_ARGS__)))

#define DATATYPE99_PRIV_GEN_TYPEDEFS_MAP_AUX_AUX_IMPL(name, tag, fields, field_typedefs)           \
    v(typedef struct {fields} name##tag; field_typedefs typedef struct name tag##ChoiceT;)

#define DATATYPE99_PRIV_GEN_VARIANT_FIELD_IMPL(x, i)      v(x _##i;)
#define DATATYPE99_PRIV_GEN_FIELD_TYPEDEF_IMPL(tag, x, i) v(typedef x tag##i;)
// }

// Generate tags of variants {
#define DATATYPE99_PRIV_GEN_TAGS_IMPL(name, ...)                                                   \
    DATATYPE99_PRIV_MAP_VARIANTS_COMMA_SEP(                                                        \
        EPILEPSY_callTrivial(EPILEPSY_appl, DATATYPE99_PRIV_GEN_TAGS_MAP, name),                   \
        v(__VA_ARGS__))

#define DATATYPE99_PRIV_GEN_TAGS_MAP_IMPL(name, tag, ...) v(tag##Tag)
// }

// Generate a union of fields of possible data {
#define DATATYPE99_PRIV_GEN_UNION_FIELDS_IMPL(name, ...)                                           \
    DATATYPE99_PRIV_MAP_VARIANTS(                                                                  \
        EPILEPSY_callTrivial(EPILEPSY_appl, DATATYPE99_PRIV_GEN_UNION_FIELDS_MAP, name),           \
        v(__VA_ARGS__))

#define DATATYPE99_PRIV_GEN_UNION_FIELDS_MAP_IMPL(name, ...)                                       \
    v(EPILEPSY_ifPlain(                                                                            \
        DATATYPE99_PRIV_IS_EMPTY_VARIANT(__VA_ARGS__),                                             \
        EPILEPSY_consumePlain,                                                                     \
        DATATYPE99_PRIV_GEN_UNION_FIELD)(name, __VA_ARGS__))

#define DATATYPE99_PRIV_GEN_UNION_FIELD(name, tag, ...) name##tag tag;
// }

// Generate value constructors {
#define DATATYPE99_PRIV_GEN_CONSTRUCTORS_IMPL(name, ...)                                           \
    DATATYPE99_PRIV_MAP_VARIANTS(                                                                  \
        EPILEPSY_callTrivial(EPILEPSY_appl, DATATYPE99_PRIV_GEN_CONSTRUCTORS_MAP, name),           \
        v(__VA_ARGS__))

#define DATATYPE99_PRIV_GEN_CONSTRUCTORS_MAP_IMPL(name, ...)                                       \
    EPILEPSY_ifPlain(                                                                              \
        DATATYPE99_PRIV_IS_EMPTY_VARIANT(__VA_ARGS__),                                             \
        DATATYPE99_PRIV_GEN_CONSTRUCTOR_1,                                                         \
        DATATYPE99_PRIV_GEN_CONSTRUCTOR_2)(name, __VA_ARGS__)

#define DATATYPE99_PRIV_GEN_CONSTRUCTOR_1(name, tag_)                                              \
    v(inline static name tag_(void) { return ((name){.tag = tag_##Tag, .data.dummy = '\0'}); })

#define DATATYPE99_PRIV_GEN_CONSTRUCTOR_2(name, tag, ...)                                          \
    EPILEPSY_call(                                                                                 \
        DATATYPE99_PRIV_GEN_CONSTRUCTOR,                                                           \
        v(name, tag) EPILEPSY_parenthesise(                                                        \
            EPILEPSY_variadicsMapICommaSep(v(DATATYPE99_PRIV_GEN_PARAM), v(__VA_ARGS__)))          \
            EPILEPSY_variadicsMapICommaSep(v(DATATYPE99_PRIV_GEN_PARAM_NAME), v(__VA_ARGS__)))

#define DATATYPE99_PRIV_GEN_CONSTRUCTOR_IMPL(name, tag_, params, ...)                              \
    v(inline static name tag_ params {                                                             \
        return ((name){.tag = tag_##Tag, .data.tag_ = {__VA_ARGS__}});                             \
    })

#define DATATYPE99_PRIV_GEN_PARAM_IMPL(x, i)       v(x _##i)
#define DATATYPE99_PRIV_GEN_PARAM_NAME_IMPL(_x, i) v(_##i)
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
#define DATATYPE99_PRIV_GEN_TYPEDEFS_MAP_ARITY     2
#define DATATYPE99_PRIV_GEN_TAGS_MAP_ARITY         2
#define DATATYPE99_PRIV_GEN_UNION_FIELDS_MAP_ARITY 2
#define DATATYPE99_PRIV_GEN_CONSTRUCTORS_MAP_ARITY 2
#define DATATYPE99_PRIV_GEN_BOUNDED_VAR_ARITY      3
#define DATATYPE99_PRIV_GEN_VARIANT_FIELD_ARITY    2
#define DATATYPE99_PRIV_GEN_FIELD_TYPEDEF_ARITY    3
#define DATATYPE99_PRIV_GEN_PARAM_ARITY            2
#define DATATYPE99_PRIV_GEN_PARAM_NAME_ARITY       2
// }

#endif // DATATYPE99_H
