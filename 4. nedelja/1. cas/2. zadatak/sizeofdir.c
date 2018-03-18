// obilazak direktorijama
// postoje 2 pristupa, DFS i BFS
// mi cemo ovde implementirati DFS,
// odnosno idemo u dubinu (u poddirektorijum sve dok ih ima)

// da bude BFS nije neka velika razlika, samo se prakticno razlikuje
// gde ce biti rekurzija

// posto je direktorijum neko stablo,
// rekurzivno cemo prolazizti kroz poddirektorijume

// najjednostavnije je unapred vizuelizovati na primeru:
// dirA
// - .
// - ..
// - dirB
// -- .
// -- ..
// -- b.txt
// -- c.txt
// - regA
// - regB

// kada prolazimo, za svaku stavku proveravamo da li je direktorijum
// ako jeste, udjemo (rekurzivni poziv)
// ako nije, citamo taj fajl

// moramo da pazimo, kad udjemo u folder, prva stavka je .
// to moramo da preskocimo jer je to referenca za trenutni direktorijum

// druga stavka je onda ..
// i to preskacemo, posto je to referenca na roditeljski direktorijum
// opet bismo napravili beskonacnu petlju

// treca stavka je onda ono sto nam je bitno
// ako je fajl, citamo ga, ako je direktorijum, ide rekurzivni poziv za njega

// sve ovo vreme, vodimo racuna o trenutnoj putanji,
// kako bismo pravilno nadovezivali string za poddirektorijum

// kad stignemo do kraja, vracamo se na nivo iznad

#include <sys/stat.h> // stat

#include <stdbool.h> // bool
#include <stdio.h> // printf
#include <stdlib.h> // malloc, free...
#include <string.h> // strlen, strcmp

#include <dirent.h> // readdir, closedir

#define check_error(expr, msg) \
    do { \
        if (!(expr)) {\
            fprintf(stderr, "%s\n", msg);\
            exit(EXIT_FAILURE); \
        }\
    } while (0)

/**
 * Obilazak direktorijuma i racunanje njegove ukupne velicine
 * @param   char*           path        putanja do direktorijuma
 * @param   unsigned int*   dir_size    akumulator velicine
 * @return                              da li je operacija uspela
 */
bool traverseDir(char* path, unsigned* dir_size);

/**
 * Racunanje velicine direktorijuma
 * @param   int     argc    broj argumenata prosledjenih sa komandne linije
 * @param   char**  argv    vrednosti prosledjenih argumenata
 * @return  int             EXUT_SUCCESS/FAILURE
 *                          (u zavisnosti od toga da li je program uspesno izvrsen)
 */
int main(int argc, char** argv) {
    // check_error(argc == 2, "./sizeofdir path");

    int *dir_size = malloc(sizeof(int));
    *dir_size = 0;

    check_error(traverseDir(argv[1], dir_size), "Error traversing");

    printf("Dir size: %d\n", *dir_size);

    free(dir_size);

    exit(EXIT_SUCCESS);
}

bool traverseDir(char* path, unsigned* dir_size) {
    // prvo proveravamo sta je u pitanju na putanji
    struct stat finfo;
    if (lstat(path, &finfo) == -1) return false;

    // Ako je fajl, ocitamo velicinu i vratimo se
    // AKo je direktorijum, udjemo u njega i prolazimo kroz sadrzaj

    *dir_size += finfo.st_size;

    if (!S_ISDIR(finfo.st_mode)) { // obratiti paznju da je S_ISDIR a NE S_IFDIR
        return true;
    }

    // man 3 opendir
    DIR* dir = opendir(path);
    if (dir == NULL) return false;

    struct dirent* curr_entry;
    while ((curr_entry = readdir(dir)) != NULL) {
        // trenutna_putanja / nova_putanja \0
        char* new_path = malloc(strlen(path) + 1 + strlen(curr_entry->d_name) + 1);
        check_error(new_path != NULL, "malloc failed");

        // moze ovako:
        // strcpy(new_path, path);
        // strcat(new_path, "/");
        // strcat(new_path, curr_entry->d_name);

        // ili ovako:
        sprintf(new_path, "%s/%s", path, curr_entry->d_name);

        // pristupi su ekvivalentni


        if (!strcmp(curr_entry->d_name, ".") || !strcmp(curr_entry->d_name, "..")) {
            // NE OVAKO! ozbiljna greska, jer nismo na dobroj poziciji
            // if (stat(curr_entry->d_name, &finfo));

            if (stat(new_path, &finfo) == -1) {
                free(new_path);
                return false;
            }

            *dir_size += finfo.st_size;
            free(new_path);

            continue;
        }

        bool success = traverseDir(new_path, dir_size);

        if (!success) return false;

        // ne zaboraviti ovo!
        free(new_path);
    }

    if (closedir(dir) == -1) return false;
    else return true;
}