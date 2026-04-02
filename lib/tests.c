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
check_math32(const char *name, math32_fn fn, int32_t a, int32_t b,
    int32_t expected, int expect_error)
{
    int32_t out = 0;
    int err = fn(&out, a, b);

    if (expect_error && !err) {
        krn_debug_printf_bios("FAIL %s(%ld, %ld): expected error\n", name, a, b);
        ++tests_failed;
    } else if (!expect_error && err) {
        krn_debug_printf_bios("FAIL %s(%ld, %ld): unexpected error\n", name, a, b);
        ++tests_failed;
    } else if (!expect_error && out != expected) {
        krn_debug_printf_bios("FAIL: %s(%ld, %ld): got %ld, expected %ld\n",
            name, a, b, out, expected);
        ++tests_failed;
    } else {
        ++tests_ok;
    }
}


static void
test_add32(void)
{
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
}

static void
test_sub32(void)
{
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
}

void
tests_run(void)
{
    tests_failed = 0;

    krn_debug_printf_bios("Starting tests...\n");

    test_add32();
    test_sub32();

    krn_debug_printf_bios("Tests done: %d ok, %d failed\n", tests_ok, tests_failed);
    krn_debug_printf_bios("Press enter to continue...");
    (void)bios_getc();
}

#endif
