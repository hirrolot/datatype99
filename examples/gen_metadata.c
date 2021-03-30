#include <datatype99.h>

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define DATATYPE99_DERIVE_metadata_IMPL(name, variants, ...)                                       \
    ML99_TERMS(                                                                                    \
        ML99_call(GEN_VARIANTS_METADATA, v(name, variants)),                                       \
        ML99_call(GEN_METADATA, v(name), ML99_listLen(v(variants))))

#define GEN_VARIANTS_METADATA_IMPL(name, variants)                                                 \
    ML99_TERMS(                                                                                    \
        v(static const VariantMetadata name##_variants_metadata[] =),                              \
        ML99_braced(ML99_listMapInPlace(                                                           \
            ML99_compose(ML99_appl(v(HANDLE_VARIANT), v(name)), v(ML99_untuple)),                  \
            v(variants))),                                                                         \
        v(;))

#define HANDLE_VARIANT_IMPL(name_, tag, _sig) v({.name = #tag, .size = sizeof(name_##tag)}, )
#define HANDLE_VARIANT_ARITY                  2

#define GEN_METADATA_IMPL(name, variants_count_)                                                   \
    v(static const DatatypeMetadata name##_metadata = {                                            \
          .variants = (const VariantMetadata *)&name##_variants_metadata,                          \
          .variants_count = variants_count_,                                                       \
      };)

typedef struct {
    const char *name;
    size_t size;
} VariantMetadata;

typedef struct {
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
    {.name = "Char", .size = sizeof(NumChar)},
    {.name = "Int", .size = sizeof(NumInt)},
    {.name = "Double", .size = sizeof(NumDouble)},
};

static const DatatypeMetadata Num_metadata = {
    .variants = (const VariantMetadata *)&Num_variants_metadata,
    .variants_count = 3,
};
*/

int main(void) {

#define CHECK(idx, name_, size_)                                                                   \
    assert(strcmp(Num_metadata.variants[idx].name, name_) == 0);                                   \
    assert(Num_metadata.variants[idx].size == size_)

    CHECK(0, "Char", sizeof(char));
    CHECK(1, "Int", sizeof(int));
    CHECK(2, "Double", sizeof(double));

    assert(3 == Num_metadata.variants_count);

#undef CHECK
}
