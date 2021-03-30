#include <datatype99.h>

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define DATATYPE99_DERIVE_metadata_IMPL(name, variants, ...)                                       \
    ML99_TERMS(                                                                                    \
        ML99_call(VARIANTS_METADATA, v(name, variants)),                                           \
        ML99_call(METADATA_TEMPLATE, v(name), ML99_listLen(v(variants))))

#define VARIANTS_METADATA_IMPL(name, variants)                                                     \
    ML99_call(                                                                                     \
        VARIANTS_METADATA_TEMPLATE,                                                                \
        v(name),                                                                                   \
        ML99_listMapInPlace(                                                                       \
            ML99_compose(ML99_appl(v(GEN_VARIANT), v(name)), v(ML99_untuple)),                     \
            v(variants)))

#define GEN_VARIANT_IMPL(name_, tag, sig)                                                          \
    ML99_call(VARIANT_TEMPLATE, v(name_), v(tag), ML99_listLen(v(sig)))
#define GEN_VARIANT_ARITY 2

#define VARIANT_TEMPLATE_IMPL(name_, tag, arity_)                                                  \
    v({.name = #tag, .arity = arity_, .size = sizeof(name_##tag)}, )

#define VARIANTS_METADATA_TEMPLATE_IMPL(name, ...)                                                 \
    v(static const VariantMetadata name##_variants_metadata[] = {__VA_ARGS__};)

#define METADATA_TEMPLATE_IMPL(name_, variants_count_)                                             \
    v(static const DatatypeMetadata name_##_metadata = {                                           \
          .name = #name_,                                                                          \
          .variants = (const VariantMetadata *)&name_##_variants_metadata,                         \
          .variants_count = variants_count_,                                                       \
      };)

typedef struct {
    const char *name;
    size_t arity;
    size_t size;
} VariantMetadata;

typedef struct {
    const char *name;
    const VariantMetadata *variants;
    size_t variants_count;
} DatatypeMetadata;

// clang-format off
datatype(
    derive((metadata, ())),
    Num,
    (Char, char),
    (Int, int),
    (Double, double)
);
// clang-format on

/*
The generated metadata:

static const VariantMetadata Num_variants_metadata[] = {
    {.name = "Char", .arity = 1, .size = sizeof(NumChar)},
    {.name = "Int", .arity = 1, .size = sizeof(NumInt)},
    {.name = "Double", .arity = 1, .size = sizeof(NumDouble)},
};

static const DatatypeMetadata Num_metadata = {
    .name = "Num",
    .variants = (const VariantMetadata *)&Num_variants_metadata,
    .variants_count = 3,
};
*/

int main(void) {

#define CHECK(idx, name_, arity_, size_)                                                           \
    assert(strcmp(Num_metadata.variants[idx].name, name_) == 0);                                   \
    assert(Num_metadata.variants[idx].arity == arity_);                                            \
    assert(Num_metadata.variants[idx].size == size_)

    CHECK(0, "Char", 1, sizeof(char));
    CHECK(1, "Int", 1, sizeof(int));
    CHECK(2, "Double", 1, sizeof(double));

#undef CHECK

    assert(strcmp(Num_metadata.name, "Num") == 0);
    assert(3 == Num_metadata.variants_count);
}
