#include "paquet_IP.h"

void init_paquet_ip(paquet_IP* pkt, uint8_t ttl, Protocole protocole, adresse_IP src, adresse_IP dst, void* donnees) {
    if (pkt==NULL) return;

    pkt->TTL = 30;
    pkt->p = protocole;
    pkt->src = src;
    pkt->dst = dst;
    if (donnees!=NULL){
        pkt->donnees = donnees; 
    }
}