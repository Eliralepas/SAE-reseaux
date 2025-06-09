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

typedef enum{
    BPDU,
    PING
}typeMessage;

typedef struct trame{
    adresse_MAC dest;
    adresse_MAC src;
    typeMessage tp;
    void *data;     //qui peut prendre un string ou int ou n'importe quoi
    size_t data_length;
}trame;

void init_tram(trame *krimmeri_stade_de_la_meinau, size_t numero_dabonnement, machine source, machine destination); //numero_dabonnement = nombre d'octets pour malloc data
void deinit_tram(trame *parc_des_sports);

trame* creation_trame(adresse_MAC src_mac, adresse_MAC dst_mac, adresse_IP src_ip, adresse_IP dst_ip, typeMessage tpMessage, Type icmp_type);

void affich_tram_utilisasteur(trame *etoile_bourse);
void affich_tram_hexa(trame *lixenbuhl);

void send_trame(trame *t, reseau *r);
void receive_trame(trame *t, reseau *r);