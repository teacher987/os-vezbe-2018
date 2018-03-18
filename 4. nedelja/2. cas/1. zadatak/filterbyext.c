/*
 * Filtriranje fajlova unutar foldera po ekstenziji
 *
 * Primer za kompajliranje:
 * gcc -std=c99 -o filterbyext.out filterbyext.c
 *
 * Primer poziva:
 * ./filterbyext.out path .txt
 *
 * Neke stvari nisu ovde dokumentovane jer vec jesu u cat_file.c
 */

#define _XOPEN_SOURCE 700

#include <stdio.h> // printf
#include <stdlib.h> // exit
#include <string.h> // strrchr, strcmp
#include <ftw.h> // nftw

#define check_error(expr, msg) \
    do { \
        if (!(expr)) {\
            fprintf(stderr, "%s\n", msg);\
            exit(EXIT_FAILURE); \
        }\
    } while (0)

int filterbyext(const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuff);

char* ext;

/**
 * Filtriranje sadrzaja foldera prema ekstenziji
 * @param   int     argc    broj argumenata prosledjenih sa komandne linije
 * @param   char**  argv    vrednosti prosledjenih argumenata
 * @return  int             EXUT_SUCCESS/FAILURE
 *                          (u zavisnosti od toga da li je program uspesno izvrsen)
 */
int main(int argc, char** argv) {
    check_error(argc == 3, "./filterbyext path ext");

    // 2 pristupa, duboko kopiranje ili pametni pokazizvaci
    ext = argv[2];

    struct stat finfo;
    check_error(stat(argv[1], &finfo) != -1, "stat failed");
    check_error(S_ISDIR(finfo.st_mode), "not a dir");

    // man 3 nftw
    check_error(nftw(argv[1], filterbyext, 50, 0) != -1, "nftw failed");

    exit(EXIT_SUCCESS);
}

int filterbyext(const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuff) {
    // FTW struktura ima 2 polja, level je nivo rekurzije, krece od 0
    // base je osnovni deo putanje fajla (sve pre imena fajla i ekstenzije)
    if (typeflag == FTW_F) {
        // ekstenziju dobijemo tako sto dohvatimo deo imena posle poslednje tacke
        char* fext = strrchr(fpath + ftwbuff->base, '.');

        // obratiti pazznju, ovde moze lako da bude segfault
        // ako uradimo ovako:
        // if (!strcmp(fext, ext)) {}
        // jer ako u nazivu fajla nema tacke, fext bice NULL

        if (fext != NULL && !strcmp(fext, ext)) {
            printf("%-80s\n", fpath);
        }
    }

    return 0; // ako ne vratimo 0, stace izvrsavanje
}
