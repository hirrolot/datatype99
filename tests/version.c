#include <datatype99.h>
#include <metalang99/assert.h>

int main(void) {

#undef DATATYPE99_MAJOR
#undef DATATYPE99_MINOR
#undef DATATYPE99_PATCH

#define DATATYPE99_MAJOR 1
#define DATATYPE99_MINOR 2
#define DATATYPE99_PATCH 3

    // DATATYPE99_VERSION_COMPATIBLE
    {

        ML99_ASSERT_UNEVAL(DATATYPE99_VERSION_COMPATIBLE(1, 0, 0));
        ML99_ASSERT_UNEVAL(DATATYPE99_VERSION_COMPATIBLE(1, 1, 0));
        ML99_ASSERT_UNEVAL(DATATYPE99_VERSION_COMPATIBLE(1, 1, 1));

        ML99_ASSERT_UNEVAL(DATATYPE99_VERSION_COMPATIBLE(1, 2, 0));
        ML99_ASSERT_UNEVAL(DATATYPE99_VERSION_COMPATIBLE(1, 2, 1));
        ML99_ASSERT_UNEVAL(DATATYPE99_VERSION_COMPATIBLE(1, 2, 2));
        ML99_ASSERT_UNEVAL(DATATYPE99_VERSION_COMPATIBLE(1, 2, 3));

        // Major-incompatible.
        ML99_ASSERT_UNEVAL(!DATATYPE99_VERSION_COMPATIBLE(2, 0, 0));
        ML99_ASSERT_UNEVAL(!DATATYPE99_VERSION_COMPATIBLE(7, 1, 2));

        // Minor-incompatible.
        ML99_ASSERT_UNEVAL(!DATATYPE99_VERSION_COMPATIBLE(1, 3, 0));
        ML99_ASSERT_UNEVAL(!DATATYPE99_VERSION_COMPATIBLE(1, 4, 9));

        // Patch-incompatible.
        ML99_ASSERT_UNEVAL(!DATATYPE99_VERSION_COMPATIBLE(1, 2, 4));
        ML99_ASSERT_UNEVAL(!DATATYPE99_VERSION_COMPATIBLE(1, 2, 5));
    }

    // DATATYPE99_VERSION_EQ
    {
        ML99_ASSERT_UNEVAL(DATATYPE99_VERSION_EQ(1, 2, 3));

        ML99_ASSERT_UNEVAL(!DATATYPE99_VERSION_EQ(1, 2, 7));
        ML99_ASSERT_UNEVAL(!DATATYPE99_VERSION_EQ(1, 7, 3));
        ML99_ASSERT_UNEVAL(!DATATYPE99_VERSION_EQ(7, 2, 3));
    }

#undef DATATYPE99_MAJOR
#undef DATATYPE99_MINOR
#undef DATATYPE99_PATCH
}