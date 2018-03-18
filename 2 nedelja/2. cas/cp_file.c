/* Kopira fajl sa mesta A na mesto B.
 * Putanje se prosledjuju kao argumenti komandne linije
 *
 * Primer za kompajliranje:
 * gcc -std=c99 -o cp_file.out cp_file.c
 *
 * Primer poziva:
 * ./cp_file.out test_file.txt test_file_b.txt
 *
 *
 * Neke stvari nisu ovde dokumentovane jer vec jesu u cat_file.c
 */

#include <fcntl.h> // open, close
#include <unistd.h> // read, write

#include <stdio.h> // printf, fprintf
#include <stdlib.h> // exit

#define check_error(expr, msg) \
    do { \
        if (!(expr)) {\
            fprintf(stderr, "%s\n", msg);\
            exit(EXIT_FAILURE); \
        }\
    } while (0)

#define MAX_SIZE (1024)

/**
 * Kopiranje fajla
 * @param   int     argc    broj argumenata prosledjenih sa komandne linije
 * @param   char**  argv    vrednosti prosledjenih argumenata
 * @return  int             EXUT_SUCCESS/FAILURE
 *                          (u zavisnosti od toga da li je program uspesno izvrsen)
 */
int main(int argc, char** argv) {
    check_error(argc == 3, "./cp_file.out source dest");

    int src_fd = open(argv[1], O_RDONLY);
    check_error(src_fd != -1, "open src failed");

    int dest_fd = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0644);
    check_error(dest_fd != -1, "open dest failed");

    char buffer[MAX_SIZE];
    int bytes_read = 0;

    while ((bytes_read = read(src_fd, buffer, MAX_SIZE)) > 0) {
        check_error(write(dest_fd, buffer, bytes_read) != -1, "write err");

        // a inace, read i write koristiti kad nam nije bitna struktura
        // posto recimo ako citamo recimo float-ove, moguce da read odsece na pola zapisa broja
        // i samim tim nemamo tacno ocitano
    }

    check_error(bytes_read != -1, "read failed");

    close(src_fd);
    close(dest_fd);

    // da bismo proverili da li su fajlovi identicni,
    // mozemo da pokrenemo diff(PATH_A, PATH_B) u terminalu
    // prikazace gde se razlikuju
    // ako ne vrati nista, znaci isti su

    exit(EXIT_SUCCESS);
}