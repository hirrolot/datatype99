#include <datatype99.h>

#include <assert.h>
#include <string.h>

// Deriver implementation {

#define DATATYPE99_RECORD_DERIVE_Metadata_IMPL(name, fields)                                       \
    ML99_TERMS(FIELDS_METADATA(name, fields), METADATA(name, fields))

#define FIELDS_METADATA(name, fields)                                                              \
    ML99_assignStmt(                                                                               \
        v(static const FieldMetadata name##_fields_metadata[]),                                    \
        GEN_FIELDS_INITIALIZER_LIST(name, fields))

#define GEN_FIELDS_INITIALIZER_LIST(name, fields)                                                  \
    ML99_braced(ML99_listMapInPlace(                                                               \
        ML99_compose(ML99_appl(v(GEN_FIELD), v(name)), v(ML99_untuple)),                           \
        v(fields)))

#define GEN_FIELD_IMPL(name_, ty, ident)                                                           \
    ML99_TERMS(                                                                                    \
        ML99_braced(ML99_assign(v(.name), v(#ident)), ML99_assign(v(.size), v(sizeof(ty)))),       \
        v(, ))
#define GEN_FIELD_ARITY 2

#define METADATA(name_, fields_)                                                                   \
    ML99_assignStmt(                                                                               \
        v(static const RecordMetadata name_##_metadata),                                           \
        ML99_braced(                                                                               \
            ML99_assign(v(.name), v(#name_)),                                                      \
            ML99_assign(v(.fields), v((const FieldMetadata *)&name_##_fields_metadata)),           \
            ML99_assign(v(.fields_count), ML99_listLen(v(fields_)))))
// } (Deriver implementation)

typedef struct {
    const char *name;
    size_t size;
} FieldMetadata;

typedef struct {
    const char *name;
    const FieldMetadata *fields;
    size_t fields_count;
} RecordMetadata;

// clang-format off
record(
    derive(Metadata),
    Point,
    (int, x),
    (int, y)
);
// clang-format on

/*
The generated metadata:

static const FieldMetadata Point_fields_metadata[] = {
    {.name = "x", .size = sizeof(int)},
    {.name = "y", .size = sizeof(int)},
};

static const RecordMetadata Point_metadata = {
    .name = "Point", .fields = (const FieldMetadata *)&Point_fields_metadata, .fields_count = 2};
*/

int main(void) {

#define CHECK(idx, name_, size_)                                                                   \
    do {                                                                                           \
        assert(strcmp(Point_metadata.fields[idx].name, name_) == 0);                               \
        assert(size_ == Point_metadata.fields[idx].size);                                          \
    } while (0)

    CHECK(0, "x", sizeof(int));
    CHECK(1, "y", sizeof(int));

#undef CHECK

    assert(strcmp(Point_metadata.name, "Point") == 0);
    assert(2 == Point_metadata.fields_count);

    return 0;
}
