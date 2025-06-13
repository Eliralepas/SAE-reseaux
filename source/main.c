#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machines.h"
#include "reseau.h"
#include "tramway.h"
#include "paquet_IP.h"
#include "protocole_STP.h"

int main() {
    reseau r;
    init_reseau(&r);

    if (charger_reseau("fichier.txt", &r) != SUCCESS) {
        fprintf(stderr, "Erreur : échec du chargement du réseau\n");
        deinit_reseau(&r);
        return EXIT_FAILURE;
    }

    //affichage_reseau(&r);

    machine *stA = &r.machines[2];
    machine *stB = &r.machines[3];

    station *sta = (station *)stA->equipement;
    station *stb = (station *)stB->equipement;

    char* data = malloc(1024);
    if (!data) {
        fprintf(stderr, "Erreur : échec de l'allocation de data\n");
        deinit_reseau(&r);
        return EXIT_FAILURE;
    }
    strcpy(data, "je t'envoie un trame.");

    trame *t = creation_trame(sta->st_MAC, stb->st_MAC, data, strlen(data) + 1);
    if (!t) {
        fprintf(stderr, "Erreur : échec de création de la trame\n");
        free(data);
        deinit_reseau(&r);
        return EXIT_FAILURE;
    }
    protocole_STP_chemin(&r);

    printf("\n=== Début de la simulation d'envoi ===\n");
    envoie_trame(&r, t);
    printf("=== Fin de la simulation ===\n");

    deinit_trame(t);
    free(t);
    deinit_reseau(&r);

    return 0;
}