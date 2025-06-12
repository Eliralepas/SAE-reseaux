// Dans protocole_STP.c
#include "protocole_STP.h"

void protocole_STP_chemin(reseau *r) {
    if (r==NULL) return;

    //recherche du root id
    uint64_t min_bridge_id = UINT64_MAX;
    machine *root_switch = NULL;
    
    for (size_t i = 0; i < r->nbr_machines; i++) {
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


    // Mise a jour des root id
    for (size_t i = 0; i < r->nbr_machines; i++) {
        machine *m = &r->machines[i];
        if (m->tp_equip == TYPE_SWITCH) {
            swtch *sw = (swtch*)m->equipement;
            sw->bridge_protocol.root_id = min_bridge_id;
            sw->bridge_protocol.cost = (m == root_switch) ? 0 : UINT16_MAX;
            
            for (int p = 0; p < sw->nb_port; p++) {
                sw->port_etat[p] = BLOQUE;
            }
        }
    }

    //calcul des chemins 
    for (int p = 0; p < r->nbr_machines; p++) {
        for (size_t i = 0; i < r->nbr_machines; i++) {
            machine *m = &r->machines[i];
            if (m->tp_equip == TYPE_SWITCH && m != root_switch) {
                swtch *sw = (swtch*)m->equipement;
                
                int nb_voisins = nb_voisin(m, r);
                machine voisins[nb_voisins];
                tab_voisin(m, voisins, r);
                
                for (int j = 0; j < nb_voisins; j++) {
                    machine *v = &voisins[j];
                    if (v->tp_equip == TYPE_SWITCH) {
                        swtch *sw_voisin = (swtch*)v->equipement;
                        
                        for (size_t k = 0; k < r->nb_aretes; k++) {
                            arete a = r->aretes[k];
                            if ((a.m1.id == m->id && a.m2.id == v->id) || 
                                (a.m2.id == m->id && a.m1.id == v->id)) {
                                
                                uint16_t nouveau_cout = sw_voisin->bridge_protocol.cost + a.poids;
                                if (nouveau_cout < sw->bridge_protocol.cost) {
                                    sw->bridge_protocol.cost = nouveau_cout;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //port désigné et racine
    for (size_t i = 0; i < r->nbr_machines; i++) {
        machine *m = &r->machines[i];
        if (m->tp_equip == TYPE_SWITCH && m != root_switch) {
            swtch *sw = (swtch*)m->equipement;
            int port_racine = -1;
            uint16_t min_cout = UINT16_MAX;
            
            int nb_voisins = nb_voisin(m, r);
            machine voisins[nb_voisins];
            tab_voisin(m, voisins, r);
            
            for (int j = 0; j < nb_voisins; j++) {
                machine *v = &voisins[j];
                if (v->tp_equip == TYPE_SWITCH) {
                    swtch *sw_voisin = (swtch*)v->equipement;
                    
                    for (size_t k = 0; k < r->nb_aretes; k++) {
                        arete a = r->aretes[k];
                        if ((a.m1.id == m->id && a.m2.id == v->id) || 
                            (a.m2.id == m->id && a.m1.id == v->id)) {
                            
                            if (sw_voisin->bridge_protocol.cost + a.poids == sw->bridge_protocol.cost) {
                                if (sw_voisin->bridge_protocol.cost < min_cout) {
                                    min_cout = sw_voisin->bridge_protocol.cost;
                                    port_racine = j;
                                }
                            }
                        }
                    }
                }
            }
            
            for (int p = 0; p < sw->nb_port; p++) {
                if (p == port_racine) {
                    sw->port_etat[p] = RACINE;
                } else {
                    sw->port_etat[p] = DESIGNE;
                }
            }
        }
    }

    //blocage des ports
    for (size_t i = 0; i < r->nbr_machines; i++) {
        machine *m = &r->machines[i];
        if (m->tp_equip == TYPE_SWITCH) {
            swtch *sw = (swtch*)m->equipement;
            
            for (int p1 = 0; p1 < sw->nb_port; p1++) {
                if (sw->port_etat[p1] == DESIGNE) {
                    for (int p2 = p1+1; p2 < sw->nb_port; p2++) {
                        if (sw->port_etat[p2] == DESIGNE) {
                            sw->port_etat[p2] = BLOQUE;
                        }
                    }
                }
            }
        }
    }
}

uint8_t calcul_cout(reseau *r, machine *m, int* visite) {
    if (!r || !m || !m->equipement) return 255;
    
    swtch* sw = (swtch*)m->equipement;
    if (sw->bridge_protocol.bridge_id == sw->bridge_protocol.root_id) 
        return 0;
    
    if (visite[m->id] == 1) 
        return 255;
    
    visite[m->id] = 1;
    uint8_t min_cost = 255;
    
    int nb = nb_voisin(m, r);
    machine voisins[nb];
    tab_voisin(m, voisins, r);
    
    for (int i = 0; i < nb; i++) {
        machine *v = &voisins[i];
        if (v->tp_equip == TYPE_SWITCH) {
            uint8_t cout_voisin = calcul_cout(r, v, visite);
            
            for (size_t j = 0; j < r->nb_aretes; j++) {
                arete a = r->aretes[j];
                if ((a.m1.id == m->id && a.m2.id == v->id) || 
                    (a.m2.id == m->id && a.m1.id == v->id)) {
                    
                    uint8_t total = a.poids + cout_voisin;
                    if (total < min_cost) {
                        min_cost = total;
                    }
                }
            }
        }
    }
    
    visite[m->id] = 0;
    return min_cost;
}