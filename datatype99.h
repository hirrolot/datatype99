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
        DATATYPE99_PRIV_parseMapValid,                                                             \
        DATATYPE99_PRIV_emitUnparenthesizedVariantError)(variant)

#define DATATYPE99_PRIV_parseMapValid(variant)                                                     \
    DATATYPE99_PRIV_parseMapValidAux(METALANG99_unparenthesizePlain(variant))

#define DATATYPE99_PRIV_parseMapValidAux(...)                                                      \
    METALANG99_ifPlain(                                                                            \
        DATATYPE99_PRIV_isEmptyVariantPlain(__VA_ARGS__),                                          \
        DATATYPE99_PRIV_parseMapValidAuxEmptyVariant,                                              \
        DATATYPE99_PRIV_parseMapValidAuxNonEmptyVariant)(__VA_ARGS__)

#define DATATYPE99_PRIV_parseMapValidAuxEmptyVariant(tag)                                          \
    DATATYPE99_PRIV_variant(v(tag), METALANG99_nil())

#define DATATYPE99_PRIV_parseMapValidAuxNonEmptyVariant(tag, ...)                                  \
    DATATYPE99_PRIV_variant(v(tag), METALANG99_list(v(__VA_ARGS__)))

#define DATATYPE99_PRIV_emitUnparenthesizedVariantError(variant)                                   \
    METALANG99_fatal(datatype99, variant is unparenthesized)
// } (Parsing)

// A variant representation {
#define DATATYPE99_PRIV_variant(tag, variant_params) METALANG99_parenthesize(tag, variant_params)

#define DATATYPE99_PRIV_variantTag    METALANG99_parenthesizedVariadicsHead
#define DATATYPE99_PRIV_variantParams METALANG99_parenthesizedVariadicsTail

#define DATATYPE99_PRIV_isEmptyVariant(variant)                                                    \
    METALANG99_isNil(DATATYPE99_PRIV_variantParams(variant))
#define DATATYPE99_PRIV_isNonEmptyVariant(variant)                                                 \
    METALANG99_not(DATATYPE99_PRIV_isEmptyVariant(variant))
// } (A variant representation)

// Sum type generation {
#define datatype99(name, ...)                                                                      \
    METALANG99_eval(METALANG99_call(                                                               \
        DATATYPE99_PRIV_genDatatypeAux,                                                            \
        v(name),                                                                                   \
        METALANG99_callTrivial(DATATYPE99_PRIV_parse, __VA_ARGS__)))                               \
                                                                                                   \
    METALANG99_semicolon()

#define DATATYPE99_PRIV_genDatatypeAux_IMPL(name, ast)                                             \
    METALANG99_call(                                                                               \
        DATATYPE99_PRIV_genDatatypeAuxAux,                                                         \
        v(name),                                                                                   \
        DATATYPE99_PRIV_genTypedefs(name, ast),                                                    \
        METALANG99_parenthesize(DATATYPE99_PRIV_genTags(ast)),                                     \
        DATATYPE99_PRIV_genUnionFields(name, ast),                                                 \
        DATATYPE99_PRIV_genCtors(name, ast))

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

// Generate type definitions {
#define DATATYPE99_PRIV_genTypedefs(name, ast)                                                     \
    METALANG99_listUnwrap(                                                                         \
        METALANG99_listMap(METALANG99_appl(v(DATATYPE99_PRIV_genTypedefsMap), v(name)), v(ast)))

#define DATATYPE99_PRIV_genTypedefsMap_IMPL(name, variant)                                         \
    METALANG99_call(                                                                               \
        DATATYPE99_PRIV_genTypedefsMapAux,                                                         \
        v(name),                                                                                   \
        DATATYPE99_PRIV_variantTag(v(variant)),                                                    \
        DATATYPE99_PRIV_variantParams(v(variant)))

#define DATATYPE99_PRIV_genTypedefsMapAux_IMPL(name, tag, variant_params)                          \
    METALANG99_call(                                                                               \
        DATATYPE99_PRIV_genTypedefsTemplate,                                                       \
        v(name, tag),                                                                              \
        METALANG99_callTrivial(                                                                    \
            DATATYPE99_PRIV_genStructOfVariantFields,                                              \
            name,                                                                                  \
            tag,                                                                                   \
            variant_params),                                                                       \
        METALANG99_callTrivial(DATATYPE99_PRIV_genTypedefsToFields, tag, variant_params))

#define DATATYPE99_PRIV_genTypedefsTemplate_IMPL(                                                  \
    name,                                                                                          \
    tag,                                                                                           \
    struct_of_variant_fields,                                                                      \
    typedefs_to_fields)                                                                            \
    v(typedef struct name tag##SumT; struct_of_variant_fields typedefs_to_fields)

/*
 * typedef struct <datatype-name><variant-name> {
 *     <type>0 _0;
 *     ...
 *     <type>N _N;
 * } <datatype-name><variant-name>;
 */
#define DATATYPE99_PRIV_genStructOfVariantFields_IMPL(name, tag, variant_params)                   \
    METALANG99_call(                                                                               \
        METALANG99_whenLazy(                                                                       \
            METALANG99_isCons(v(variant_params)),                                                  \
            v(DATATYPE99_PRIV_genStructOfVariantFieldsNonEmpty)),                                  \
        v(name, tag, variant_params))

#define DATATYPE99_PRIV_genStructOfVariantFieldsNonEmpty_IMPL(name, tag, variant_params)           \
    METALANG99_call(                                                                               \
        DATATYPE99_PRIV_genStructOfVariantFieldsTemplate,                                          \
        v(name, tag),                                                                              \
        METALANG99_indexedFields(v(variant_params)))

#define DATATYPE99_PRIV_genStructOfVariantFieldsTemplate_IMPL(name, tag, fields)                   \
    v(typedef struct name##tag{fields} name##tag;)

/*
 * typedef <type>0 <variant-name>_0;
 * ...
 * typedef <type>N <variant-name>_N;
 */
#define DATATYPE99_PRIV_genTypedefsToFields_IMPL(tag, variant_params)                              \
    METALANG99_call(                                                                               \
        METALANG99_whenLazy(                                                                       \
            METALANG99_isCons(v(variant_params)),                                                  \
            v(DATATYPE99_PRIV_genTypedefsToFieldsNonEmpty)),                                       \
        v(tag, variant_params))

#define DATATYPE99_PRIV_genTypedefsToFieldsNonEmpty_IMPL(tag, variant_params)                      \
    METALANG99_listUnwrap(METALANG99_listMapI(                                                     \
        METALANG99_appl(v(DATATYPE99_PRIV_genTypedefToField), v(tag)),                             \
        v(variant_params)))
#define DATATYPE99_PRIV_genTypedefToField_IMPL(tag, type, i) v(typedef type tag##_##i;)
// } (Generate type definitions)

// Generate tags of variants {

// <variant-name>0Tag, ..., <variant-name>NTag
#define DATATYPE99_PRIV_genTags(ast)                                                               \
    METALANG99_listUnwrapCommaSep(METALANG99_listMap(v(DATATYPE99_PRIV_genTagsMap), v(ast)))

#define DATATYPE99_PRIV_genTagsMap_IMPL(variant)                                                   \
    METALANG99_cat(DATATYPE99_PRIV_variantTag(v(variant)), v(Tag))
// } (Generate tags of variants)

// Generate a union of possible alternatives {

/*
 * <datatype-name><variant-name>0 <variant-name>0;
 * ...
 * <datatype-name><variant-name>N <variant-name>N;
 */
#define DATATYPE99_PRIV_genUnionFields(name, ast)                                                  \
    METALANG99_listUnwrap(METALANG99_listMap(                                                      \
        METALANG99_appl(v(DATATYPE99_PRIV_genUnionFieldsMap), v(name)),                            \
        v(ast)))

#define DATATYPE99_PRIV_genUnionFieldsMap_IMPL(name, variant)                                      \
    METALANG99_call(                                                                               \
        METALANG99_whenLazy(                                                                       \
            DATATYPE99_PRIV_isNonEmptyVariant(v(variant)),                                         \
            v(DATATYPE99_PRIV_genUnionField)),                                                     \
        v(name),                                                                                   \
        DATATYPE99_PRIV_variantTag(v(variant)))

#define DATATYPE99_PRIV_genUnionField_IMPL(name, tag) v(name##tag tag;)
// } (Generate a union of possible alternatives)

// Generate value constructors {

/*
 * inline static <datatype99-name> <variant-name>0(...) { ... }
 * ...
 * inline static <datatype99-name> <variant-name>N(...) { ... }
 */
#define DATATYPE99_PRIV_genCtors(name, ast)                                                        \
    METALANG99_listUnwrap(                                                                         \
        METALANG99_listMap(METALANG99_appl(v(DATATYPE99_PRIV_genCtorsMap), v(name)), v(ast)))

#define DATATYPE99_PRIV_genCtorsMap_IMPL(name, variant)                                            \
    METALANG99_call(                                                                               \
        METALANG99_if(                                                                             \
            DATATYPE99_PRIV_isEmptyVariant(v(variant)),                                            \
            v(DATATYPE99_PRIV_genCtorEmpty),                                                       \
            v(DATATYPE99_PRIV_genCtorNonEmpty)),                                                   \
        METALANG99_unparenthesize(v(variant)),                                                     \
        v(name))

#define DATATYPE99_PRIV_genCtorEmpty_IMPL(tag_, _variant_params, name)                             \
    v(inline static DATATYPE99_PRIV_WARN_UNUSED_RESULT DATATYPE99_PRIV_CONST name tag_(void) {     \
        return ((name){.tag = tag_##Tag, .data.dummy = '\0'});                                     \
    })

#define DATATYPE99_PRIV_genCtorNonEmpty_IMPL(tag, variant_params, name)                            \
    METALANG99_call(                                                                               \
        DATATYPE99_PRIV_genCtorTemplate,                                                           \
        v(name, tag),                                                                              \
        METALANG99_indexedParams(v(variant_params)),                                               \
        METALANG99_indexedInitializerList(METALANG99_listLen(v(variant_params))))

#define DATATYPE99_PRIV_genCtorTemplate_IMPL(name, tag_, params, ...)                              \
    v(inline static DATATYPE99_PRIV_WARN_UNUSED_RESULT DATATYPE99_PRIV_CONST name tag_ params {    \
        return ((name){.tag = tag_##Tag, .data.tag_ = __VA_ARGS__});                               \
    })
// } (Generate value constructors)

// Auxiliary stuff {
#define DATATYPE99_PRIV_isEmptyVariantPlain(...)                                                   \
    METALANG99_uintEqPlain(METALANG99_variadicsCountPlain(__VA_ARGS__), 1)
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
// } (Compiler-specific stuff)

// Arity specifiers {
#define DATATYPE99_PRIV_parseMap_ARITY                    1
#define DATATYPE99_PRIV_genBinding_ARITY                  3
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
