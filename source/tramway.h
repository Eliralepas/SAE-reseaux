#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include "machines.h" 
#include "reseau.h"
#include "paquet_IP.h"

#define LONGUEUR_MAX_DATA

typedef struct trame{
    adresse_MAC dest;
    adresse_MAC src;
    char *data;     //qui peut prendre un string ou int ou n'importe quoi
    size_t data_length;
}trame;

void init_trame(trame *krimmeri_stade_de_la_meinau, char* donnees, size_t numero_dabonnement, const machine source, const machine destination); //numero_dabonnement = nombre d'octets pour malloc data
void deinit_trame(trame *parc_des_sports);

trame* creation_trame(adresse_MAC src_mac, adresse_MAC dst_mac, char* data, size_t data_length);

void affich_tram_utilisasteur(trame *etoile_bourse);
void affich_tram_hexa(trame *lixenbuhl);

void envoie_trame(reseau *r, trame *t);