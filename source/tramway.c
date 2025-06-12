#include "tramway.h"

#define MAX_DATA_SIZE 1500

bool send_trame(reseau *r, trame *t, machine* equip, int port_recep);

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
    char ps[18];

    mac_to_str(etoile_bourse->src, src);
    mac_to_str(etoile_bourse->dest, dest);
    mac_to_str(etoile_bourse->passerelle, ps);

    printf("----- Trame Ethernet (mode utilisateur) -----\n");
    printf("Adresse source      : %s\n", src);
    printf("Adresse destination : %s\n", dest);
    printf("Taille données      : %zu octets\n", etoile_bourse->data_length);
    printf("Données               \n");
    if (etoile_bourse->tp==PING){
        paquet_IP *pkt = (paquet_IP *)etoile_bourse->data;
        char ip_src_str[16], ip_dst_str[16];
        ip_to_str(pkt->src, ip_src_str);
        ip_to_str(pkt->dst, ip_dst_str);

        printf("Protocole IP        : %s\n", pkt->p == ICMP ? "ICMP" : "Inconnu");
        printf("IP source           : %s\n", ip_src_str);
        printf("IP destination      : %s\n", ip_dst_str);
        printf("Passerelle :        : %s\n", ps);


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
    char ps[18];

    mac_to_str(lixenbuhl->src, src);
    mac_to_str(lixenbuhl->dest, dest);
    mac_to_str(lixenbuhl->passerelle, ps);
    

    printf("----- Trame Ethernet (mode hexadécimal) -----\n");
    printf("%s - %s\t passerelle : %s", src, dest, ps);
    for(size_t i = 0; i < lixenbuhl->data_length; i++){
        //printf("%02hhX:", octetlixenbuhl->data[i]);   //A FAIRE
    }
    printf("\n---------------------------------------------\n");
}

void envoie_trame(reseau *r, trame *t){
    bool machine1_existe = false;
    int id_src = -1;
    bool machine2_existe = false;

    //parcourir les machines
    for(int i=0; i<r->nbr_machines; i++){
        machine* m = &r->machines[i];
        if(m->tp_equip == TYPE_STATION){
            station * st = (station*) m->equipement;
            if(memcmp(st->st_MAC.mac, t->src.mac, 6)==0){
                machine1_existe = true;
                id_src = m->id;
            }
            else if(memcmp(st->st_MAC.mac, t->dest.mac, 6)==0){
                machine2_existe = true;
            }
        }
    }

    if(!machine1_existe || !machine2_existe){
        perror("Erreur : une des deux machines n'existe pas\n");
        return;
    }

    //trouve le switch connecté à la station source
    machine *m = NULL;
    swtch *sw = NULL;
    for (int i=0; i<r->nb_aretes; i++){
        arete art = r->aretes[i];
        machine m1 = art.m1;
        machine m2 = art.m2;
        if (m1.tp_equip == TYPE_STATION){
            station* st = (station*) m1.equipement;
            if(memcmp(st->st_MAC.mac, t->src.mac, 6)==0){
                m = &art.m2;
                sw = (swtch*) &art.m2.equipement;
            }
        }
        else if(m2.tp_equip == TYPE_STATION){
            station* st = (station*) m2.equipement;
            if(memcmp(st->st_MAC.mac, t->src.mac, 6)==0){
                m = &art.m1;
                sw = (swtch*) &art.m1.equipement;
            }
        }
    }

    if(sw == NULL){
        perror("Erreur : pas de switch connecté à la source\n");
        return;
    }

    //cherche le port du switch de réception
    int port=-1;
    for(int i=0; i<sw->port_utilises; i++){
        if(sw->connectes[i] == id_src){
            port = i;
            break;
        }
    }

    bool essai = send_trame(r, t, m, port);

    if(essai){
        printf("C'est good\n");
    }
    else{
        printf("La trame n'est pas arrivée\n");
    }

}

bool send_trame(reseau *r, trame* t, machine *equip, int port_recep){
    if(equip->tp_equip == TYPE_STATION){
        station * st = (station*) equip->equipement;
        if(memcmp(t->dest.mac, st->st_MAC.mac, 6)==0){
            printf("c'est bon\n");
            return true;
        }
        else{
            printf("c'est aps pour moi\n");
            return false;
        }
    }

    bool existe_asso_src = false;
    int port=-1;
    swtch *sw = (swtch*) equip->equipement;
    for (int i=0; i<sw->nb_asso; i++){
        if(memcmp(sw->tab_association[i].st_MAC.mac, t->src.mac, 6)==0){
            existe_asso_src = true;
            port = sw->tab_association[i].port;
            break;
        }

    }
    
    if(!existe_asso_src){
        //creer une nouvelle asso 
        sw->tab_association[sw->nb_asso] = (association) {t->src.mac, port};
        sw->nb_asso++;
    }

    bool existe_asso_dest = false;
    int port_suivant=-1;
    for (int i=0; i<sw->nb_asso; i++){
        if(memcmp(sw->tab_association[i].st_MAC.mac, t->dest.mac, 6)==0){
            existe_asso_dest = true;
            port_suivant = sw->tab_association[i].port;
            break;
        }
    }

    if(existe_asso_dest){
        int id_suivant = sw->connectes[port_suivant];
        machine *equip_suivant = &r->machines[id_suivant];
        int port = 0;
        if(equip_suivant->tp_equip == TYPE_SWITCH){
            swtch *sw_suivant = (swtch*) equip_suivant->equipement;
            for (int i=0; i<sw_suivant->port_utilises; i++){
                if(sw_suivant->connectes[i] == equip->id){
                    port = i;
                    break;
                }
            }
        }

        return send_trame(r, t, equip_suivant, port);
    }
    else{
        for(int i=0; i < sw->port_utilises; i++){
            if(i!=port_recep){
                int id_suivant = sw->connectes[i];
                machine *equip_suivant = &r->machines[id_suivant];
                int port = 0;
                if(equip_suivant->tp_equip == TYPE_SWITCH){
                    swtch *sw_suivant = (swtch*) equip_suivant->equipement;
                    for (int i=0; i<sw_suivant->port_utilises; i++){
                        if(sw_suivant->connectes[i] == equip->id){  //verif si le switch est bloqué
                            port = i;
                            break;
                        }
                    }
                }

                bool essaie = send_trame(r, t, equip_suivant, port);
                if(essaie){
                    return true;
                }   
            }
        }

        return false;
    }
}
