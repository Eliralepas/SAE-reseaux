#include <stdio.h>
#include <stdlib.h>


typedef u_int8_t octet;
typedef struct adresse_MAC{
    typedef octet mac[6];
};

typedef struct adresse_IP{
    typedef octet ip[4];
};

typedef struct station
{
    adresse_MAC st_MAC;
    adresse_IP st_IP;
};

typedef struct association
{
    adresse_MAC st_MAC;
    int port;
};

typedef struct swtch
{
    adresse_MAC sw_mac;
    int nb_port;
    octet *priorite;
    association *tab_association; //malloc * TAILLE NB PORTS
};



void mac_to_str(adresse_MAC M, char *str);
