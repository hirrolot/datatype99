#include <datatype99.h>

#include <assert.h>
#include <string.h>

// Deriver implementation {

#define DATATYPE99_DERIVE_Metadata_IMPL(name, variants)                                            \
    ML99_TERMS(VARIANTS_METADATA(name, variants), METADATA(name, variants))

#define VARIANTS_METADATA(name, variants)                                                          \
    ML99_assignStmt(                                                                               \
        v(static const VariantMetadata name##_variants_metadata[]),                                \
        GEN_VARIANTS_INITIALIZER_LIST(name, variants))

#define GEN_VARIANTS_INITIALIZER_LIST(name, variants)                                              \
    ML99_braced(ML99_listMapInPlace(                                                               \
        ML99_compose(ML99_appl(v(GEN_VARIANT), v(name)), v(ML99_untuple)),                         \
        v(variants)))

#define GEN_VARIANT_IMPL(name_, tag, sig)                                                          \
    ML99_TERMS(                                                                                    \
        ML99_braced(                                                                               \
            ML99_assign(v(.name), v(#tag)),                                                        \
            ML99_assign(v(.arity), ML99_listLen(v(sig))),                                          \
            ML99_assign(v(.size), v(sizeof(name_##tag)))),                                         \
        v(, ))
#define GEN_VARIANT_ARITY 2

#define METADATA(name_, variants_)                                                                 \
    ML99_assignStmt(                                                                               \
        v(static const DatatypeMetadata name_##_metadata),                                         \
        ML99_braced(                                                                               \
            ML99_assign(v(.name), v(#name_)),                                                      \
            ML99_assign(v(.variants), v((const VariantMetadata *)&name_##_variants_metadata)),     \
            ML99_assign(v(.variants_count), ML99_listLen(v(variants_)))))
// } (Deriver implementation)

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
    derive(Metadata),
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
    do {                                                                                           \
        assert(strcmp(Num_metadata.variants[idx].name, name_) == 0);                               \
        assert(arity_ == Num_metadata.variants[idx].arity);                                        \
        assert(size_ == Num_metadata.variants[idx].size);                                          \
    } while (0)

    CHECK(0, "Char", 1, sizeof(char));
    CHECK(1, "Int", 1, sizeof(int));
    CHECK(2, "Double", 1, sizeof(double));

#undef CHECK

    assert(strcmp(Num_metadata.name, "Num") == 0);
    assert(3 == Num_metadata.variants_count);

    return 0;
}
