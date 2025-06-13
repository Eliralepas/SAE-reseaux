// Dans protocole_STP.c
#include "protocole_STP.h"

void protocole_STP_chemin(reseau *r) {
    if (r==NULL) return;

    //recherche du root id
    uint64_t min_bridge_id = UINT64_MAX;
    machine *root_switch = NULL;
    
    for (size_t i = 0; i < r->nb_machines; i++) {
        machine *m = &r->machines[i];
        if (m->tp_equip == TYPE_SWITCH) {
            swtch *sw = (swtch*)m->equipement;
            sw->bridge_protocol.bridge_id = concat_bridge_id(sw);
            
            if (sw->bridge_protocol.bridge_id < min_bridge_id) {
                min_bridge_id = sw->bridge_protocol.bridge_id;
                root_switch = m;
            }
        }
    }

    if (root_switch == NULL) {
        printf("Il n'y a pas de switch racine");
        return;
    }

    printf("La racine est la machine n°%d\n", root_switch->id);


    // Mise a jour des root id
    for (size_t i = 0; i < r->nb_machines; i++) {
        machine *m = &r->machines[i];
        if (m->tp_equip == TYPE_SWITCH) {
            swtch *sw = (swtch*)m->equipement;
            sw->bridge_protocol.root_id = min_bridge_id;
            sw->bridge_protocol.cost = (m == root_switch) ? 0 : UINT16_MAX;
            
            for (int p = 0; p < sw->nb_port; p++) {
                sw->port_etat[p] = DESIGNE;
            }
        }
    }

    size_t *distance_sommet = malloc(sizeof(size_t)*r->nb_machines);
    bool *fixes = malloc(sizeof(bool)*r->nb_machines);

    for(size_t i=0; i<r->nb_machines; i++){
        fixes[i] = false;
    }
    fixes[root_switch->id] = true;

    for(size_t i=0; i<r->nb_machines; i++){
        if(i == root_switch->id){
        distance_sommet[i] = 0;
        }
        else{                                                                                                                                                                               
        distance_sommet[i] = SIZE_MAX;
        }
    }

    int* sommets_precedents = malloc(sizeof(int) * r->nb_machines);

    bool continuer = true;
    int id_sommet_fixe = root_switch->id;

    sommets_precedents[root_switch->id] = root_switch->id;
    while(continuer){
        size_t poids_sommet_fixe = distance_sommet[id_sommet_fixe];

        int nb = nb_voisin(&r->machines[id_sommet_fixe], r);
        machine* tab[nb];
        tab_voisin(&r->machines[id_sommet_fixe], tab, r);

        for(int i=0; i<nb; i++){
            arete *art_choisi=NULL;
            for (int j=0; j<r->nb_aretes; j++){
                arete *art = &r->aretes[j];
                if(art->m1->id == r->machines[id_sommet_fixe].id && art->m2->id == tab[i]->id){
                    art_choisi = art;
                    break;
                }
                else if(art->m2->id == r->machines[id_sommet_fixe].id && art->m1->id == tab[i]->id){
                    art_choisi = art;
                    break;
                }
            }
            
            size_t poids = art_choisi->poids;
            if(poids_sommet_fixe + poids < distance_sommet[tab[i]->id]){
                distance_sommet[tab[i]->id] = poids_sommet_fixe + poids;
                sommets_precedents[tab[i]->id] = id_sommet_fixe;
            }
        }

        //Verifie si tous les sommets ont été fixés
        size_t nb_fixes = 0;
        for(size_t i=0; i<r->nb_machines; i++){
            if(fixes[i]){
                nb_fixes++;
            }
        }

        if(nb_fixes == r->nb_machines){
            continuer = false;
        }

        //Fixe le sommet
        int min = 0;
        size_t poids_min = SIZE_MAX;
        for(size_t i=0; i<r->nb_machines; i++){
            if(!fixes[i] && distance_sommet[i] <= poids_min){
                min = i;
                poids_min = distance_sommet[i];
            }
        }
        
        id_sommet_fixe = min;
        fixes[id_sommet_fixe] = true;
    }

    for (int i=0; i<r->nb_machines; i++){
        machine *m = (machine*) &r->machines[i];
        if(m->tp_equip==TYPE_SWITCH){
            //regarder dans le tab distance sommet
            swtch *sw = (swtch*) m->equipement;
            sw->bridge_protocol.cost = distance_sommet[m->id];

            int port= -1;
            for (int j = 0; j<sw->nb_port; j++){
                if(sommets_precedents[m->id] == sw->connectes[j]){
                    port = j;
                    break;
                }
            }
            if(port!=-1){
                sw->port_etat[port] = RACINE;
            }
        }        
    }   

    for (int i=0; i<r->nb_machines; i++){
        machine *m = (machine*) &r->machines[i];
        if(m->tp_equip==TYPE_SWITCH){
            swtch *sw = (swtch*) m->equipement;

            for(int j=0; j<sw->nb_port; j++){
                if(sw->port_etat[j]== RACINE){
                    continue;
                }

                int port_en_face = sw->connectes[j];
                machine *recup_machine = (machine*) &r->machines[port_en_face];

                if (recup_machine->tp_equip == TYPE_SWITCH){
                    swtch *sw_face = (swtch*) m->equipement;

                    int port = -1;
                    bool est_designe = false;
                    for (int p = 0; p<sw_face->nb_port; p++){
                        if(sw_face->connectes[p]==m->id){
                            port = p;
                            if(sw_face->port_etat[p] == DESIGNE){
                                est_designe = true;
                            }
                            break;
                        }
                    }
                    if(est_designe){
                        sw->port_etat[j] = BLOQUE;
                    }
                }
            }
        }        
    }


    //A la fin avec les autres free
    free(sommets_precedents);
    free(fixes);
    free(distance_sommet);
}

void affichage_port_etat(reseau *r){
    for (int i=0; i<r->nb_machines; i++){
        machine *m = &r->machines[i];

        if(m->tp_equip == TYPE_SWITCH){
            swtch *sw = (swtch*) m->equipement;
            printf("Switch ID %d :\n", m->id);
            for (int p = 0; p < sw->nb_port; p++) {
                if(sw->connectes[p]!= -1){
                    printf("  Port %d -> ", p);
                    switch (sw->port_etat[p]) {
                        case RACINE:
                            printf("RACINE\n");
                            break;
                        case DESIGNE:
                            printf("DESIGNE\n");
                            break;
                        case BLOQUE:
                            printf("BLOQUE\n");
                            break;
                        default:
                            printf("ERREUR\n");
                            break;
                    }
                }
            }
        }        
    }
}