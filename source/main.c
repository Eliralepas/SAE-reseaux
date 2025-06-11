#include <stdio.h>
#include <stdlib.h>
#include "machines.h"
#include "reseau.h"
#include "tramway.h"
#include "paquet_IP.h"
#include "protocole_STP.h"

int main() {
    reseau r;
    init_reseau(&r);

    if (charger_reseau("./fichier.txt", &r) != 0) {
        fprintf(stderr, "Erreur : impossible de charger le réseau\n");
        return EXIT_FAILURE;
    }

    affichage_reseau(&r);

    machine *stA = &r.machines[0];
    machine *stB = &r.machines[1];

    station *sta = (station *)stA->equipement;
    station *stb = (station *)stB->equipement;

    trame *t = creation_trame(sta->st_MAC, stb->st_MAC, sta->st_IP, stb->st_IP, PING, ICMP_ECHO_REQUEST);
    if (!t) {
        fprintf(stderr, "Erreur : échec de création de la trame\n");
        deinit_reseau(&r);
        return EXIT_FAILURE;
    }

    machine *sw = NULL;
    for (int i=0; i<r.nbr_machines; i++){
        machine *m = (machine*) &r.machines[i];
        if (m->tp_equip == TYPE_SWITCH){
            sw = m;
        }
    }

    //protocole_STP_chemin(&r);
    int visite[r.nbr_machines];
        for (int k=0; k<r.nbr_machines; k++){
            visite[k]=0;
    }
    printf("%u",calcul_cout(&r, sw, visite));

    printf("\n=== Début de la simulation d'envoi ===\n");
    send_trame(t, &r);
    printf("=== Fin de la simulation ===\n");

    deinit_tram(t);
    free(t);
    deinit_reseau(&r);


    return 0;
}
