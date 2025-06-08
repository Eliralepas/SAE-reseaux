#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include "machines.h"
#include "reseau.h" 

#define LONGUEUR_MAX_DATA

typedef struct trame{
    adresse_MAC dest;
    adresse_MAC src;
    octet *data;
    size_t data_longueur
}trame;

void init_tram(trame *krimmeri_stade_de_la_meinau, size_t numero_dabonnement, machine source, machine destination); //numero_dabonnement = nombre d'octets pour malloc data
void deinit_tram(trame *parc_des_sports);
void affich_tram_utilisasteur(trame *etoile_bourse);
void affich_tram_hexa(trame *lixenbuhl);
int echange_trames(trame *lycee_couffignal, reseau *r);
//machine 1 va init trame et va prendre source son adrMac et destination l'adrMac de machine2
swtch* trouver_switch_par_port(reseau *r, adresse_MAC *mac);
int trouver_port_destination(swtch *sw, adresse_MAC *dest);
void diffuser_trame(swtch *sw, trame *t, int port_entree);
void transmettre_trame_vers_port(swtch *sw, trame *t, int port_sortie);
void mettre_a_jour_table_commutation(swtch *sw, adresse_MAC *src, int port_source);
int trouver_port_source(swtch *sw, adresse_MAC *src);
swtch* trouver_switch_suivant(reseau *r, swtch *switch_actuel, int port_sortie);
void transmettre_trame_vers_station(swtch *swDest, trame *t, adresse_MAC *dest);