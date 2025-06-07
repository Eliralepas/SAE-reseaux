#include "paquet_IP.h"

void init_paquet_ip(paquet_IP* pkt, uint8_t ttl, Protocole protocole, Adresse_IP src, Adresse_IP dst, void* donnees) {
    if (pkt==NULL) return;

    pkt->TTL = ttl;
    pkt->p = protocole;
    pkt->src = src;
    pkt->dst = dst;
    if (donnees!=NULL){
        pkt->donnees = donnees; 
    }
}