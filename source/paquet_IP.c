#include "paquet_IP.h"

void init_paquet_ip(paquet_IP* pkt, Protocole protocole, adresse_IP src, adresse_IP dst, void* donnees) {
    if (pkt==NULL) return;
    pkt->p = protocole;
    pkt->src = src;
    pkt->dst = dst;
    if (donnees!=NULL){
        pkt->donnees = donnees; 
    }
}

void deinit_paquet_ip(paquet_IP *pkt) {
    if (!pkt) return;
    if (pkt->donnees) {
        free(pkt->donnees);  
        pkt->donnees = NULL;
    }
    free(pkt); 
}

paquet_IP* creation_paquet_ip(adresse_IP src, adresse_IP dst, Protocole proto, void* data, size_t data_len) {
    paquet_IP* pkt = malloc(sizeof(paquet_IP));
    if (pkt==NULL) {
        perror("Erreur d'allocation paquet IP\n");
        return NULL;
    }

    pkt->p = proto;
    pkt->src = src;
    pkt->dst = dst;

    pkt->donnees = NULL;
    if (data && data_len > 0) {
        pkt->donnees = malloc(data_len);
        if (pkt->donnees==NULL) {
            perror("Erreur d'allocation des données IP\n");
            free(pkt);
            return NULL;
        }
        memcpy(pkt->donnees, data, data_len);
    }

    return pkt;
}

paquet_ICMP* creation_paquet_icmp(Type tp){
    paquet_ICMP * pkt = malloc(sizeof(paquet_ICMP));
    if (pkt==NULL){
        perror("Erreur d'allocation paquet ICMP\n");
        return NULL;
    }

    pkt->type = tp;
    return pkt;
}
