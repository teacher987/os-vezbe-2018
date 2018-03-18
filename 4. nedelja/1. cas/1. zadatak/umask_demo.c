/*
 * Kreira fajl sa datim dozvolama
 *
 * Primer za kompajliranje:
 * gcc -std=c99 -o umask_demo.out umask_demo.c
 *
 * Primer poziva:
 * ./umask_demo.out test_file.txt 777
 *
 * Neke stvari nisu ovde dokumentovane jer vec jesu u cat_file.c
 */

#include <sys/stat.h> // umask

#include <errno.h> // errno
#include <fcntl.h> // O_WRONLY | O_EXCL...
#include <unistd.h> // close

#include <stdio.h> // fprintf
#include <stdlib.h> // exit

#define check_error(expr, msg) \
    do { \
        if (!(expr)) {\
            fprintf(stderr, "%s\n", msg);\
            exit(EXIT_FAILURE); \
        }\
    } while (0)

/**
 * Kreiranje fajla sa datim dozvolama
 * @param   int     argc    broj argumenata prosledjenih sa komandne linije
 * @param   char**  argv    vrednosti prosledjenih argumenata
 * @return  int             EXUT_SUCCESS/FAILURE
 *                          (u zavisnosti od toga da li je program uspesno izvrsen)
 */
int main(int argc, char** argv) {
    check_error(argc == 3, "./umask_demo path mode");

    // proveraamo open sa man 2 open,
    // da vidimo koji su argumenti
    // obratiti paznju na mode_t, posto je to broj,
    // a argv stize kao string
    // takodje, broj je u oktalnoj varijanti,
    // pa koristimo strtol a NE strtoi

    // cuvamo staru vrednost umask-a
    mode_t umask_old = umask(0);

    long mode = strtol(argv[2], NULL, 8);

    // O_EXCL pravi fajl ako ne postoji, a ako postoji, puca
    // s druge strane, O_TRUNC bi samo pregazio postojeci fajl
    int fd = open(argv[1], O_WRONLY | O_EXCL /* O_TRUNC */ | O_CREAT, mode);

    // errno koristimo za vodjenje racuna sta je poslo naopako kod sistemskih poziva
    // mozemo na man stranama videti tacno kodove

    // check_error(fd != -1, "file open failed");
    // ne mozemo da koristimo check_error, jer ne daje detaljnije informacije

    if (fd == -1) {
        if (errno == EEXIST) {
            fprintf(stderr, "File already exists\n");
            check_error(chmod(argv[1], mode) != -1, "chmod failed");
        } else { // neki drugi problem, koji nam nije bitan
            check_error(0, "Greska"); // ne moze false, treba stdbol da se ukljuci
        }
    } else {
        close(fd);
    }

    // ovde treba imati na umu, ako se menjaju prava pristupa postojecem fajlu,
    // umask nije resenje
    umask(umask_old);

    exit(EXIT_SUCCESS);
}