/*
 * Brise postojeci sadrzaj u fajlu
 *
 * Primer za kompajliranje:
 * gcc -std=c99 -o truncate.out truncate.c
 *
 * Primer poziva:
 * ./truncate.out test_file.txt
 *
 * Neke stvari nisu ovde dokumentovane jer vec jesu u cat_file.c
 */

#include <sys/stat.h> // stat
#include <stdio.h> // fprintf
#include <stdlib.h> // exit
#include <stdint.h> // za intmax

#include <utime.h> // utime

#define check_error(expr, msg) \
    do { \
        if (!(expr)) {\
            fprintf(stderr, "%s\n", msg);\
            exit(EXIT_FAILURE); \
        }\
    } while (0)

/**
 * Brisanje sadrzaja fajla
 * @param   int     argc    broj argumenata prosledjenih sa komandne linije
 * @param   char**  argv    vrednosti prosledjenih argumenata
 * @return  int             EXUT_SUCCESS/FAILURE
 *                          (u zavisnosti od toga da li je program uspesno izvrsen)
 */
int main(int argc, char** argv) {
    check_error(argc == 2, "./truncate.out path");

    struct stat finfo;
    check_error(stat(argv[1], &finfo) != -1, "stat failed");

    // otvaranjem fajla u rezimu "w" (pisanje), brisemo postojeci sadrzaj
    FILE* f = fopen(argv[1], "w");
    check_error(f != NULL, "file open failed");
    fclose(f);

    // man 2 utime
    struct utimbuf ftime;
    ftime.actime = finfo.st_atime;
    ftime.modtime = finfo.st_mtime;

    check_error(utime(argv[1], &ftime) != -1, "Error setting time");

    exit(EXIT_SUCCESS);
}