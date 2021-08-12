#include <datatype99.h>

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

datatype(Trivial1, (Trivial1A));
datatype(Trivial2, (Trivial2A));
datatype(Trivial3, (Trivial3A), (Trivial3B), (Trivial3C));

// clang-format off
datatype(
    Complex,
    (A),
    (B, int),
    (C, const char *, double),
    (D, char, unsigned, long long, int *)
);

record(
    MyTinyRecord,
    (int, x)
);

record(
    MyRecord,
    (int, x),
    (double, d),
    (const char *, str)
);

record(MyEmptyRecord);
// clang-format on

int main(void) {
    const char *hello = "hello", *world = "world";
    Complex a = A(), b = B(42), c = C(hello, 12.13414), d = D('~', 0, 121434551, NULL);

    ComplexTag tag;
    tag = ATag;
    tag = BTag;
    tag = CTag;
    tag = DTag;
    (void)tag;

    ComplexVariants data = {.dummy = 0};
    data.B._0 = 123;
    data.C._0 = "abc";
    data.C._1 = 124.45476;
    data.D._0 = 'A';
    data.D._1 = 2924;
    data.D._2 = -198474;
    data.D._3 = (int *)(int[]){123};
    (void)data;

    const ASumT a_indirect = a;
    const BSumT b_indirect = b;
    const CSumT c_indirect = c;
    const DSumT d_indirect = d;
    (void)a_indirect;
    (void)b_indirect;
    (void)c_indirect;
    (void)d_indirect;

    B_0 b_0 = 123;
    (void)b_0;

    C_0 c_0 = "baba";
    C_1 c_1 = 13.31131;
    (void)c_0;
    (void)c_1;

    D_0 d_0 = '(';
    D_1 d_1 = 12322;
    D_2 d_2 = -13131313;
    D_3 d_3 = (int *)(int[]){42};
    (void)d_0;
    (void)d_1;
    (void)d_2;
    (void)d_3;

    assert(MATCHES(a, A));
    assert(MATCHES(b, B));
    assert(MATCHES(c, C));
    assert(MATCHES(d, D));

    assert(!MATCHES(a, C));
    assert(!MATCHES(b, D));
    assert(!MATCHES(c, B));
    assert(!MATCHES(d, A));

    // Pass an lvalue to `matches`.
    assert(MATCHES(A(), A));

    assert(a.tag == ATag);
    assert(b.tag == BTag);
    assert(c.tag == CTag);
    assert(d.tag == DTag);

    int foo = 0;
    match(a) {
        otherwise {
            foo = 123;
        }
    }
    assert(123 == foo);

    int n = 7;

#define TEST_MATCH(expr)                                                                           \
    match(expr) {                                                                                  \
        of(A) {                                                                                    \
            foo = 0;                                                                               \
        }                                                                                          \
        of(B, x) {                                                                                 \
            assert(42 == *x);                                                                      \
                                                                                                   \
            *x = 191;                                                                              \
        }                                                                                          \
        of(C, str, x) {                                                                            \
            assert(hello == *str);                                                                 \
            assert(12.13414 == *x);                                                                \
                                                                                                   \
            *str = world;                                                                          \
            *x = 143.11;                                                                           \
        }                                                                                          \
        of(D, c, x, y, ptr) {                                                                      \
            assert('~' == *c);                                                                     \
            assert(0 == *x);                                                                       \
            assert(121434551 == *y);                                                               \
            assert(NULL == *ptr);                                                                  \
                                                                                                   \
            *c = 'A';                                                                              \
            *x = 13;                                                                               \
            *y = 191991;                                                                           \
            *ptr = &n;                                                                             \
        }                                                                                          \
    }                                                                                              \
    do {                                                                                           \
    } while (0)

#if !defined(__clang__) && __GNUC__ >= 6
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#endif

    TEST_MATCH(a);
    assert(0 == foo);

    TEST_MATCH(b);
    assert(191 == b.data.B._0);

    TEST_MATCH(c);
    assert(world == c.data.C._0);
    assert(143.11 == c.data.C._1);

    TEST_MATCH(d);
    assert('A' == d.data.D._0);
    assert(13 == d.data.D._1);
    assert(191991 == d.data.D._2);
    assert(&n == d.data.D._3);

#if !defined(__clang__) && __GNUC__ >= 6
#pragma GCC diagnostic pop
#endif

    // Test a nested `match`.
    match(a) {
        of(A) {
            match(b) {
                of(B) {
                    foo = 34;
                }
                otherwise assert(false);
            }
        }
        otherwise assert(false);
    }

    assert(34 == foo);

    // Test the reserved identifier `_`.
    {
        const Complex expr = C("abc", 124.1404);

        match(expr) {
            of(A) {
                assert(false);
            }
            of(B, _) {
                assert(false);
            }
            of(C, _, x) {
                assert(124.1404 == *x);
            }
            of(D, c, _, _, ptr) {
                (void)c;
                (void)ptr;
                assert(false);
            }
        }
    }

    // ifLet
    {
        const Complex expr = B(42);
        ifLet(expr, B, x) {
            *x = 8601;
        }

        assert(8601 == expr.data.B._0);

        ifLet(expr, C, str, x) {
            (void)str;
            (void)x;
            assert(false);
        }
    }

    // Make sure that `match` and `ifLet` result in a single C statement.
    {
        const Complex expr = B(42);
        if (true)
            match(expr) {
                otherwise {}
            }

        if (true)
            ifLet(expr, B, _) {}
    }

    // Test record types.
    {
        MyTinyRecord tiny = (MyTinyRecord){.x = 123};
        struct MyTinyRecord tiny2 = tiny;
        (void)tiny;
        (void)tiny2;

        MyRecord r = (MyRecord){.x = 123, .d = 15.25, .str = "hello world"};
        struct MyRecord r2 = r;
        (void)r;
        (void)r2;

        MyEmptyRecord empty_r = {.dummy = '\0'};
        struct MyEmptyRecord empty_r2 = empty_r;
        (void)empty_r;
        (void)empty_r2;
    }

    UnitT dummy = unit_v;
    (void)dummy;
}
