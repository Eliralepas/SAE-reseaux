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

    //Adresse MAC SOURCE
    if(source.tp_equip == TYPE_STATION){
        station* mach = (station*) source.equipement;
        dest = mach->st_MAC;
    }
    else{
        swtch* mach = (swtch*) source.equipement;
        dest = mach->sw_MAC;
    }

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
    if (parc_des_sports->data != NULL) {
        deinit_paquet_ip((paquet_IP*)parc_des_sports->data);
    }
    else{
        free(parc_des_sports->data);
    }
    parc_des_sports->data = NULL;
}

trame* creation_trame(adresse_MAC src_mac, adresse_MAC dst_mac, adresse_IP src_ip, adresse_IP dst_ip, typeMessage tpMessage, Type icmp_type) {
    trame* t = malloc(sizeof(trame));
    if (t==NULL) {
        perror("Erreur d'allocation pour la trame\n");
        return NULL;
    }

    t->src = src_mac;
    t->dest = dst_mac;
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
    printf("Données             : ");
    for (size_t i = 0; i < etoile_bourse->data_length; i++) {
        //printf("%02hhX:", etoile_bourse->data[i]);        //A FAIRE
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

    // 1. Trouver la machine source
    machine *src = NULL;
    for (size_t i = 0; i < r->nbr_machines; i++) {
        machine *m = &r->machines[i];
        if ((m->tp_equip == TYPE_STATION && memcmp(((station*)m->equipement)->st_MAC.mac, t->src.mac, 6) == 0) ||
            (m->tp_equip == TYPE_SWITCH && memcmp(((swtch*)m->equipement)->sw_MAC.mac, t->src.mac, 6) == 0)) {
            src = m;
            break;
        }
    }

    if (src == NULL) {
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
            
            if (icmp->type == ICMP_ECHO_REQUEST) {
                printf("Envoi Ping vers %s...\n", ip_str);
            } else if (icmp->type == ICMP_ECHO_REPLY) {
                printf("Réponse Ping de %s\n", ip_str);
            }
        }
    }

    // obtenir les voisins directs
    int nb_voisins = nb_voisin(src, r);
    machine voisins[nb_voisins];
    tab_voisin(src, voisins, r);

    bool trame_envoyee = false;

    for (int i = 0; i < nb_voisins; i++) {
        machine *voisin = &voisins[i];

        //si le voisin est un switch
        if (voisin->tp_equip == TYPE_SWITCH) {
            swtch *sw = (swtch *)voisin->equipement;

            // Mise à jour table de commutation si source inconnue
            bool src_connue = false;
            for (int j = 0; j < sw->port_utilises; j++) {
                if (memcmp(sw->tab_association[j].st_MAC.mac, t->src.mac, 6) == 0) {
                    src_connue = true;
                    break;
                }
            }

            if (!src_connue && sw->port_utilises < sw->nb_port) {
                int port_libre = sw->port_utilises;
                sw->tab_association[port_libre].st_MAC = t->src;
                sw->tab_association[port_libre].port = port_libre;
                sw->port_etat[port_libre] = DESIGNE;
                sw->port_utilises++;
                
                printf("Switch %d: Apprentissage MAC ", voisin->id);
                for (int k = 0; k < 6; k++) printf("%02X:", t->src.mac[k]);
                printf(" sur port %d\n", port_libre);
            }

            // Vérifier si la destination est connue
            bool dest_connue = false;
            int port_dest = -1;

            for (int j = 0; j < sw->port_utilises; j++) {
                if (memcmp(sw->tab_association[j].st_MAC.mac, t->dest.mac, 6) == 0) {
                    dest_connue = true;
                    port_dest = sw->tab_association[j].port;
                    break;
                }
            }

            // si destination connue et port actif
            if (dest_connue && sw->port_etat[port_dest] != BLOQUE) {
                printf("Switch %d: Forwarding vers port %d (MAC connu)\n", voisin->id, port_dest);
                receive_trame(t, r);
                trame_envoyee = true;
                break;
            } 
            // sinon, broadcast sur ports actifs
            else {
                printf("Switch %d: Broadcast (MAC inconnu)\n", voisin->id);
                for (int j = 0; j < sw->port_utilises; j++) {
                    if (sw->port_etat[j] != BLOQUE) {
                        for (size_t k = 0; k < r->nbr_machines; k++) {
                            machine *m = &r->machines[k];
                            if (m->tp_equip == TYPE_STATION) {
                                station *st = (station *)m->equipement;
                                if (memcmp(sw->tab_association[j].st_MAC.mac, st->st_MAC.mac, 6) == 0 &&
                                    memcmp(st->st_MAC.mac, t->src.mac, 6) != 0) {
                                    receive_trame(t, r);
                                    trame_envoyee = true;
                                }
                            }
                        }
                    }
                }
            }
        }
        // si le voisin est une station et c'est la destination
        else if (voisin->tp_equip == TYPE_STATION) {
            station *st = (station *)voisin->equipement;
            if (memcmp(st->st_MAC.mac, t->dest.mac, 6) == 0) {
                printf("Destination trouvée: Station %d\n", voisin->id);
                receive_trame(t, r);
                trame_envoyee = true;
                break;
            }
        }
    }

    if (!trame_envoyee) {
        perror("Erreur: Aucun chemin actif vers la destination\n");

        printf("Source MAC: ");
        for (int i = 0; i < 6; i++) printf("%02X:", t->src.mac[i]);
        printf("\nDest MAC: ");
        for (int i = 0; i < 6; i++) printf("%02X:", t->dest.mac[i]);
        printf("\n");
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

    //printf("Machine %d a reçu une trame !\n", dest_machine->id);
    //affich_tram_utilisasteur(t);

    paquet_IP* pkt = (paquet_IP*) t->data;
    if (pkt->p == ICMP) {
        paquet_ICMP *icmp = (paquet_ICMP*) pkt->donnees;
        switch (icmp->type) {
            case ICMP_ECHO_REQUEST:
                printf("Reçu un ICMP ECHO REQUEST (ping).\n");

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
                trame *reply = creation_trame(mac_src_reply, mac_dst_reply, ip_src_reply, ip_dst_reply, PING, ICMP_ECHO_REPLY);

                if (reply==NULL) {
                    perror("Erreur lors de la création du pong.\n");
                    break;
                }

                send_trame(reply, r);
                deinit_tram(reply);
                free(reply);
                break;

            case ICMP_ECHO_REPLY:
                printf("Reçu un ICMP ECHO REPLY (pong).\n");
                break;
            case ICMP_DEST_UNREACHABLE:
                printf("Reçu ICMP DESTINATION UNREACHABLE.\n");
                break;
            default:
                printf("ICMP inconnu.\n");
        }
    }
    else {
        printf("Paquet IP avec protocole non supporté : %d\n", pkt->p);
    }
}

