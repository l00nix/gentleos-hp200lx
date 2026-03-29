/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License.
 *
 * File: mkdisk.c - Tool to build floppy disk images
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 4096

static unsigned long
file_write(FILE *out, const char *buf, unsigned long n)
{
    if (fwrite(buf, 1, n, out) != n) {
        fprintf(stderr, "write error\n");
        exit(1);
    }

    return n;
}

static void
file_fill(FILE *out, unsigned long n)
{
    static char buf[BUF_SIZE];

    memset(buf, 0, BUF_SIZE);

    while (n >= BUF_SIZE) {
        n -= file_write(out, buf, BUF_SIZE);
    }

    if (n > 0) {
        n -= file_write(out, buf, n);
    }
}

static unsigned long
file_copy(FILE *out, const char *path)
{
    FILE *in;
    static char buf[BUF_SIZE];
    unsigned long n;
    unsigned long total = 0;

    in = fopen(path, "rb");
    if (!in) {
        fprintf(stderr, "cannot open '%s'\n", path);
        exit(1);
    }

    while ((n = fread(buf, 1, sizeof buf, in)) > 0) {
        total += file_write(out, buf, n);
    }

    if (ferror(in)) {
        fprintf(stderr, "read error on '%s'\n", path);
        exit(1);
    }

    fclose(in);

    return total;
}

static void
make_disk(const char *path, unsigned long size)
{
    FILE *out;

    printf("Creating %s... ", path);

    unlink(path);
    out = fopen(path, "wb");

    if (!out) {
        fprintf(stderr, "cannot create '%s'\n", path);
        exit(1);
    }

    size -= file_copy(out, "build\\boot.bin");
    size -= file_copy(out, "build\\kernel.com");
    file_fill(out, size);

    if (fclose(out) != 0) {
        fprintf(stderr, "write error\n");
        exit(1);
    }

    printf("Done\n");
}

int
main()
{
    make_disk("build\\fd720.img", 720UL * 1024UL);
    make_disk("build\\fd1440.img", 1440UL * 1024UL);

    return 0;
}
