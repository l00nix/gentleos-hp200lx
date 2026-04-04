/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License.
 *
 * File: mk.c - Build tool
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Silence IDE warnings */
#ifdef __CLANGD__
struct ffblk { char *ff_name; int ff_ftime; int ff_fdate; };
int findfirst(const char *, struct ffblk *, int);
int findnext(struct ffblk *);
char *strlwr(char *);
int stricmp(const char *, const char *);
int access(const char *, int);
int unlink(const char *);
#else
#include <dir.h>
#endif

#define MAX_SRC_FILES 128
#define NASM_PATH "vendor\\nasm301\\nasm"
#define TCC_PATH "tcc"
#define CFLAGS "-mt -u- -g1 -c -Iinclude"
#define TLINK_FLAGS "/s /t /l"

static const char *SRC_DIRS[] = {
    "lib",
    "kernel",
    "gui",
    "data",
    "apps",
};

#define BUF_SIZE 4096

#define ASSERT(cond, msg) {             \
    if (!(cond)) {                      \
        fprintf(stderr, "\n%s(%d): %s\n", \
            __FILE__, __LINE__, msg);   \
        exit(1);                        \
    }                                   \
}

unsigned long latest_include_ftime = 0;

typedef struct {
    char type;
    unsigned long ftime;
    char dirname[16];
    char basename[16];
} src_file_st;

src_file_st src_files[MAX_SRC_FILES];
size_t src_files_count;


static unsigned long
file_get_ftime(const char *path)
{
    struct ffblk ff;

    if (findfirst(path, &ff, 0) != 0) {
        return 0;
    }

    return ((unsigned long)ff.ff_fdate << 16) | ff.ff_ftime;
}


static unsigned long
file_write(FILE *out, const char *buf, unsigned long n)
{
    if (fwrite(buf, 1, n, out) != n) {
        fprintf(stderr, "Write error\n");
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
    ASSERT(in, "Cannot open file to read");

    while ((n = fread(buf, 1, sizeof buf, in)) > 0) {
        total += file_write(out, buf, n);
    }

    ASSERT(!ferror(in), "Read error");

    fclose(in);

    return total;
}

static char
file_get_basename_and_type(const char *name, char *basename)
{
    const char *ext;

    ext = strrchr(name, '.');

    if (!ext) {
        return 0;
    }

    memcpy(basename, name, ext - name);
    basename[ext - name] = '\0';

    if (stricmp(ext, ".c") == 0) {
        return 'c';
    }

    if (stricmp(ext, ".s") == 0) {
        return 's';
    }

    return 0;
}

static void
build_and_check(const char *target, const char *cmd)
{
    unlink(target);

    printf("%s\n", cmd);

    ASSERT(system(cmd) == 0, "Build failed");
    ASSERT(access(target, 0) == 0, "Build failed");
}

static void
find_latest_include_ftime(void)
{
    struct ffblk ff;
    unsigned long t;
    int done;

    for (done = findfirst("include\\*.h", &ff, 0); !done; done = findnext(&ff)) {
        t = ((unsigned long)ff.ff_fdate << 16) | ff.ff_ftime;

        if (t > latest_include_ftime) {
            latest_include_ftime = t;
        }
    }
}

static void
find_src_files(void)
{
    struct ffblk ff;
    char pattern[32];
    char base[16];
    char type;
    src_file_st *sf;
    int i, done;

    src_files_count = 0;

    for (i = 0; i < (sizeof(SRC_DIRS) / sizeof(SRC_DIRS[0])); ++i) {
        sprintf(pattern, "%s\\*.*", SRC_DIRS[i]);

        for (done = findfirst(pattern, &ff, 0); !done; done = findnext(&ff)) {
            strlwr(ff.ff_name);

            type = file_get_basename_and_type(ff.ff_name, base);
            if (!type) {
                continue;
            }

            ASSERT(src_files_count < MAX_SRC_FILES, "Too many source files");

            sf = &src_files[src_files_count++];
            sf->type = type;
            sf->ftime = ((unsigned long)ff.ff_fdate << 16) | ff.ff_ftime;
            strcpy(sf->dirname, SRC_DIRS[i]);
            strcpy(sf->basename, base);
        }
    }
}

static void
make_objs(void)
{
    unsigned long obj_time;
    char cmd[256];
    char src_path[32];
    char obj_path[32];
    src_file_st *sf;
    size_t i;

    for (i = 0; i < src_files_count; ++i) {
        sf = &src_files[i];

        sprintf(src_path, "%s\\%s.%c", sf->dirname, sf->basename, sf->type);
        sprintf(obj_path, "build\\%s\\%s.obj", sf->dirname, sf->basename);

        obj_time = file_get_ftime(obj_path);

        if (obj_time != 0 && obj_time >= sf->ftime && (sf->type != 'c' || obj_time >= latest_include_ftime)) {
            continue;
        }

        if (sf->type == 'c') {
            sprintf(cmd, "%s %s -nbuild\\%s %s", TCC_PATH, CFLAGS, sf->dirname, src_path);
        } else {
            sprintf(cmd, "%s -f obj -o %s %s", NASM_PATH, obj_path, src_path);
        }

        build_and_check(obj_path, cmd);
    }
}

static void
make_kernel(void)
{
    FILE *f;
    src_file_st *sf;
    size_t i, n;

    make_objs();

    f = fopen("build\\tlink.rsp", "wb");
    ASSERT(f, "Cannot create tlink.rsp")

    fprintf(f, "%s +\r\n", TLINK_FLAGS);
    fprintf(f, "\tbuild\\kernel\\start.obj+\r\n");

    for (i = 0; i < src_files_count; ++i) {
        sf = &src_files[i];

        if (!strcmp(sf->dirname, "kernel") && !strcmp(sf->basename, "start")) {
            continue;
        }

        fprintf(f, "\tbuild\\%s\\%s.obj+\r\n", sf->dirname, sf->basename);
    }

    fprintf(f, ",build\\kernel.com,build\\kernel.map\r\n");
    fclose(f);

    build_and_check("build/kernel.com", "tlink @build\\tlink.rsp");
}

static void
make_boot(void)
{
    unsigned long obj_ftime = file_get_ftime("build\\boot.bin");
    unsigned long src_ftime = file_get_ftime("boot\\boot.s");

    if (obj_ftime != 0 && obj_ftime >= src_ftime) {
        return;
    }

    build_and_check("build\\boot.bin", NASM_PATH " -o build\\boot.bin boot\\boot.s");
}

static void
make_disk(const char *path, unsigned long size)
{
    FILE *out;

    printf("Creating %s... ", path);

    unlink(path);
    out = fopen(path, "wb");
    ASSERT(out, "Cannot open file to write");

    size -= file_copy(out, "build\\boot.bin");
    size -= file_copy(out, "build\\kernel.com");
    file_fill(out, size);

    ASSERT(fclose(out) == 0, "Write error");

    printf("Done\n");
}

static void
make_all(void)
{
    make_kernel();
    make_boot();
    make_disk("build\\fd720.img", 720UL * 1024UL);
    make_disk("build\\fd1440.img", 1440UL * 1024UL);
}

static void
make_clean(void)
{
    char cmd[128];
    int i;

    for (i = 0; i < (sizeof(SRC_DIRS) / sizeof(SRC_DIRS[0])); ++i) {
        sprintf(cmd, "del build\\%s\\*.*", SRC_DIRS[i]);
        system(cmd);
    }

    system("del build\\*.*");
}

int
main(int argc, char **argv)
{
    size_t i;
    const char *cmd;

    unlink("mk.obj");
    find_latest_include_ftime();
    find_src_files();

    cmd = argc >= 2? argv[1] : "all";

    if (!strcmp(cmd, "all")) {
        make_all();
    } else if (!strcmp(cmd, "boot")) {
        make_all();
        system("boot build\\fd1440.img");
    } else if (!strcmp(cmd, "run")) {
        make_all();
        system("build\\kernel.com");
    } else if (!strcmp(cmd, "clean")) {
        make_clean();
    } else if (!strcmp(cmd, "mk")) {
        system(TCC_PATH " misc\\mk.c");
        unlink("mk.obj");
    } else {
        printf("Usage: mk [all boot run clean mk]\n");
    }

    return 0;
}
