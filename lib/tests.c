/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: tests.c - Tests for math operations
 */

#include <kernel.h>

#if ENABLE_TESTS && __TURBOC__

typedef int (*math32_fn)(int32_t *out, int32_t a, int32_t b);
static int tests_failed = 0;
static int tests_ok = 0;

static void
start_test_case(const char *name)
{
    tests_failed = 0;
    tests_ok = 0;
    krn_debug_printf("- Checking %s... ", name);
}

static void
end_test_case()
{
    int total = tests_ok + tests_failed;

    krn_debug_printf("%d/%d ok\n", tests_ok, total);

    if (tests_failed) {
        krn_debug_beep();
    }
}

static void
check_math32(const char *name, math32_fn fn, int32_t a, int32_t b,
    int32_t expected, int expect_error)
{
    int32_t out = 0;
    int err = fn(&out, a, b);

    if (expect_error && !err) {
        krn_debug_printf("FAIL %s(%ld, %ld): expected error\n", name, a, b);
        ++tests_failed;
    } else if (!expect_error && err) {
        krn_debug_printf("FAIL %s(%ld, %ld): unexpected error\n", name, a, b);
        ++tests_failed;
    } else if (!expect_error && out != expected) {
        krn_debug_printf("FAIL: %s(%ld, %ld): got %ld, expected %ld\n",
            name, a, b, out, expected);
        ++tests_failed;
    } else {
        ++tests_ok;
    }
}

static void
test_add32(void)
{
    start_test_case("add32");

    check_math32("add32", add32, 0, 0, 0, 0);
    check_math32("add32", add32, 1, 2, 3, 0);
    check_math32("add32", add32, -1, -2, -3, 0);
    check_math32("add32", add32, -1, 1, 0, 0);
    check_math32("add32", add32, 100, -50, 50, 0);
    check_math32("add32", add32, INT32_MAX, 0, INT32_MAX, 0);
    check_math32("add32", add32, INT32_MIN, 0, INT32_MIN, 0);
    check_math32("add32", add32, INT32_MIN, INT32_MAX, -1, 0);
    check_math32("add32", add32, INT32_MAX, 1, 0, 1);
    check_math32("add32", add32, INT32_MIN, -1, 0, 1);
    check_math32("add32", add32, INT32_MAX, INT32_MAX, 0, 1);
    check_math32("add32", add32, INT32_MIN, INT32_MIN, 0, 1);

    end_test_case();
}

static void
test_sub32(void)
{
    start_test_case("sub32");

    check_math32("sub32", sub32, 0, 0, 0, 0);
    check_math32("sub32", sub32, 3, 2, 1, 0);
    check_math32("sub32", sub32, -1, -2, 1, 0);
    check_math32("sub32", sub32, 1, 1, 0, 0);
    check_math32("sub32", sub32, -50, -100, 50, 0);
    check_math32("sub32", sub32, INT32_MAX, 0, INT32_MAX, 0);
    check_math32("sub32", sub32, INT32_MAX, INT32_MAX, 0, 0);
    check_math32("sub32", sub32, INT32_MIN, 0, INT32_MIN, 0);
    check_math32("sub32", sub32, INT32_MIN, INT32_MIN, 0, 0);
    check_math32("sub32", sub32, INT32_MAX, -1, 0, 1);
    check_math32("sub32", sub32, INT32_MIN, 1, 0, 1);

    end_test_case();
}

static void
test_mul32(void)
{
    start_test_case("mul32");

    check_math32("mul32", mul32, 0, 0, 0, 0);
    check_math32("mul32", mul32, 1, 0, 0, 0);
    check_math32("mul32", mul32, 0, 1, 0, 0);
    check_math32("mul32", mul32, 1, 1, 1, 0);
    check_math32("mul32", mul32, 2, 3, 6, 0);
    check_math32("mul32", mul32, -2, 3, -6, 0);
    check_math32("mul32", mul32, 2, -3, -6, 0);
    check_math32("mul32", mul32, -2, -3, 6, 0);
    check_math32("mul32", mul32, 1000, 1000, 1000000L, 0);
    check_math32("mul32", mul32, -1, INT32_MAX, -INT32_MAX, 0);
    check_math32("mul32", mul32, -1, INT32_MIN, 0, 1);
    check_math32("mul32", mul32, INT32_MAX, 1, INT32_MAX, 0);
    check_math32("mul32", mul32, INT32_MIN, 1, INT32_MIN, 0);
    check_math32("mul32", mul32, INT32_MAX, 2, 0, 1);
    check_math32("mul32", mul32, INT32_MAX, INT32_MAX, 0, 1);
    check_math32("mul32", mul32, INT32_MIN, INT32_MIN, 0, 1);

    end_test_case();
}

static void
test_div32(void)
{
    start_test_case("div32");

    check_math32("div32", div32, 0, 1, 0, 0);
    check_math32("div32", div32, 6, 3, 2, 0);
    check_math32("div32", div32, 7, 3, 2, 0);
    check_math32("div32", div32, -6, 3, -2, 0);
    check_math32("div32", div32, 6, -3, -2, 0);
    check_math32("div32", div32, -6, -3, 2, 0);
    check_math32("div32", div32, INT32_MAX, 1, INT32_MAX, 0);
    check_math32("div32", div32, INT32_MIN, 1, INT32_MIN, 0);
    check_math32("div32", div32, INT32_MAX, -1, INT32_MIN + 1, 0);
    check_math32("div32", div32, INT32_MIN, -1, 0, 1);
    check_math32("div32", div32, 1, 0, 0, 1);
    check_math32("div32", div32, 0, 0, 0, 1);

    end_test_case();
}

static void
test_append32(void)
{
    start_test_case("append32");

    check_math32("append32", append32, 0, 0, 0, 0);
    check_math32("append32", append32, 0, 5, 5, 0);
    check_math32("append32", append32, 1, 2, 12, 0);
    check_math32("append32", append32, 12, 3, 123, 0);
    check_math32("append32", append32, 123, 4, 1234, 0);
    check_math32("append32", append32, -1, 2, -12, 0);
    check_math32("append32", append32, -12, 3, -123, 0);
    check_math32("append32", append32, -123, 4, -1234, 0);
    check_math32("append32", append32, 214748364L, 7, INT32_MAX, 0);
    check_math32("append32", append32, 214748364L, 8, 0, 1);
    check_math32("append32", append32, INT32_MAX, 0, 0, 1);
    check_math32("append32", append32, 0, -1, 0, 1);
    check_math32("append32", append32, 0, 10, 0, 1);

    end_test_case();
}

global void
tests_run(void)
{
    tests_failed = 0;

    krn_debug_printf("Starting tests...\n");

    test_add32();
    test_sub32();
    test_mul32();
    test_div32();
    test_append32();

    krn_debug_printf("Tests done.\n");
}

#endif
