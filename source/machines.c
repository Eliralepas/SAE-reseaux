#include "machines.h"
#include <string.h>


void init_station(station *st){
    //pratique vu qu'une machine a 0.0.0.0 en IP avant de s'en voir attribuée une
    memset(st->st_IP.ip, 0, 4);
    memset(st->st_MAC.mac, 0, 6);
}

void deinit_station(station *st){
    //Je pense que faut réutiliser cette meme commande vu qu'on veut les repasser à 0
    memset(st->st_IP.ip, 0, 4);
    memset(st->st_MAC.mac, 0, 6);
}

void init_switch(swtch *sw){
    memset(sw->sw_MAC.mac, 0, 6);
    sw->nb_port = 16; //valeur totalement arbitraire, sujette à débat
    sw->priorite = 0;
    sw->tab_association = (sizeof(association) * sw->nb_port);
    //tab voisin mais on a oublié son utilité MDRRR...
}

void deinit_switch(swtch *sw){
    memset(sw->sw_MAC.mac, 0, 6);
    free(sw->tab_association);
    sw->tab_association = NULL;
}

void mac_to_str(adresse_MAC M, char *str){
    sprintf(str, "%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX", M.mac[0], M.mac[1], M.mac[2], M.mac[3], M.mac[4], M.mac[5]);
}

void ip_to_str(adresse_IP IP, char *str){
    sprintf(str, "%hhu.%hhu.%hhu.%hhu", IP.ip[0], IP.ip[1], IP.ip[2], IP.ip[3]);
}

void str_to_mac(adresse_MAC * M, char *str){
    sscanf(str, "%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX", &M->mac[0], &M->mac[1], &M->mac[2], &M->mac[3], &M->mac[4], &M->mac[5]);
}

void str_to_ip(adresse_IP * IP, char *str){
    sscanf(str, "%hhu.%hhu.%hhu.%hhu", &IP->ip[0], &IP->ip[1], &IP->ip[2], &IP->ip[3]);
}

