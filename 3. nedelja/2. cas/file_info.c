/*
 * Prikazuje informacije o fajlu na datoj putanji
 *
 * Primer za kompajliranje:
 * gcc -o file_info.out file_info.c
 *
 * Primer poziva:
 * ./file_info.out test_file.txt
 *
 * Neke stvari nisu ovde dokumentovane jer vec jesu u cat_file.c
 */

#include <sys/stat.h> // stat

#include <stdio.h> // sprintf
#include <stdlib.h> // exit
#include <stdint.h> // intmax_t

#include <grp.h> // getgrgid
#include <pwd.h> // getpwuid
#include <string.h> // strdup, memset
#include <time.h> // ctime

#define MAX_SIZE 1024

#define check_error(expr, msg) \
    do { \
        if (!(expr)) {\
            fprintf(stderr, "%s\n", msg);\
            exit(EXIT_FAILURE); \
        }\
    } while (0)

void printFileInfo(const char* path);

/**
 * Dohvatanje imena korisnika na osnovu uid-a
 * @param   uid_t   uid     ID korisnika za kog dohvatamo ime
 * @return  char*           Korisnicko ime
 */
char* getUserName(uid_t uid);

/**
 * Dohvatanje imena grupe na osnovu gid-a
 * @param   gid_t   gid     ID grupe za koju dohvatamo ime
 * @return  char*           Naziv grupe
 */
char* getGroupName(gid_t gid);

/**
 * Prikazivanje informacija o fajlu
 * @param   int     argc    broj argumenata prosledjenih sa komandne linije
 * @param   char**  argv    vrednosti prosledjenih argumenata
 * @return  int             EXUT_SUCCESS/FAILURE
 *                          (u zavisnosti od toga da li je program uspesno izvrsen)
 */
int main(int argc, char** argv) {
    // ispisivanje informacija o fajlu
    // postoje 3 funkcije, stat, fstat i lstat
    // man 2 stat

    // obratiti pazznju na st mod polje
    // makroi S_IFMT i ostali, pazljivo koristiti,
    // ima S_IFREG i S_ISREG, ne mesati

    check_error(argc == 2, "./fileInfo filePath");

    printFileInfo(argv[1]);

    exit(EXIT_SUCCESS);
}

void printFileInfo(const char* path) {
    struct stat finfo;
    check_error(stat(path, &finfo) != -1, "stat error");

    // 2 moguce provere da li je regularni fajl:
    // - (finfo.st_mode & S_IFMT) == S_IFREG; // rucno
    // - S_ISREG(finfo.st_mode);              // funkcija

    char* finfostr = malloc(MAX_SIZE);
    check_error(finfostr != NULL, "malloc failed");
    // da bismo sve alocirane bajtove setovali na nulu, izbegnemo greske
    memset(finfostr, 0, MAX_SIZE); // ili calloc, sto je ekvivalent malloc+memset

    int total_bytes = 0;
    // racunamo da imamo sve dozvole (tako formiramo string),
    // pa cemo sklanjati sta nemamo
    total_bytes += sprintf(finfostr, "%s ", "-rwxrwxrwx");
    total_bytes += sprintf(finfostr + total_bytes, "%jd", (intmax_t) finfo.st_size);

    switch(finfo.st_mode & S_IFMT) {
        case S_IFREG:
            finfostr[0] = '-';
        break;

        case S_IFDIR:
            finfostr[0] = 'd';
        break;

        case S_IFLNK:
            finfostr[0] = 'l';
        break;

        case S_IFCHR:
            finfostr[0] = 'c';
        break;

        case S_IFBLK:
            finfostr[0] = 'b';
        break;

        case S_IFIFO:
            finfostr[0] = 'f';
        break;

        default:
        break;
    }

    if (!(finfo.st_mode & S_IRUSR)) { // provera: "ne mogu da citam fajl?"
        finfostr[1] = '-';
    }

    if (!(finfo.st_mode & S_IWUSR)) { // "ne mogu da upisujem u fajl?"
        finfostr[2] = '-';
    }

    if (!(finfo.st_mode & S_IXUSR)) { // "ne mogu da pokrenem fajl?"
        finfostr[3] = '-';
    }

    if (!(finfo.st_mode & S_IRGRP)) { // "grupa ne moze da otvori fajl?"
        finfostr[4] = '-';
    }

    if (!(finfo.st_mode & S_IRGRP)) { // "grupa ne moze da upise u fajl?"
        finfostr[5] = '-';
    }

    if (!(finfo.st_mode & S_IRGRP)) { // "grupa ne moze da pokrene fajl?"
        finfostr[6] = '-';
    }

    if (!(finfo.st_mode & S_IROTH)) { // "ostali ne mogu da otvore fajl?"
        finfostr[7] = '-';
    }

    if (!(finfo.st_mode & S_IWOTH)) { // "ostali ne mogu da upisu u fajl?"
        finfostr[8] = '-';
    }

    if (!(finfo.st_mode & S_IXOTH)) { // "ostali ne mogu da pokrenu fajl?"
        finfostr[9] = '-';
    }

    total_bytes += sprintf(finfostr + total_bytes, "%jd", (intmax_t)finfo.st_size);

    char* username = getUserName(finfo.st_uid);
    total_bytes += sprintf(finfostr + total_bytes, "%s", username);
    free(username);

    char* groupname = getGroupName(finfo.st_gid);
    total_bytes += sprintf(finfostr + total_bytes, "%s", groupname);
    free(groupname);

    char* timestr = ctime(&finfo.st_mtime);
    total_bytes += sprintf(finfostr + total_bytes, "%s", timestr);

	printf("%s\n", finfostr);
}

char* getUserName(uid_t uid) {
    struct passwd* userinfo = getpwuid(uid);
    check_error(userinfo != NULL, "getpwuidfail");

    // ne mozemo nikako return userinfo->pw_name
    // jer ce biti vracen string koji je na steku,
    // i bice obrisan kada izadjemo iz funkcije
    // umesto toga, moramo da kopiramo na heap i to onda koristimo

    char* username = malloc(strlen(userinfo->pw_name));
    check_error(username != NULL, "malloc failed");
    strcpy(username, userinfo->pw_name);

    return username;
}

char* getGroupName(gid_t gid) {
    struct group* groupname = getgrgid(gid);
    check_error(groupname != NULL, "getgr");

    // strdup <=> malloc + strcpy
    char* grname = strdup(groupname->gr_name);
    check_error(grname != NULL, "strdup failed");

    return grname;
}
