/*
 * Prikazuje informacije o datom korisniku
 *
 * Primer za kompajliranje:
 * gcc -std=c99 -o user_info.out user_info.c
 *
 * Primer poziva:
 * ./user_info.out some_user_name
 *
 * Neke stvari nisu ovde dokumentovane jer vec jesu u cat_file.c
 */

#include <stdio.h> // fprintf
#include <stdlib.h> // exit
#include <stdint.h> // intmax_t
#include <string.h> // memcpy

#include <pwd.h> // getpwnam

#define check_error(expr, msg) \
    do { \
        if (!(expr)) {\
            fprintf(stderr, "%s\n", msg);\
            exit(EXIT_FAILURE); \
        }\
    } while (0)

/**
 * Prikazivanje informacija o korisniku
 * @param   int     argc    broj argumenata prosledjenih sa komandne linije
 * @param   char**  argv    vrednosti prosledjenih argumenata
 * @return  int             EXUT_SUCCESS/FAILURE
 *                          (u zavisnosti od toga da li je program uspesno izvrsen)
 */
int main(int argc, char** argv) {
    check_error(argc == 2, "./user_info username");

    // strukturu tacnu smo videli u man stranama
    // ovo je staticki alocirana sistemska struktura i NIKAD SE NE DEALOCIRA
    struct passwd* user_info = getpwnam(argv[1]);
    check_error(user_info != NULL, "getpw failed");

    // Mogli bismo da imamo recimo ovo:
    // fprintf(stdout, "User name: %s\n", user_info->pw_name); // -> jer je char*
    // fprintf(stdout, "User ID: %jd\n", (intmax_t) user_info->pw_uid);
    // fprintf(stdout, "Group ID %jd\n", (intmax_t) user_info->pw_gid);
    // fprintf(stdout, "User Info %s\n", user_info->pw_gecos);
    // fprintf(stdout, "HomeDir: %s\n", user_info->pw_dir);
    // fprintf(stdout, "Shell: %s\n", user_info->pw_shell);
    // Ali kod iznad nije reentrant-safe, sto znaci da pri multitaskingu
    // moze da se desi da se razzlikuje ono sto se ispisuje ako neka druga aplikacija
    // izmeni info korisnika od kad se ucita struktura do kad se izvrsi print

    // zato kopiramo informacije
    struct passwd user_info_copy;
    // memcpy vs strcpy <- koliko se kopira; broj bajtova vs. term. nula
    memcpy(&user_info_copy, user_info, sizeof(struct passwd));

    fprintf(stdout, "User name: %s\n", user_info_copy.pw_name); // -> jer je char*
    fprintf(stdout, "User ID: %jd\n", (intmax_t) user_info_copy.pw_uid);
    fprintf(stdout, "Group ID %jd\n", (intmax_t) user_info_copy.pw_gid);
    fprintf(stdout, "User Info %s\n", user_info_copy.pw_gecos);
    fprintf(stdout, "HomeDir: %s\n", user_info_copy.pw_dir);
    fprintf(stdout, "Shell: %s\n", user_info_copy.pw_shell);

    // ovo je bolje, ali opet nije uradjen deep copy (kopiran je samo pokazivac)

    exit(EXIT_SUCCESS);
}
