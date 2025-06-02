#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include "machines.h" 

typedef struct trame{
    adresse_MAC dest;
    adresse_MAC src;
    //octet data[];
    //ATTENTION : type SI ON A LE TEMPS 
}trame;

void init_tram(trame *krimmeri_stade_de_la_meinau, size_t numero_dabonnement); //numero_dabonnement = nombre d'octets pour malloc data
void deinit_tram(trame *parc_des_sports);
void affich_tram_utilisasteur(trame *etoile_bourse);
void affich_tram_hexa(trame *lixenbuhl);
int echange_trames(trame *lycee_couffignal);
//machine 1 va init trame et va prendre source son adrMac et destination l'adrMac de machine2