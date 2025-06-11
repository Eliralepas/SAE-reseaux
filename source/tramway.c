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
    if (t==NULL || r==NULL) return;

    //trouver la machine source pour le tableau de voisin
    machine *src = NULL;
    for (size_t i = 0; i < r->nbr_machines; i++){
        machine *m = &r->machines[i];

        if (m->tp_equip == TYPE_STATION) {
            station *st = (station *)m->equipement;
            if (memcmp(st->st_MAC.mac, t->src.mac, 6) == 0) {
                src = m;
                break;
            }
        } else if (m->tp_equip == TYPE_SWITCH) {
            swtch *sw = (swtch *)m->equipement;
            if (memcmp(sw->sw_MAC.mac, t->src.mac, 6) == 0) {
                src = m;
                break;
            }
        }
    }

    if (src==NULL){
        printf("Erreur : machine source introuvable.\n");
        return;
    }

    //décodage par rapport type du message
    if (t->tp == PING){
        paquet_IP* pkt = (paquet_IP*) t->data;
        paquet_ICMP* icmp = (paquet_ICMP*) pkt->donnees;
        switch (icmp->type)
        {
        case ICMP_ECHO_REQUEST:
            char str[16];
            ip_to_str(pkt->dst, str);
            printf("Envoie d'une requête 'Ping' %s avec %ld octets de données :\n", str, t->data_length);
            break;
        
        case ICMP_ECHO_REPLY:
            ip_to_str(pkt->dst, str);
            printf("Réponse de %s\n", str);
            break;  
        
        case ICMP_DEST_UNREACHABLE: 
            printf("Destination introuvable.\n");
            break;
        
        default:
            break;
        }
    }

    //envoyer la trame aux voisins
    int nb = nb_voisin(src, r);
    machine voisins [nb];
    tab_voisin(src, voisins, r);

    for (int i = 0; i < nb; i++) {
        machine *dest = &voisins[i];

        //si le destinataire est un switch -> propager la trame
        if (dest->tp_equip == TYPE_SWITCH) {
            swtch *sw = (swtch *)dest->equipement;

            //MAC source connu par la switch ??
            bool connu = false;
            for (int j = 0; j < sw->port_utilises; j++) {
                if (memcmp(sw->tab_association[j].st_MAC.mac, t->src.mac, 6) == 0){
                    connu = true;
                    break;
                }
            }
            //s'il ne le connait pas on le rajoute a la tab de commutation
            if (!connu && sw->port_utilises < sw->nb_port) {
                sw->tab_association[sw->port_utilises].st_MAC = t->src;
                sw->tab_association[sw->port_utilises].port = sw->port_utilises;
                sw->port_utilises++;
                printf("Table de commutation mise à jour (machine %d).\n", dest->id);
                affichage_tab_commutation(sw);
            }

            //MAC dest connu ?
            bool mac_dest_connue = false;
            for (int j = 0; j < sw->port_utilises; j++) {
                if (memcmp(sw->tab_association[j].st_MAC.mac, t->dest.mac, 6) == 0){
                    mac_dest_connue = true;
                    break;
                }
            }

            if (mac_dest_connue) {
                //on l'envoie au bon voisin
                receive_trame(t, r);
            } 
            else {
                // Diffusion : le switch envoie à tous ses voisins sauf la source
                int nb_sw = nb_voisin(dest, r);
                machine voisins_sw [nb_sw];
                tab_voisin(dest, voisins_sw, r);

                for (int k = 0; k < nb_sw; k++) {
                    if (voisins_sw[k].id != src->id) {
                        receive_trame(t, r); //propager à tous sauf retour source !!
                    }
                }
            }

        } 
        else {
            //destinataire est une station ? on essaie de lui envoyer directement
            receive_trame(t, r);
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

