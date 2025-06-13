#include <stdio.h>
#include <stdlib.h>
#include "machines.h"
#include "reseau.h"
#include "tramway.h"
#include "protocole_STP.h"

int main() {
    reseau *r = malloc(sizeof(reseau));
    init_reseau(r);

    // Choix du réseau
    int input;
    printf("Veuillez choisir un reseau :\n1 - AVEC boucle\n2 - SANS boucle\n> ");
    scanf("%d", &input);
    while(getchar() != '\n'); // vider le buffer

    if (input == 1) {
        if (charger_reseau("cycle.txt", r) == 0) {
            printf("Réseau AVEC boucle chargé.\n");
            protocole_STP_chemin(r);
            printf("Protocole STP appliqué.\n");
            affichage_port_etat(r);
        } else {
            printf("Erreur de chargement.\n");
        }
    }
    else if (input == 2) {
        if (charger_reseau("fichier.txt", r) == 0) {
            printf("Réseau AVEC boucle chargé.\n");
            protocole_STP_chemin(r);
            printf("Protocole STP appliqué.\n");
            affichage_port_etat(r);
        } else {
            printf("Erreur de chargement.\n");
        }
    }
    else {
        printf("ROOOOOOOOOOOOOOOOOOOOOOH! >:c\n");
        deinit_reseau(r);
        free(r);
        return EXIT_FAILURE;
    }

    // PING interactif
    int oui;
    printf("Voulez-vous envoyer une trame ?\n1 - OUI\n2 - NON\n> ");
    scanf("%d", &oui);
    while(getchar() != '\n');

    while (oui == 1) {
        int choix_source = -1;
        int choix_dest = -1;

        // Affichage des STATIONS disponibles
        printf("Stations disponibles :\n");

        int index_station = 0;
        int map_index_to_id[r->nb_machines];

        for (int i = 0; i < r->nb_machines; i++) {
            if (r->machines[i].tp_equip == TYPE_STATION) {
                station *st = (station *) r->machines[i].equipement;
                char ipstr[16];
                ip_to_str(st->st_IP, ipstr);
                printf("%d - machine n°%d (%s)\n", index_station, r->machines[i].id, ipstr);
                map_index_to_id[index_station] = i;
                index_station++;
            }
        }

        printf("Choisissez l’index de la station source : ");
        scanf("%d", &choix_source);
        while(getchar() != '\n');

        if (choix_source < 0 || choix_source >= index_station) {
            printf("Index invalide.\n");
            continue;
        }

        int src_id = map_index_to_id[choix_source];
        machine *src = &r->machines[src_id];
        station *st_src = (station *) src->equipement;

        // Destination
        printf("Stations disponibles (destinations) :\n");
        index_station = 0;
        for (int i = 0; i < r->nb_machines; i++) {
            if (r->machines[i].tp_equip == TYPE_STATION && i != src_id) {
                station *st = (station *) r->machines[i].equipement;
                char ipstr[16];
                ip_to_str(st->st_IP, ipstr);
                printf("%d - machine n°%d (%s)\n", index_station, r->machines[i].id, ipstr);
                map_index_to_id[index_station] = i;
                index_station++;
            }
        }

        printf("Choisissez l’index de la station destination : ");
        scanf("%d", &choix_dest);
        while(getchar() != '\n');

        if (choix_dest < 0 || choix_dest >= index_station) {
            printf("Index invalide.\n");
            continue;
        }

        int dst_id = map_index_to_id[choix_dest];
        machine *dst = &r->machines[dst_id];
        station *st_dst = (station *) dst->equipement;

        printf("Quel message voulez vous envoyez ?\n");
        char *data = malloc(sizeof(char)*1024);
        scanf("%s", data);

        // Création et envoi de la trame
        trame *t = creation_trame(st_src->st_MAC, st_dst->st_MAC, data, (strlen(data) + 1));
        if (!t) {
            perror("Erreur lors de la création de la trame\n");
        } else {
            envoie_trame(r, t);
            deinit_trame(t);
            free(t);
        }

        free(data);

        // Reboucle
        printf("Voulez-vous envoyer un autre message ?\n1 - OUI\n2 - NON\n> ");
        scanf("%d", &oui);
        while(getchar() != '\n');
    }

    // Nettoyage
    deinit_reseau(r);
    free(r);
    return 0;
}
