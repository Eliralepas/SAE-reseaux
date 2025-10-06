#include "machines.h"
#include <string.h>


void init_station(station *st){
    //pratique vu qu'une machine a 0.0.0.0 en IP avant de s'en voir attribuée une
    if (st==NULL) return;
    memset(st->st_IP.ip, 0, 4);
    memset(st->st_MAC.mac, 0, 6);
}

//free les alloc s'il y en a et met les valeurs à 0/NULL
void deinit_station(station *st){
    if(st==NULL) return;
    memset(st->st_IP.ip, 0, 4);
    memset(st->st_MAC.mac, 0, 6);
}

void init_switch(swtch *sw, int nb_equip, int nb_port){
    if (sw==NULL) return;
    memset(sw->sw_MAC.mac, 0, 6);

    sw->nb_port = nb_port; //valeur totalement arbitraire, sujette à débat
    sw->priorite = 0;
    sw->nb_asso = 0;
    sw->port_utilises = 0;

    sw->tab_association = malloc(sizeof(association) * (nb_equip-1));

    sw->port_etat = malloc(sizeof(etatPort) * sw->nb_port);
    for (int p = 0; p<sw->nb_port; p++){
        sw->port_etat[p] = DESIGNE;
    }

    //tab connecté 
    sw->connectes = malloc(sizeof(int) * sw->nb_port);
    if (sw->connectes==NULL) {
        perror("Erreur allocation connectes");
        return;
    }
    for (int p = 0; p < sw->nb_port; p++) {
        sw->connectes[p] = -1; 
    }
    
    sw->bridge_protocol.cost = 0;
    sw->bridge_protocol.bridge_id = concat_bridge_id(sw);
    sw->bridge_protocol.root_id = sw->bridge_protocol.bridge_id;
}

//free les alloc s'il y en a et met les valeurs à 0/NULL
void deinit_switch(swtch *sw){
    if (sw==NULL) return;

    memset(sw->sw_MAC.mac, 0, 6);

    if (sw->tab_association!=NULL) {
        free(sw->tab_association);
        sw->tab_association = NULL;
    }

    if (sw->port_etat!=NULL) {
        free(sw->port_etat);
        sw->port_etat = NULL;
    }

    if (sw->connectes!=NULL) {
        free(sw->connectes);
        sw->connectes = NULL;
    }

    sw->nb_asso = 0;
    sw->port_utilises = 0;

    sw->bridge_protocol.root_id = 0;
    sw->bridge_protocol.bridge_id = 0;
    sw->bridge_protocol.cost = 0;
    
    free(sw->port_etat);
    sw->port_etat = NULL;
    

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

uint64_t concat_bridge_id(swtch *sw) {
    return ((uint64_t)sw->priorite << 48) |
           ((uint64_t)sw->sw_MAC.mac[0] << 40) |
           ((uint64_t)sw->sw_MAC.mac[1] << 32) |
           ((uint64_t)sw->sw_MAC.mac[2] << 24) |
           ((uint64_t)sw->sw_MAC.mac[3] << 16) |
           ((uint64_t)sw->sw_MAC.mac[4] << 8)  |
           ((uint64_t)sw->sw_MAC.mac[5] << 0)  ;  //ça me stressait les trucs | et le ; pas alignés
}