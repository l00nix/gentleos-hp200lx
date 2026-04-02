/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: math.c - Math operations on 32-bit numbers
 */

#include <lib.h>

static int
udivmod32(uint32_t dividend, uint32_t divisor, uint32_t *quotient, uint32_t *remainder)
{
    uint32_t quot;
    uint32_t remd;
    uint16_t bit;

    if (divisor == 0) {
        return 1;
    }

    if (dividend < divisor) {
        *quotient = 0;
        *remainder = dividend;
        return 0;
    }

    if (dividend == divisor) {
        *quotient = 1;
        *remainder = 0;
        return 0;
    }

    quot = 0;
    remd = 0;

    for (bit = 32; bit != 0; --bit)
    {
        remd <<= 1;

        if (dividend & 0x80000000)
            remd |= 1;

        dividend <<= 1;
        quot <<= 1;

        if (remd >= divisor) {
            remd -= divisor;
            quot |= 1;
        }
    }

    *quotient = quot;
    *remainder = remd;

    return 0;
}

int
udiv32(uint32_t *out, uint32_t dividend, uint32_t divisor)
{
    uint32_t quot, remd;

    if (udivmod32(dividend, divisor, &quot, &remd)) {
        return 1;
    }

    *out = quot;
    return 0;
}

int
umod32(uint32_t *out, uint32_t dividend, uint32_t divisor)
{
    uint32_t quot, remd;

    if (udivmod32(dividend, divisor, &quot, &remd)) {
        return 1;
    }

    *out = remd;
    return 0;
}

int
add32(int32_t *out, int32_t a, int32_t b)
{
    int32_t res = a + b;

    if (a > 0 && b > 0 && res < 0) {
        return 1;
    }

    if (a < 0 && b < 0 && res >= 0) {
        return 1;
    }

    *out = res;
    return 0;
}

int
sub32(int32_t *out, int32_t a, int32_t b)
{
    int32_t res = a - b;

    if (a >= 0 && b < 0 && res < 0) {
        return 1;
    }

    if (a < 0 && b > 0 && res > 0) {
        return 1;
    }

    *out = res;
    return 0;
}
