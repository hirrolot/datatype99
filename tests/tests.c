#include <datatype99.h>

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

// clang-format off
datatype(
    EmptyDatatype,
    (EmptyA),
    (EmptyB)
);

datatype(
    ComplexDatatype,
    (A),
    (B, int),
    (C, const char *, int),
    (D, char, unsigned, long long, int *)
);

record(
    MyTinyRecord,
    (int, x)
);

record(
    MyRecord,
    (int, x),
    (long long, d),
    (const char *, str)
);

record(MyEmptyRecord);
// clang-format on

#define FAIL assert(false)

static void test_match_complex(ComplexDatatype expr) {
    match(expr) {
        of(A) {
            assert(ATag == expr.tag);

            return;
        }
        of(B, x) {
            assert(BTag == expr.tag);

            assert(x == &expr.data.B._0);

            return;
        }
        of(C, str, x) {
            assert(CTag == expr.tag);

            assert(str == &expr.data.C._0);
            assert(x == &expr.data.C._1);

            return;
        }
        of(D, c, x, y, ptr) {
            assert(DTag == expr.tag);

            assert(c == &expr.data.D._0);
            assert(x == &expr.data.D._1);
            assert(y == &expr.data.D._2);
            assert(ptr == &expr.data.D._3);

            return;
        }
    }

    FAIL;
}

static void test_match_empty(EmptyDatatype expr) {
    match(expr) {
        of(A) {
            assert(EmptyATag == expr.tag);
            return;
        }
        of(B) {
            assert(EmptyBTag == expr.tag);
            return;
        }
    }

    FAIL;
}

int main(void) {
    const char *const hello = "hello";

    const ComplexDatatype a = A(), b = B(42), c = C(hello, 12), d = D('~', 0, 5274, NULL);
    const EmptyDatatype empty_a = EmptyA(), empty_b = EmptyB();

    // Test the contents of the values.
    {
        assert(ATag == a.tag);

        assert(BTag == b.tag);
        assert(42 == b.data.B._0);

        assert(CTag == c.tag);
        assert(hello == c.data.C._0);
        assert(12 == c.data.C._1);

        assert(DTag == d.tag);
        assert('~' == d.data.D._0);
        assert(0 == d.data.D._1);
        assert(5274 == d.data.D._2);
        assert(NULL == d.data.D._3);

        assert(EmptyATag == empty_a.tag);
        assert('\0' == empty_a.data.dummy);

        assert(EmptyBTag == empty_b.tag);
        assert('\0' == empty_b.data.dummy);
    }

    // <datatype-name>Tag
    {
        ComplexDatatypeTag tag;

        tag = ATag;
        tag = BTag;
        tag = CTag;
        tag = DTag;

        (void)tag;
    }

    // <datatype-name>Variants
    {
        ComplexDatatypeVariants data = {.dummy = 0};

        data.B._0 = (int)123;

        data.C._0 = (const char *)"abc";
        data.C._1 = (int)9;

        data.D._0 = (char)'A';
        data.D._1 = (unsigned)2924;
        data.D._2 = (long long)-1811;
        data.D._3 = (int *)(int[]){123};

        (void)data;
    }

    // <variant-name>SumT
    {
        const ASumT a_indirect = a;
        const BSumT b_indirect = b;
        const CSumT c_indirect = c;
        const DSumT d_indirect = d;

        (void)a_indirect;
        (void)b_indirect;
        (void)c_indirect;
        (void)d_indirect;
    }

    // <variant-name>_I
    {
        B_0 b_0 = (int)123;
        (void)b_0;

        C_0 c_0 = (const char *)"baba";
        C_1 c_1 = (int)-91;
        (void)c_0;
        (void)c_1;

        D_0 d_0 = (char)'(';
        D_1 d_1 = (unsigned)12322;
        D_2 d_2 = (long long)-7;
        D_3 d_3 = (int *)(int[]){42};
        (void)d_0;
        (void)d_1;
        (void)d_2;
        (void)d_3;
    }

    // MATCHES
    {
        assert(MATCHES(a, A));
        assert(MATCHES(b, B));
        assert(MATCHES(c, C));
        assert(MATCHES(d, D));

        assert(!MATCHES(a, C));
        assert(!MATCHES(b, D));
        assert(!MATCHES(c, B));
        assert(!MATCHES(d, A));

        // Pass an rvalue to `matches`.
        assert(MATCHES(A(), A));
    }

    // Test a single `otherwise` branch.
    {
        match(a) {
            otherwise goto end_single_otherwise;
        }
        FAIL;
    end_single_otherwise:;
    }

    // Test `match`.
    {
        test_match_complex(a);
        test_match_complex(b);
        test_match_complex(c);
        test_match_complex(d);

        test_match_empty(empty_a);
        test_match_empty(empty_b);
    }

    // Test the reserved identifier `_`.
    {
        const ComplexDatatype expr = C("abc", 124);

        match(expr) {
            of(A) {}
            of(B, _) {}
            of(C, _, x) {
                (void)x;
            }
            of(D, c, _, _, ptr) {
                (void)c;
                (void)ptr;
            }
        }
    }

    // Test a nested `match`.
    {
        match(a) {
            of(A) {
                match(b) {
                    of(B) goto end_nested_match;
                    otherwise FAIL;
                }
            }
            otherwise FAIL;
        }
        FAIL;
    end_nested_match:;
    }

    // Test two `match` in the same lexical scope.
    {
#define TEST_MATCH                                                                                 \
    match(a) {                                                                                     \
        of(A);                                                                                     \
        of(B, _);                                                                                  \
        of(C, _, _);                                                                               \
        of(D, _, _, _, _);                                                                         \
    }

        TEST_MATCH;
        TEST_MATCH;

#undef TEST_MATCH
    }

    // The same identifiers from different branches shall not clash with each other.
    {
        const ComplexDatatype expr = A();

        match(expr) {
            of(A) {}
            of(B, same1) {
                (void)same1;
            }
            of(C, same1, same2) {
                (void)same1;
                (void)same2;
            }
            of(D, same1, same2, _, _) {
                (void)same1;
                (void)same2;
            }
        }
    }

    // ifLet
    {
        const ComplexDatatype expr = C("abc", 918);
        ifLet(expr, C, str, x) {
            assert(str == &expr.data.C._0);
            assert(x == &expr.data.C._1);
            goto end_if_let;
        }
        FAIL;
    end_if_let:;

        ifLet(expr, B, _) FAIL;
    }

    // Test two `ifLet` in the same lexical scope.
    {
        ifLet(b, B, _);
        ifLet(b, B, _);
    }

    // Make sure that `match` and `ifLet` result in a single C statement.
    {
        const ComplexDatatype expr = B(42);
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

        MyRecord r = (MyRecord){.x = 123, .d = 15, .str = "hello world"};
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
