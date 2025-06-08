#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>


typedef u_int8_t octet;
typedef struct adresse_MAC{
    octet mac[6];
} adresse_MAC;

typedef struct adresse_IP{
    octet ip[4];
} adresse_IP;

typedef struct station
{
    adresse_MAC st_MAC;
    adresse_IP st_IP;
} station;

typedef struct association
{
    adresse_MAC st_MAC;
    int port;
} association;

typedef struct swtch
{
    adresse_MAC sw_MAC;
    int nb_port;
    uint16_t priorite;    //2 octets de pt fort
    association *tab_association; //malloc * TAILLE NB PORTS
    int port_utilises; // compteur 
} swtch;

typedef enum typeEquipement{
    TYPE_STATION,
    TYPE_SWITCH
}typeEquipement;

typedef struct machine{
    int id;
    typeEquipement tp_equip;
    void *equipement;

} machine;


void init_station(station *st);
void deinit_station(station *st);
void init_switch(swtch *sw);
void deinit_switch(swtch *sw);
void mac_to_str(adresse_MAC M, char *str);
void ip_to_str(adresse_IP IP, char *str);
void str_to_mac(adresse_MAC *M, char *str);
void str_to_ip(adresse_IP *IP, char *str);