#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>


typedef u_int8_t octet;
typedef struct addr_MAC{
    octet mac[6];
} adresse_MAC;

typedef struct addr_IP{
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
    adresse_MAC sw_mac;
    int nb_port;
    octet *priorite;
    association *tab_association; //malloc * TAILLE NB PORTS
    adresse_MAC tab_voisins[];
};



void init_station(station *st);
void deinit_station(station *st);
void init_switch(swtch *sw);
void deinit_switch(swtch *sw);
void mac_to_str(adresse_MAC M, char *str);
void ip_to_str(adresse_IP, char *str);