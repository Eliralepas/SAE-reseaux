#include "tramway.h"

#define MAX_DATA_SIZE 1500

void init_tram(trame *krimmeri_stade_de_la_meinau, size_t numero_dabonnement,
    const machine source, const machine destination){
    adresse_MAC dest; 
    adresse_MAC src;

    //Adresse MAC DESTINATION
    if(destination.tp_equip == TYPE_STATION){
        station* mach = (station*) destination.equipement;
        dest = mach->st_MAC;
    }
    else{
        swtch* mach = (swtch*) destination.equipement;
        dest = mach->sw_MAC;
    }
    krimmeri_stade_de_la_meinau->dest = dest;

    //Adresse MAC SOURCE
    if(source.tp_equip == TYPE_STATION){
        station* mach = (station*) source.equipement;
        src = mach->st_MAC;
    }
    else{
        swtch* mach = (swtch*) source.equipement;
        src = mach->sw_MAC;
    }
    krimmeri_stade_de_la_meinau->src = src;
    memcpy(krimmeri_stade_de_la_meinau->passerelle.mac, src.mac, sizeof(adresse_MAC));

    //Longueur et allocation de Data
    if(numero_dabonnement > MAX_DATA_SIZE){
        perror("Erreur : Data trop longue\n");
        return;
    }
    krimmeri_stade_de_la_meinau->data_length = numero_dabonnement;
    krimmeri_stade_de_la_meinau->data = malloc(sizeof(octet)*krimmeri_stade_de_la_meinau->data_length);
}

void deinit_tram(trame *parc_des_sports){
    memset(parc_des_sports->dest.mac, 0, 6);
    memset(parc_des_sports->src.mac, 0, 6);
    memset(parc_des_sports->passerelle.mac, 0, 6);
    if (parc_des_sports->data != NULL) {
        deinit_paquet_ip((paquet_IP*)parc_des_sports->data);
    }
    else{
        free(parc_des_sports->data);
    }
    parc_des_sports->data = NULL;
}

trame* creation_trame(adresse_MAC src_mac, adresse_MAC dst_mac, adresse_MAC passerelle, adresse_IP src_ip, adresse_IP dst_ip, typeMessage tpMessage, Type icmp_type) {
    trame* t = malloc(sizeof(trame));
    if (t==NULL) {
        perror("Erreur d'allocation pour la trame\n");
        return NULL;
    }

    t->src = src_mac;
    t->dest = dst_mac;
    t->passerelle = passerelle;
    t->tp = tpMessage;
    t->data = NULL;
    t->data_length = 0;

    if (tpMessage == PING) {
        paquet_ICMP *icmp = creation_paquet_icmp(icmp_type);
        if (icmp==NULL) {
            free(t);
            return NULL;
        }

        paquet_IP *ip = creation_paquet_ip(src_ip, dst_ip, ICMP, icmp, sizeof(paquet_ICMP));
        free(icmp);

        if (ip==NULL) {
            free(t);
            return NULL;
        }

        t->data = ip;
        t->data_length = sizeof(paquet_IP);
    }

    return t;
}

void affich_tram_utilisasteur(trame *etoile_bourse){
    if (etoile_bourse==NULL) return;

    char src[18];
    char dest[18];

    mac_to_str(etoile_bourse->src, src);
    mac_to_str(etoile_bourse->dest, dest);

    printf("----- Trame Ethernet (mode utilisateur) -----\n");
    printf("Adresse source      : %s\n", src);
    printf("Adresse destination : %s\n", dest);
    printf("Taille données      : %zu octets\n", etoile_bourse->data_length);
    printf("Données             : \n");
    if (etoile_bourse->tp==PING){
        paquet_IP *pkt = (paquet_IP *)etoile_bourse->data;
        char ip_src_str[16], ip_dst_str[16];
        ip_to_str(pkt->src, ip_src_str);
        ip_to_str(pkt->dst, ip_dst_str);

        printf("Protocole IP        : %s\n", pkt->p == ICMP ? "ICMP" : "Inconnu");
        printf("IP source           : %s\n", ip_src_str);
        printf("IP destination      : %s\n", ip_dst_str);

        if (pkt->p == ICMP) {
            paquet_ICMP *icmp = (paquet_ICMP *)pkt->donnees;
            printf("Type ICMP           : ");
            switch (icmp->type) {
                case ICMP_ECHO_REQUEST:
                    printf("Echo Request (Ping)\n");
                    break;
                case ICMP_ECHO_REPLY:
                    printf("Echo Reply (Pong)\n");
                    break;
                case ICMP_DEST_UNREACHABLE:
                    printf("Destination Unreachable\n");
                    break;
                default:
                    printf("Inconnu (%d)\n", icmp->type);
            }
        }
    }
    printf("\n---------------------------------------------\n");
}

void affich_tram_hexa(trame *lixenbuhl){
    if(lixenbuhl==NULL) return;

    char src[18];
    char dest[18];

    mac_to_str(lixenbuhl->src, src);
    mac_to_str(lixenbuhl->dest, dest);

    printf("----- Trame Ethernet (mode hexadécimal) -----\n");
    printf("%s : %s", src, dest);
    for(size_t i = 0; i < lixenbuhl->data_length; i++){
        //printf("%02hhX:", octetlixenbuhl->data[i]);   //A FAIRE
    }
    printf("\n---------------------------------------------\n");
}

void send_trame(trame *t, reseau *r) {
    if (t == NULL || r == NULL) {
        perror("Erreur: trame ou réseau invalide\n");
        return;
    }

    //on cherche la machine passerelle dans le réseau
    machine *ps = NULL;
    for (size_t i = 0; i < r->nbr_machines; i++) {
        machine *m = &r->machines[i];
        if ((m->tp_equip == TYPE_STATION && memcmp(((station*)m->equipement)->st_MAC.mac, t->passerelle.mac, 6) == 0) ||
            (m->tp_equip == TYPE_SWITCH && memcmp(((swtch*)m->equipement)->sw_MAC.mac, t->passerelle.mac, 6) == 0)) {
            ps = m;
            break;
        }
    }

    if (ps == NULL) {
        perror("Erreur: machine source introuvable\n");
        return;
    }

    //afficher le message ICMP
    if (t->tp == PING && t->data != NULL) {
        paquet_IP *pkt = (paquet_IP *)t->data;
        if (pkt->p == ICMP && pkt->donnees != NULL) {
            paquet_ICMP *icmp = (paquet_ICMP *)pkt->donnees;
            char ip_str[16];
            ip_to_str(pkt->dst, ip_str);

            char mac_pas[16];
            mac_to_str(t->passerelle, mac_pas);
            
            if (icmp->type == ICMP_ECHO_REQUEST) {
                printf(">>> Envoi Ping vers %s... de %s\n", ip_str, mac_pas);
            } else if (icmp->type == ICMP_ECHO_REPLY) {
                printf(">>> Réponse Ping de %s\n", ip_str);
            }
        }
    }

    // Vérifier si la destination est connue
    bool dest_connue = false;
    int port_dest = -1;

    //on trouve le port
    if(ps->tp_equip == TYPE_SWITCH){
        swtch* sw_ps = (swtch*) ps->equipement;
        int p=0;
        while (!dest_connue && p < sw_ps->port_utilises){
            if (memcmp(sw_ps->tab_association[p].st_MAC.mac, t->dest.mac, 6) == 0) {
                dest_connue = true;
                port_dest = sw_ps->tab_association[p].port;
            }
            p++;
        }
        
        // si destination connue et port actifc
        if (dest_connue && sw_ps->port_etat[port_dest] != BLOQUE) {
            printf("TRANSMETTEUR : Switch %d vers port %d (MAC connu)\n", ps->id, port_dest);
            receive_trame(t, r);
        }
        else{
            int nb_voisins = nb_voisin(ps, r);
            machine voisins[nb_voisins];
            tab_voisin(ps, voisins, r);

            //on parcourt ses voisins
            for (int i=0; i<nb_voisins; i++){
                machine *voisin = &voisins[i];
                if (voisin->tp_equip == TYPE_SWITCH) {
                    swtch * sw_voisin = (swtch*) voisin->equipement;
                    printf(">>> BROADCAST : Switch %d\n", voisin->id);
                    t->passerelle = sw_voisin->sw_MAC;
                    for (int j = 0; j < sw_voisin->nb_port; j++) {
                        if (sw_voisin->port_etat[j] != BLOQUE && memcmp(t->passerelle.mac, sw_ps->sw_MAC.mac, 6)!=0) {
                            send_trame(t, r);
                        }
                    }
                }
            }
        }
    }
    else if (ps->tp_equip == TYPE_STATION) {
        for (size_t i = 0; i < r->nb_aretes; i++) {
            arete a = r->aretes[i];

            if (a.m1.id == ps->id){
                machine *m_voisin = &a.m2;
                t->passerelle = ((swtch*)m_voisin->equipement)->sw_MAC;
                send_trame(t, r);
            }
            else if (a.m2.id == ps->id)
            {
                machine *m_voisin = &a.m1;
                t->passerelle = ((swtch*)m_voisin->equipement)->sw_MAC;
                send_trame(t, r);
            }
            
        }
    }
}


void receive_trame(trame *t, reseau *r) {
    if (t == NULL || r == NULL) return;

    //chercher la machine avec l'adresse MAC de destination
    machine *dest_machine = NULL;

    for (size_t i = 0; i < r->nbr_machines; i++) {
        if (r->machines[i].tp_equip == TYPE_STATION) {
            station *st = (station*) r->machines[i].equipement;
            if (memcmp(st->st_MAC.mac, t->dest.mac, 6) == 0) {
                dest_machine = &r->machines[i];
                break;
            }
        } else if (r->machines[i].tp_equip == TYPE_SWITCH) {
            swtch *sw = (swtch*) r->machines[i].equipement;
            if (memcmp(sw->sw_MAC.mac, t->dest.mac, 6) == 0) {
                dest_machine = &r->machines[i];
                break;
            }
        }
    }

    if (dest_machine==NULL) {
        printf("Aucune machine ne correspond à l'adresse MAC destination.\n");   //NORMALEMENT CA NE PEUT PAS ARRIVER MAIS AU CAS OU PETER MON CRANE
        return;
    }

    printf("Machine %d a reçu une trame !\n", dest_machine->id);
  

    paquet_IP* pkt = (paquet_IP*) t->data;
    if (pkt->p == ICMP) {
        paquet_ICMP *icmp = (paquet_ICMP*) pkt->donnees;
        switch (icmp->type) {
            case ICMP_ECHO_REQUEST:
                printf("\n=======Reçu un ICMP ECHO REQUEST (ping).=======\n\n");

                //inverser adresses
                adresse_MAC mac_src_reply;
                adresse_MAC mac_dst_reply;
                adresse_IP ip_src_reply;
                adresse_IP ip_dst_reply;

                if (dest_machine->tp_equip == TYPE_STATION) {
                    station* st = (station*)dest_machine->equipement;
                    mac_src_reply = st->st_MAC;
                    ip_src_reply = st->st_IP;
                } 
                else {
                    swtch* sw = (swtch*)dest_machine->equipement;
                    mac_src_reply = sw->sw_MAC;
                    ip_src_reply = pkt->dst;  // approximation (pas d'IP pour switch)
                }

                mac_dst_reply = t->src;
                ip_dst_reply = pkt->src;

                // Créer trame de réponse (pong)
                trame *reply = creation_trame(mac_src_reply, mac_dst_reply, mac_src_reply ,ip_src_reply, ip_dst_reply, PING, ICMP_ECHO_REPLY);
                if (reply==NULL) {
                    perror("Erreur lors de la création du pong.\n");
                    break;
                }

                send_trame(reply, r);
                deinit_tram(reply);
                free(reply);
                break;

            case ICMP_ECHO_REPLY:
                printf("\n=======Reçu un ICMP ECHO REPLY (pong).=======\n\n");
                break;
            case ICMP_DEST_UNREACHABLE:
                printf("\n=======Reçu ICMP DESTINATION UNREACHABLE.=======\n\n");
                break;
            default:
                printf("ICMP inconnu.\n");
        }
    }
    else {
        printf("Paquet IP avec protocole non supporté : %d\n", pkt->p);
    }
}

