#include <string.h> // strcmp
#include <unistd.h> // unlink, rmdir

#include <stdio.h> // printf, fprintf
#include <stdlib.h> // exit

/**
 * Brise zadat fajl ili folder
 *
 * Primer za kompajliranje:
 * gcc -std=c99 -o rmfile.out rmfile.c
 *
 * Upotreba:
 * ./rmfile.out -d dir_name
 * (brise folder po imenu dir_name)
 * ili
 * ./rmfile.out -f file_name.tmp
 * (brise fajl po imenu file_name tmp)
 *
 * Neke stvari nisu ovde dokumentovane jer vec jesu u cat_file.c
 */

#define check_error(expr, msg) \
    do { \
        if (!(expr)) {\
            fprintf(stderr, "%s\n", msg);\
            exit(EXIT_FAILURE); \
        }\
    } while (0)

/**
 * Brisanje fajla/foldera
 * @param   int     argc    broj argumenata prosledjenih sa komandne linije
 * @param   char**  argv    vrednosti prosledjenih argumenata
 * @return  int             EXUT_SUCCESS/FAILURE
 *                          (u zavisnosti od toga da li je program uspesno izvrsen)
 */
int main(int argc, char** argv) {
    check_error(argc == 3, "./rmfile -d/f filename");

    if (!strcmp("-f", argv[1])) {
        check_error(unlink(argv[2]) != -1, "unlink failed");
    } else if (!strcmp("-d", argv[1])) {
        check_error(rmdir(argv[2]) != -1, "rmdif failed");
    } else {
        check_error(0, "wrong option");
    }

    exit(EXIT_SUCCESS);
}