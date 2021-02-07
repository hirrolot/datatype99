#include <datatype99.h>

// Your array must be put into a structure because value constructors, which are ordinary C
// functions, cannot accept an array.
typedef struct {
    int data[5];
} MyArray;

// clang-format off
datatype(
    Foo,
    (MkFoo, MyArray)
);
// clang-format on

int main(void) {
    Foo foo = MkFoo((MyArray){.data = {1, 2, 3, 4, 5}});
    (void)foo;
}
