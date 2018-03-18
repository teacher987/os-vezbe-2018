/*
 * Nadovezuje se sadrzaj fajla na standardni izlaz
 *
 * Primer za kompajliranje:
 * gcc -std=c99 -o cat_file.out cat_file.c
 *
 * Primer poziva:
 * ./cat_file.out test.txt
 */

#include <fcntl.h> // open, close
#include <unistd.h> // read, write

#include <stdio.h> // printf, fprintf
#include <stdlib.h> // exit

// Performansi radi, definisemo makro a ne funkciju
// (na kolokvijumu je nebitno kako cemo, samo da radi)

// makro ce samo iskopirati ovo sve kao tekst tamo gde se koristi,
// pa nam zato treba \ na kraju svakog reda

// expr je u zagradama bas iz ovog razloga,
// da nije, i da imamo if (!expr) samo, a expr je, recimo
// 1 == 1 || 2 == 2, imali bismo efektivno
// if (! (1 == 1) || 2 == 2)
// ovde 1 == 1 => true => ! (1 == 1) => false
// ali ! ( 1 == 1) || 2 == 2 => false || true => true,
// S druge strane, ono sto smo hteli je
// ! (1 == 1 || 2 == 2) => ! (true || true) => ! true => false

// jer ne znamo gde ce se kod ubaciti
// takodje, sve je obavijeno sa do-while iz istog razloga,
// Ovo je pravilo za inace za makroe, ovde ne bi bio problem i da nema toga,
// jer imamo samo jednu naredbu, if.
// Medjutim, da imamo recimo #define nesto() { printf("A"); printf("B"); }
// a makro pozivamo u if, kao if (1 == 2) nesto();
// nastao bi problem, jer bi generisani kod bio:
// if (1 == 2) printf("A"); printf("B");
// prvi printf bi bio preskocen, sto i treba da se desi,
// ali B bi se svejedno ispisao, sto nismo hteli
// ovako smo obavili makro sa do-while i izlazimo rucno (ovde sa EXIT)
#define check_error(expr, msg) \
    do { \
        if (!(expr)) {\
            fprintf(stderr, "%s\n", msg);\
            exit(EXIT_FAILURE); \
        }\
    } while (0)

// po koliko (najvise) bajtova citamo
// nebitno koliko, samo da nije 1 po 1
#define MAX_SIZE (1024)

/**
 * Prepisivanje fajla
 * @param   int     argc    broj argumenata prosledjenih sa komandne linije
 * @param   char**  argv    vrednosti prosledjenih argumenata
 * @return  int             EXUT_SUCCESS/FAILURE
 *                          (u zavisnosti od toga da li je program uspesno izvrsen)
 */
int main(int argc, char** argv) {
    // Bitno je obraditi sve potencijalne greske,
    // ali nije toliko bitno da opisi budu opsirni
    check_error(argc == 2, "./cat_file.out path");

    // otvaranje fajla sa prosledjene putanje sa dozvolom za citanje
    int fd = open(argv[1], O_RDONLY);
    check_error(fd != -1, "open failed");

    // bafer za citanje
    // nema potrebe za dinamickom alokacijom, posto citamo isti broj karaktera svaki put
    // (nije greska da se uradi sa malloc, ali se nista ne dobije)
    char* buffer[MAX_SIZE];
    int bytes_read = 0; // koliko smo procitali

    // mora da se radi kroz petlju i za read i za write
    // kod read mora da ide sve do kraja fajla (dok ne naidjemo na EOF)
    // kow write mora onoliko koliko je procitano, ne umnozak MAX_SIZE
    // (posto ako ima 1538 bajtova, a mi ocitavamo po 1024, to nece biti tacno)
    while ((bytes_read = read(fd, buffer, MAX_SIZE)) > 0) {
        // konstanta za standardni izlaz; NE koristiti 0, 1, 2 direktno
        check_error(write(STDOUT_FILENO, buffer, bytes_read) != -1, "write err");

        // printf("%s", buffer); <-- ovo ne koristiti nikad, jer read upisuje u void*,
        // ne znamo da li je u pitanju string OBRATITI PAZNJU!

        // a inace, read i write koristiti kad nam nije bitna struktura
        // posto recimo ako citamo recimo float-ove, moguce da read odsece na pola zapisa broja
        // i samim tim nemamo tacno ocitano
    }

    printf("\n");

    // da li smo uopste nesto procitali
    check_error(bytes_read != -1, "read failed");

    close(fd);

    exit(EXIT_SUCCESS);
}