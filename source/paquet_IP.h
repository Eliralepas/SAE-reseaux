#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <string.h>

#include "machines.h" 
#include "reseau.h"
#include "machines.h"

typedef enum Protocole{
    ICMP
} Protocole;

typedef struct paquet_IP{
    Protocole p;
    adresse_IP src;
    adresse_IP dst;
    void* donnees;
} paquet_IP;

typedef enum Type{
    ICMP_ECHO_REPLY = 0,   // TYPE 0
    ICMP_DEST_UNREACHABLE = 3, // TYPE 3
    ICMP_ECHO_REQUEST = 8  // TYPE 8
} Type;

typedef struct paquet_ICMP{
    Type type;
}paquet_ICMP;

void deinit_paquet_ip(paquet_IP *pkt);
void init_paquet_ip(paquet_IP* pkt, Protocole protocole, adresse_IP src, adresse_IP dst, void* donnees);
paquet_IP* creation_paquet_ip(adresse_IP src, adresse_IP dst, Protocole proto, void* data, size_t data_len);
paquet_ICMP* creation_paquet_icmp(Type tp);