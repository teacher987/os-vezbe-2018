/* Ilustruje upotrebu sistemskog poziva lseek
 * i pozicioniranje unutar fajla
 *
 * Primer za kompajliranje:
 * gcc -std=c99 -o file_size.out file_size.c
 *
 * Primer poziva:
 * ./file_size.out test_file.txt
 *
 * Neke stvari nisu ovde dokumentovane jer vec jesu u cat_file.c
 */

#include <fcntl.h> // open, close
#include <unistd.h> // lseek

#include <stdio.h> // printf, fprintf
#include <stdint.h> // intmax_t
#include <stdlib.h> // exit

#define check_error(expr, msg) \
    do { \
        if (!(expr)) {\
            fprintf(stderr, "%s\n", msg);\
            exit(EXIT_FAILURE); \
        }\
    } while (0)

/**
 * Racunanje velicine fajla (na los nacin!)
 * @param   int     argc    broj argumenata prosledjenih sa komandne linije
 * @param   char**  argv    vrednosti prosledjenih argumenata
 * @return  int             EXUT_SUCCESS/FAILURE
 *                          (u zavisnosti od toga da li je program uspesno izvrsen)
 */
int main (int argc, char** argv) {
    check_error(argc == 2, "file_size.out path");

    int fd = open(argv[1], O_RDONLY);
    check_error(fd != -1, "open failed");

    off_t file_size = lseek(fd, 0, SEEK_END); // mora ovaj tip za velicine fajlova
    check_error(file_size != (off_t) -1, "lseek failed"); // mora da se kastuje

    // Ovo inace nije nacin za merenje velicine fajla
    // Moguce da mozemo da vidimo velicinu ali nemamo dozvolu za otvaranje
    // A moguce je i da smo otisli sa SEEK na pogresno mesto i dobijemo pogresan broj
    // stat se koristi za informacije o velicini

    // intmax_t moze svaki da ispise, cak i bas velike
    // zato se koristi %jd
    printf("File size: %jdB\n", (intmax_t) file_size);

    exit(EXIT_SUCCESS);
}