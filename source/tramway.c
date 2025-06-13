#include "tramway.h"

#define MAX_DATA_SIZE 1500

bool send_trame(reseau *r, trame *t, machine* equip, int port_recep, int id_precedent);

void init_trame(trame *krimmeri_stade_de_la_meinau, char* donnees, size_t numero_dabonnement, const machine source, const machine destination){
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

    //Longueur et allocation de Data
    if(numero_dabonnement > MAX_DATA_SIZE){
        perror("Erreur : Data trop longue\n");
        return;
    }
    krimmeri_stade_de_la_meinau->data_length = numero_dabonnement;
    krimmeri_stade_de_la_meinau->data = donnees;

}

void deinit_trame(trame *parc_des_sports){
    memset(parc_des_sports->dest.mac, 0, 6);
    memset(parc_des_sports->src.mac, 0, 6);
    parc_des_sports->data_length = 0;
    parc_des_sports->data = NULL;
}

trame* creation_trame(adresse_MAC src_mac, adresse_MAC dst_mac, char* data, size_t data_length) {
    trame* t = malloc(sizeof(trame));
    if (t==NULL) {
        perror("Erreur d'allocation pour la trame\n");
        return NULL;
    }

    t->src = src_mac;
    t->dest = dst_mac;
    t->data = data;
    t->data_length = data_length;

    return t;
}

void affich_tram_utilisasteur(trame *etoile_bourse){
    if (etoile_bourse==NULL) return;

    char src[18];
    char dest[18];
    char ps[18];

    mac_to_str(etoile_bourse->src, src);
    mac_to_str(etoile_bourse->dest, dest);

    printf("----- Trame Ethernet (mode utilisateur) -----\n");
    printf("Adresse source      : %s\n", src);
    printf("Adresse destination : %s\n", dest);
    printf("Taille données      : %zu octets\n", etoile_bourse->data_length);
    printf("Données (message)   : %s\n", (char*) etoile_bourse->data);    
    printf("\n---------------------------------------------\n");
}

void affich_tram_hexa(trame *lixenbuhl){
    if(lixenbuhl==NULL) return;

    char src[18];
    char dest[18];

    mac_to_str(lixenbuhl->src, src);
    mac_to_str(lixenbuhl->dest, dest);
    

    printf("----- Trame Ethernet (mode hexadécimal) -----\n");
    printf("%s - %s\t", src, dest);
    printf("%s", lixenbuhl->data);   //A FAIRE
    
    printf("\n---------------------------------------------\n");
}

void envoie_trame(reseau *r, trame *t){
    bool machine1_existe = false;
    int id_src = -1;
    bool machine2_existe = false;

    //parcourir les machines
    for(int i=0; i<r->nb_machines; i++){
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
        arete *art = &r->aretes[i];
        machine *m1 = art->m1;
        machine *m2 = art->m2;
        if (m1->tp_equip == TYPE_STATION){
            station* st = (station*) m1->equipement;
            if(memcmp(st->st_MAC.mac, t->src.mac, 6)==0){
                m = art->m2;
                sw = (swtch*) art->m2->equipement;
                break;
            }
        }
        else if(m2->tp_equip == TYPE_STATION){
            station* st = (station*) m2->equipement;
            if(memcmp(st->st_MAC.mac, t->src.mac, 6)==0){
                m = art->m1;
                sw = (swtch*) art->m1->equipement;
                break;

            }
        }
    }

    if(sw == NULL){
        perror("Erreur : pas de switch connecté à la source\n");
        return;
    }

    //cherche le port du switch de réception
    int port=-1;
    for(int i=0; i<sw->nb_port; i++){
        if(sw->connectes[i] == id_src){
            port = i;
            break;
        }
    }

    bool essai = send_trame(r, t, m, port, id_src);

    if(essai){
        printf("La trame est bien été reçue!\n");
    }
    else{
        printf("La trame n'est pas arrivée à destination.\n");
    }

}

bool send_trame(reseau *r, trame* t, machine *equip, int port_recep, int id_precedent){
    if(equip->tp_equip == TYPE_STATION){
        station * st = (station*) equip->equipement;
        if(memcmp(t->dest.mac, st->st_MAC.mac, 6)==0){
            printf("Station n°%d : ce trame est bien pour moi.\n", equip->id);
            return true;
        }
        else{
            printf("Station n°%d : ce trame n'est pas pour moi.\n", equip->id);
            return false;
        }
    }

    bool existe_asso_src = false;
    int port=-1;
    printf("Switch n°%d a recu la trame.\n", equip->id);
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
        sw->tab_association[sw->nb_asso] = (association) {t->src, port};
        sw->nb_asso++;
    }

    bool existe_asso_dest = false;
    int port_suivant=-1;
    //Apprentissage sur le port
    for (int i=0; i<sw->nb_asso; i++){
        if(memcmp(sw->tab_association[i].st_MAC.mac, t->dest.mac, 6)==0){
            existe_asso_dest = true;
            port_suivant = sw->tab_association[i].port;
            break;
        }
    }

    if(existe_asso_dest){  
        //le switch connait la dest connu
        printf("Le switch n°%d connait la destination.\n", equip->id);
        int id_suivant = sw->connectes[port_suivant];
        machine *equip_suivant = &r->machines[id_suivant];
        int port = -1;
        if(equip_suivant->tp_equip == TYPE_SWITCH){
            swtch *sw_suivant = (swtch*) equip_suivant->equipement;
            for (int i=0; i<sw_suivant->port_utilises; i++){
                if(sw_suivant->connectes[i] == equip->id){
                    port = i;
                    break;
                }
            }
        }

        if (port!=-1){
            swtch *sw_suivant = (swtch*) equip_suivant->equipement;
            if (sw_suivant->port_etat[port]==BLOQUE){
                return false;
            }
        }
        
        if(id_suivant == id_precedent){
            return false;
        }

        return send_trame(r, t, equip_suivant, port, equip->id);
    }
    else{
        printf("BROADCAST : adresse MAC inconnu.\n");
        for(int i=0; i < sw->nb_port; i++){ 
            
            int id_suivant = sw->connectes[i];
            if (id_suivant==id_precedent){
                continue;
            }
            machine *equip_suivant = &r->machines[id_suivant];
            int port = -1;
            if(equip_suivant->tp_equip == TYPE_SWITCH){
                swtch *sw_suivant = (swtch*) equip_suivant->equipement;
                for (int i=0; i<sw_suivant->port_utilises; i++){
                    if(sw_suivant->connectes[i] == equip->id){  //verif si le switch est bloqué
                        port = i;
                        break;
                    }
                }
            }
            //trouvé en face le num de port
            if (port!=-1){
                swtch *sw_suivant = (swtch*) equip_suivant->equipement;
                if (sw_suivant->port_etat[port]==BLOQUE && sw->port_etat[i]==BLOQUE){
                    continue;
                }
            }

            bool essaie = send_trame(r, t, equip_suivant, port, equip->id);
            if(essaie){
                return true;
            }   
        
        }


        return false;
    }
}
