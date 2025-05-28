#include "machines.h"
#include <string.h>


void init_station(station *st){
    memset(st->st_IP.ip, 0, 4);
    memset(st->st_MAC.mac, 0, 6);
}

void deinit_station(station *st){

}

void init_switch(swtch *sw){

}

void deinit_switch(swtch *sw){

}

void mac_to_str(adresse_MAC M, char *str){
    sprintf(str, "%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX\n", M.mac[0], M.mac[1], M.mac[2], M.mac[3], M.mac[4], M.mac[5]);
}

void ip_to_str(adresse_IP IP, char *str){
    sprintf(str, "%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX\n", IP.ip[0], IP.ip[1], IP.ip[2], IP.ip[3]);
}

void str_to_mac(adresse_MAC * M, char *str){
    sscanf(str, "%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX\n", M->mac[0], M->mac[1], M->mac[2], M->mac[3], M->mac[4], M->mac[5]);
}

void str_to_ip(adresse_IP * IP, char *str){
    sscanf(str, "%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX\n", IP->ip[0], IP->ip[1], IP->ip[2], IP->ip[3]);
}
