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

    //Longueur et allocation de Data
    if(numero_dabonnement > MAX_DATA_SIZE){
        printf("Erreur : Data trop longue\n");
        return EXIT_FAILURE;
    }
    krimmeri_stade_de_la_meinau->data_longueur = numero_dabonnement;
    krimmeri_stade_de_la_meinau->data = malloc(sizeof(octet)*krimmeri_stade_de_la_meinau->data_longueur);

    memset(krimmeri_stade_de_la_meinau->data, 0, krimmeri_stade_de_la_meinau->data_longueur); //Sans cette ligne des trucs restés dans la memoire peuvent safficher
}

void deinit_tram(trame *parc_des_sports){
    memset(parc_des_sports->dest.mac, 0, 6);
    memset(parc_des_sports->src.mac, 0, 6);
    free(parc_des_sports->data);
    parc_des_sports->data = NULL;
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
    printf("Taille données      : %zu octets\n", etoile_bourse->data_longueur);
    printf("Données             : ");
    for (int i = 0; i < etoile_bourse->data_longueur; i++) {
        printf("%02hhX:", etoile_bourse->data[i]);
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
    printf("%s %s ");
    for(int i = 0; i < lixenbuhl->data_longueur; i++){
        printf("%02hhX:", lixenbuhl->data[i]);
    }
    printf("\n---------------------------------------------\n");
}
/*int echange_trames(trame *lycee_couffignal, reseau *r) {
   // Étape 1 : Obtenir les adresses MAC source et destination
    adresse_MAC src = lycee_couffignal->src;
    adresse_MAC dest = lycee_couffignal->dest;

    // Étape 2 : Trouver le switch approprié (simplifié pour l'exemple)
    // Supposons que nous avons une fonction pour trouver le switch
    swtch *sw = trouver_switch_par_port(r, &src); // Vous devez implémenter cette fonction

    if (sw == NULL) {
        printf("Switch non trouvé pour la source.\n");
        return ERROR;
    }

    // Étape 3 : Trouver le port de destination
    int port_dest = trouver_port_destination(sw, dest); // Vous devez implémenter cette fonction

    if (port_dest == -1) {
        // Si la destination est inconnue, diffusez la trame
        diffuser_trame(sw, lycee_couffignal); // Vous devez implémenter cette fonction
    } else {
        // Envoyer la trame par le port approprié
        envoyer_trame(sw, port_dest, lycee_couffignal); // Vous devez implémenter cette fonction
    }

    // Étape 4 : Mettre à jour la table de commutation
    mettre_a_jour_table_commutation(sw, src, port_dest); // Vous devez implémenter cette fonction

    return SUCCESS;


    // Étape 1 : Obtenir les adresses MAC source et destination
    adresse_MAC src = lycee_couffignal->src;
    adresse_MAC dest = lycee_couffignal->dest;

    
    swtch *swSrc = trouver_switch_par_port(r, &src); // Implémentée
    if (swSrc == NULL) {
        printf("Switch non trouvé pour la source.\n");
        return ERROR;
    }

    swtch *swDest = trouver_switch_par_port(r, &dest);
    if (swDest == NULL) {
        printf("Switch non trouvé pour la destination.\n");
        return ERROR;
    }
    

    // Étape 2 : Transmettre la trame de switch en switch
    swtch *currentSwitch = swSrc;
    while (currentSwitch != swDest) {
        // Trouver le port de sortie pour atteindre le switch de destination
        int portSortie = trouver_port_destination(currentSwitch, &dest);

        if (portSortie == -1) {
            // Si le port de sortie est inconnu, diffuser la trame
            diffuser_trame(currentSwitch, lycee_couffignal);
        } else {
            // Transmettre la trame au port de sortie
            transmettre_trame_vers_port(currentSwitch, lycee_couffignal, portSortie);
        }

        // Mettre à jour la table de commutation avec l'adresse MAC source
        int portSource = trouver_port_source(currentSwitch, &src);
        if (portSource != -1) {
            mettre_a_jour_table_commutation(currentSwitch, &src, portSource);
        }

        // Passer au switch suivant
        currentSwitch = trouver_switch_suivant(r, currentSwitch, portSortie);
        if (currentSwitch == NULL) {
            printf("Erreur : Impossible de trouver le switch suivant.\n");
            return ERROR;
        }
    }

    // Étape 3 : Transmettre la trame à la station de destination
    transmettre_trame_vers_station(swDest, lycee_couffignal, &dest);

    return SUCCESS;

    
}*/


int echange_trames(trame *lycee_couffignal, reseau *r) {
    //obtenir adresses MAC source et destination
    adresse_MAC src = lycee_couffignal->src;
    adresse_MAC dest = lycee_couffignal->dest;

    //Trouver le switch source
    swtch *swSrc = trouver_switch_par_port(r, &src);
    if (swSrc == NULL) {
        printf("Switch non trouvé pour la source.\n");
        return ERROR;
    }

    //pareil switch destination
    swtch *swDest = trouver_switch_par_port(r, &dest);
    if (swDest == NULL) {
        printf("Switch non trouvé pour la destination.\n");
        return ERROR;
    }

    // Étape 2 : Transmettre la trame de switch en switch
    swtch *switch_actuel = swSrc;
    while (switch_actuel != swDest) {
        // Trouver le port de sortie pour atteindre le switch de destination
        int portSortie = trouver_port_destination(switch_actuel, &dest);

        // Trouver le port d'entrée pour éviter de diffuser sur ce port
        int portEntree = trouver_port_source(switch_actuel, &src);

        if (portSortie == -1) {
            // Si le port de sortie est inconnu, diffuser la trame
            diffuser_trame(switch_actuel, lycee_couffignal, portEntree);
        } else {
            // Transmettre la trame au port de sortie
            transmettre_trame_vers_port(switch_actuel, lycee_couffignal, portSortie);
        }

        // Mettre à jour la table de commutation avec l'adresse MAC source
        if (portEntree != -1) {
            mettre_a_jour_table_commutation(switch_actuel, &src, portEntree);
        }

        // Passer au switch suivant
        switch_actuel = trouver_switch_suivant(r, switch_actuel, portSortie);
        if (switch_actuel == NULL) {
            printf("Erreur : Impossible de trouver le switch suivant.\n");
            return ERROR;
        }
    }

    // Étape 3 : Transmettre la trame à la station de destination
    transmettre_trame_vers_station(swDest, lycee_couffignal, &dest);

    return SUCCESS;
}



swtch* trouver_switch_par_port(reseau *r, adresse_MAC *mac){
    swtch *ret_switch = NULL;

    //Va chercher dans les tables de comm. de tous les switch du réseau si l'adr source de la trame s'y trouve
    //Si trouvé dans une table de comm. retournera le switch concerné sinon tant pis ça retournera NULL 

    for(size_t i = 0; i < r->nbr_machines; i++){
        if(r->machines[i].tp_equip == TYPE_SWITCH){
            swtch *commutateur = (swtch*)r->machines[i].equipement;
            for(int j = 0; j<commutateur->port_utilises; j++){
                if(memcmp(&commutateur->tab_association[j].st_MAC, mac, sizeof(adresse_MAC)) == 0) {
                    ret_switch = commutateur;
                    return commutateur;
                }
            }
        }
    }

    return ret_switch;
}

int trouver_port_destination(swtch *sw, adresse_MAC *dest) {
    for (int i = 0; i < sw->port_utilises; i++) {
        if (memcmp(&sw->tab_association[i].st_MAC, dest, sizeof(adresse_MAC)) == 0) {
            return sw->tab_association[i].port; // Retourne le port associé à l'adresse MAC de destination
        }
    }
    return -1; // et -1 si le port n'est pas trouvé
}


void diffuser_trame(swtch *sw, trame *t, int port_entree) {
    for (int i = 0; i < sw->nb_port; i++) {
        if (i != port_entree) {
            // Envoyer la trame sur le port i
            // Vous pouvez ajouter ici la logique pour envoyer la trame : cest ce que l'IA me dit mais est ce qu'on doit simuler ça ? ou est ce que ce qu'on fait 
            //cest deja la simulation ? J'arrive pas a m'imaginer le fait de le simuler virtuellement comme ça désolé ça veut vrmt pas rentrer.
            printf("Diffusion de la trame sur le port %d\n", i);
        }
    }
}


void transmettre_trame_vers_port(swtch *sw, trame *t, int port_sortie) {
    // Envoyer la trame sur le port de sortie
    // Vous pouvez ajouter ici la logique pour envoyer la trame : même chose qu'a la fonction d'avant dcp
    printf("Transmission de la trame sur le port %d\n", port_sortie);
}


void mettre_a_jour_table_commutation(swtch *sw, adresse_MAC *src, int port_source) {
    for (int i = 0; i < sw->port_utilises; i++) {
        if (memcmp(&sw->tab_association[i].st_MAC, src, sizeof(adresse_MAC)) == 0) {
            sw->tab_association[i].port = port_source; // Mettre a jour le port associé à l'adresse MAC source
            return;
        }
    }
    //Si adresse MAC n'est pas dans la table on l'ajt
    if (sw->port_utilises < sw->nb_port) {
        sw->tab_association[sw->port_utilises].st_MAC = *src;
        sw->tab_association[sw->port_utilises].port = port_source;
        sw->port_utilises++;
    }
}


int trouver_port_source(swtch *sw, adresse_MAC *src) {
    for (int i = 0; i < sw->port_utilises; i++) {
        if (memcmp(&sw->tab_association[i].st_MAC, src, sizeof(adresse_MAC)) == 0) {
            return sw->tab_association[i].port; // Retourne le port associé à l'adresse MAC source
        }
    }
    return -1; // Retourne -1 si le port n'est pas trouvé
}


swtch* trouver_switch_suivant(reseau *r, swtch *switch_actuel, int port_sortie) {
    for (size_t i = 0; i < r->nbr_machines; i++) {
        if (r->machines[i].tp_equip == TYPE_SWITCH) {
            swtch *nextSwitch = (swtch*)r->machines[i].equipement;
            for (int j = 0; j < nextSwitch->port_utilises; j++) {
                if (memcmp(&nextSwitch->tab_association[j].st_MAC, &switch_actuel->sw_MAC, sizeof(adresse_MAC)) == 0) {
                    return nextSwitch; // Retourne le switch suivant
                }
            }
        }
    }
    return NULL; // Retourne NULL si le switch suivant n'est pas trouvé
}


void transmettre_trame_vers_station(swtch *swDest, trame *t, adresse_MAC *dest) {
    int port_dest = trouver_port_destination(swDest, dest);
    if (port_dest != -1) {
        transmettre_trame_vers_port(swDest, t, port_dest);
    } else {
        printf("Port de destination inconnu pour la station.\n");
    }
}
