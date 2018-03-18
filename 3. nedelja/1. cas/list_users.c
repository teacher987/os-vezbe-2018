/*
 * Prikazuje informacije o korisnicima na sistemu
 *
 * Primer za kompajliranje:
 * gcc -o list_users.out list_users.c
 * (iz nekog razloga, kad stavim -std=c99, funkcije set/end/getpwent nisu dostupne)
 *
 * Primer poziva:
 * ./list_users.out
 *
 * Neke stvari nisu ovde dokumentovane jer vec jesu u cat_file.c
 */

#include <pwd.h> // setpwent, getpwent, endpwent

#include <stdint.h> // intmax_t
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
 * Prikazuje informacije o korisniku
 * @param   passwd*     user_info   Struktura sa informacijama
 */
void printUser(struct passwd* user_info);

/**
 * Listanje korisnika
 * @param   int     argc    broj argumenata prosledjenih sa komandne linije
 * @param   char**  argv    vrednosti prosledjenih argumenata
 * @return  int             EXUT_SUCCESS/FAILURE
 *                          (u zavisnosti od toga da li je program uspesno izvrsen)
 */
int main(int argc, char** argv) {
    check_error(argc == 1, "./list_users.out");

    // man 3 setpwent, getpwent, endpwent

    // premotava na pocetak baze s korisnicima
    setpwent();

    struct passwd* current_user;
    while ((current_user = getpwent()) != NULL) {
        printUser(current_user);
    }

    // zatvara bazu s korisnicima na kraju koriscenja
    endpwent();

    exit(EXIT_SUCCESS);
}

void printUser(struct passwd* user_info) {
    fprintf(stdout, "User name: %s\n", user_info->pw_name);
    fprintf(stdout, "User ID: %jd\n", (intmax_t) user_info->pw_uid);
    fprintf(stdout, "Group ID %jd\n", (intmax_t) user_info->pw_gid);
    fprintf(stdout, "User Info %s\n", user_info->pw_gecos);
    fprintf(stdout, "HomeDir: %s\n", user_info->pw_dir);
    fprintf(stdout, "Shell: %s\n", user_info->pw_shell);
    fprintf(stdout, "\n\n\n\n");
}
