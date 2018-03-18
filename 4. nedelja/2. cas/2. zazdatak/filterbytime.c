/*
 * Filtriranje fajlova unutar foldera po datumu
 *
 * Primer za kompajliranje:
 * gcc -std=c99 -o filterbytime.out filterbytime.c
 *
 * Primer poziva:
 * ./filterbytime.out path nr_days
 *
 * Neke stvari nisu ovde dokumentovane jer vec jesu u cat_file.c
 */

#define _XOPEN_SOURCE 700

#include <stdio.h> // printf
#include <stdlib.h> // exit
#include <string.h> // strrchr, strcmp

#include <ftw.h> // nftw
#include <time.h> // za time()

#define check_error(expr, msg) \
    do { \
        if (!(expr)) {\
            fprintf(stderr, "%s\n", msg);\
            exit(EXIT_FAILURE); \
        }\
    } while (0)

#define DAY_IN_SEC (24*60*60)

int filterbytime(const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuff);

int days = 0;

/**
 * Filtriranje sadrzaja foldera prema datumu
 * @param   int     argc    broj argumenata prosledjenih sa komandne linije
 * @param   char**  argv    vrednosti prosledjenih argumenata
 * @return  int             EXUT_SUCCESS/FAILURE
 *                          (u zavisnosti od toga da li je program uspesno izvrsen)
 */
int main(int argc, char** argv) {
    check_error(argc == 3, "./filterbytime path days");

    // 2 pristupa, duboko kopiranje ili pametni pokazizvaci
    days = atoi(argv[2]);

    struct stat finfo;
    check_error(stat(argv[1], &finfo) != -1, "stat failed");
    check_error(S_ISDIR(finfo.st_mode), "not a dir");

    // man 3 nftw
    check_error(nftw(argv[1], filterbytime, 50, 0) != -1, "nftw failed");

    exit(EXIT_SUCCESS);
}

int filterbytime(const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuff) {
    time_t now = time(NULL);
    time_t diffinsec = now - sb->st_mtime;

    if (diffinsec / DAY_IN_SEC < days) {
        printf("%-80s\n", fpath);
    }

    return 0; // ako ne vratimo 0, stace izvrsavanje
}
