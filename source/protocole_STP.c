#include "protocole_STP.h"

void protocole_STP_chemin(reseau *r){
    if(r==NULL) return;

    // Recherche Root ID
    uint64_t rootID = UINT64_MAX;
    machine* root_machine = NULL;

    for (size_t i = 0; i < r->nbr_machines; i++) {
        machine* m = &r->machines[i];
        if (m->tp_equip == TYPE_SWITCH) {
            swtch* sw = (swtch*) m->equipement;
            uint64_t id = sw->bridge_protocol.bridge_id;
            if (id < rootID) {
                rootID = id;
                root_machine = m;
            }
        }
    }

    printf("Le root racine = %02lX (machine %d)\n", rootID, root_machine->id);

    //rajouter au bpdu le rootID trouvé pour chaque switch
    for (size_t i=0; i<r->nbr_machines; i++){
        machine* m = &r->machines[i];
        if (m->tp_equip == TYPE_SWITCH){
            swtch* sw = (swtch*) m->equipement;
            sw->bridge_protocol.root_id=rootID;
        }
    }

    //initialiser tous les ports a désigné
    for (size_t i = 0; i < r->nbr_machines; i++) {
        machine* m = &r->machines[i];
        if (m->tp_equip == TYPE_SWITCH) {
            swtch* sw = (swtch*) m->equipement;
            for (int p = 0; p < sw->nb_port; p++) {
                sw->port_etat[p] = DESIGNE;
            }
        }
    }

    //changer les états de port (racine est désigné)
    for (size_t i = 0; i < r->nbr_machines; i++) {
        machine* m = &r->machines[i];
        //si c'est un switch autre que le switch racine
        if (m->tp_equip == TYPE_SWITCH && m != root_machine) {
            swtch* sw = (swtch*) m->equipement;

            // Chercher voisin avec plus petit coût vers racine
            size_t nb = nb_voisin(m, r);
            machine voisins[nb];
            tab_voisin(m, voisins, r);

            int meilleur_port = -1;
            size_t meilleur_cout = 255;

            for (size_t j = 0; j < nb; j++) {
                machine *v = &voisins[j];
                if (v->tp_equip == TYPE_SWITCH){
                    
                    int visite[r->nbr_machines];
                    for (int k=0; k<r->nbr_machines; k++){
                        visite[k]=0;
                    }

                    uint8_t cout = calcul_cout(r, v, visite);
                    for (int p = 0; p < sw->port_utilises; p++) {
                        if (memcmp(sw->tab_association[p].st_MAC.mac, ((swtch*)v->equipement)->sw_MAC.mac, 6) == 0) {
                            if (cout < meilleur_cout) {
                                meilleur_cout = cout;
                                meilleur_port = p;
                            }
                        }
                    }
                }   

                if (meilleur_port != -1) {
                    sw->port_etat[meilleur_port] = RACINE;
                }

                // Les autres ports qui sont actifs → DESIGNÉS
                for (int p = 0; p < sw->port_utilises; p++) {
                    if (sw->port_etat[p] != RACINE) {
                        sw->port_etat[p] = DESIGNE;
                    }
                }
            }
        }
    }

    //bloqué des ports
    for (size_t i = 0; i < r->nbr_machines; i++) {
        machine* m = &r->machines[i];

        if (m->tp_equip == TYPE_SWITCH || m != root_machine){
            swtch* sw = (swtch*) m->equipement;

            for (int p = 0; p < sw->port_utilises; p++) {
                if (sw->port_etat[p] != RACINE){

                    adresse_MAC mac_voisin = sw->tab_association[p].st_MAC;

                    for (size_t j = 0; j < r->nbr_machines; j++) {
                        machine* voisin = &r->machines[j];
                        if (voisin->tp_equip == TYPE_SWITCH){

                            swtch* sw_voisin = (swtch*) voisin->equipement;
                            if (memcmp(sw_voisin->sw_MAC.mac, mac_voisin.mac, 6) == 0){

                                for (int q = 0; q < sw_voisin->port_utilises; q++) {
                                    if (memcmp(sw_voisin->tab_association[q].st_MAC.mac, sw->sw_MAC.mac, 6) == 0) {
                                        if (sw_voisin->port_etat[q] != RACINE) {
                                            sw->port_etat[p] = BLOQUE;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    printf("ARBRE CONFIGURÉ\n");
}

uint8_t calcul_cout(reseau *r, machine *m, int* visite) {
    if (r==NULL || m==NULL) return 255;

    //on part de machine au switch machine (on part sur les voisins de ses voisins jusqu'à ce qu'on le trouve)
    swtch* sw = (swtch*) m->equipement;
    uint64_t root_id = sw->bridge_protocol.root_id;

    if (sw->bridge_protocol.bridge_id == root_id) return 0;

    if (visite[m->id]=1) {
        return 255; // évite les cycles
    }
    visite[m->id] = 1;

    size_t nb = nb_voisin(m, r);
    machine voisins[nb];
    tab_voisin(m, voisins, r);

    uint8_t min_cost = 255;

    for (size_t i = 0; i < nb; i++) {
        machine *v = &voisins[i];
        if (v->tp_equip == TYPE_SWITCH) 
        {
            swtch* sw_v = (swtch*) v->equipement;
            
            //va rentrer dedans jusqu'à rencontre la racine qui elle renvoie 0
            uint8_t cout_voisin = calcul_cout(r, v, visite);

            for (size_t j = 0; j < r->nb_aretes; j++) {
                arete a = r->aretes[j];
                if ((a.m1.id == m->id && a.m2.id == v->id) || (a.m2.id == m->id && a.m1.id == v->id)) 
                {
                    uint8_t total = a.poids + cout_voisin;
                    if (total < min_cost) 
                    {
                        min_cost = total;
                    }
                }
            }
        }

        
    }

    return min_cost; // pas de chemin trouvé
}
