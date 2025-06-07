#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include "machines.h" 
#include "reseaux.h"
#include "tramway.h"

typedef enum Protocole{
    ICMP = 1
} Protocole;

typedef struct paquet_IP{
    uint8_t TTL;
    Protocole p;
    Adresse_IP src;
    Adresse_IP dst;
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