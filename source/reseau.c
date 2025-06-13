#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "machines.h"
#include "reseau.h"

#define MAX_BUFFER_SIZE 255

void init_reseau(reseau *r)
{
    r->nb_machines = 0;
    r->nb_aretes = 0;
    r->machines = NULL;
    r->aretes = NULL;
    r->aretes_capacite = 0;
}

void deinit_reseau(reseau *r) {
    if (r == NULL) return;
    
    // libération des machines
    for (size_t i = 0; i < r->nb_machines; i++) {
        machine *m = &r->machines[i];
        if (m->tp_equip == TYPE_SWITCH && m->equipement != NULL) {
            swtch *sw = (swtch*) m->equipement;
            deinit_switch(sw);
            free(sw);
            m->equipement = NULL;
        }
        else if (m->tp_equip == TYPE_STATION && m->equipement != NULL){
            station *st = (station*) m->equipement;
            deinit_station(st);
            free(st);
            m->equipement = NULL;
        }
    }
    r->nb_machines=0;
    if (r->machines != NULL) {
        free(r->machines);
        r->machines = NULL;
    }
    if (r->aretes != NULL) {
        free(r->aretes);
        r->aretes = NULL;
    }
    r->nb_machines = 0;
    r->nb_aretes = 0;
    r->aretes_capacite=0;
}

//Lecture du fichier de configuration renvoie un structure reseau, si celui ci n'est pas possible alors renvoie null
int charger_reseau(const char* nom_fichier, reseau* g) {
    FILE* fichier = fopen(nom_fichier, "r");

    //Si le fichier n'existe pas ou échec d'ouverture 
    if (fichier == NULL) {
        perror("Erreur d'ouverture du fichier");
        return ERROR;
    }

    int nb_machines;
    int nb_liens;
    //lire des données de la première ligne 
    if (fscanf(fichier, "%u %d\n", &nb_machines, &nb_liens)!=2){
        //si ne lis pas 2 entiers pour la première ligne
        perror("Erreur dans la lecture du fichier: mauvais format");
        fclose(fichier);
        return ERROR;
    }

    //Initialisation de ma structure reseau
    g->nb_machines = nb_machines;
    g->nb_aretes = nb_liens;
    g->aretes = malloc(nb_liens*sizeof(arete));
    if (!g->aretes) {
        perror("Erreur allocation aretes");
        return ERROR;
    }
    g->aretes_capacite = nb_liens;
    g->machines = malloc(nb_machines * sizeof(machine));
    if (!g->machines) {
        perror("Erreur allocation machines");
        free(g->aretes);
        return ERROR;
    }   
    
    //configuration des machines a partir du fichier 
    for (int i = 0; i < nb_machines; i++) {
        //récupérer la ligne dans une variable ligne
        char ligne[MAX_BUFFER_SIZE];
        if (fgets(ligne, MAX_BUFFER_SIZE, fichier) == NULL) 
        {
            perror("Erreur de lecture de la machine");
            fclose(fichier);
            return ERROR;
        }

        int type;
        char info1[32];
        char info2[32];
        char info3[32];
        int nb = sscanf(ligne, "%d;%31[^;];%31[^;];%31[^\n]", &type, info1, info2, info3);
        //%31 -> Limite à 31 caractères maximum
        // [^;]	lit tout sauf ;

        if (nb != 4 && nb != 3) {
            printf("Ligne mal formée pour machine %d : %s\n", i, ligne);
            fclose(fichier);
            return ERROR;
        }

        machine* m = &g->machines[i];
        m->id = i;

        // Si 1 alors station, 2 si switch
        m->tp_equip = (type == 1) ? TYPE_STATION : TYPE_SWITCH;
        
        if (m->tp_equip == TYPE_STATION) {  
            station* st = malloc(sizeof(station));
            init_station(st);
            //Adresse mac
            str_to_mac(&st->st_MAC, info1);

            //Adresse IP
            str_to_ip(&st->st_IP, info2);

            //rajouter la station dans equipement
            m->equipement = st;

        } 
        else if (m->tp_equip == TYPE_SWITCH) {
            int nombre = atoi(info2);   //convertir une chaine de caractère en entier
            if (nombre <= 0) 
            {
                fprintf(stderr, "Switch avec un nombre de ports invalide : %s\n", info2);
                return ERROR;
            }

            swtch* sw = malloc(sizeof(swtch));
            init_switch(sw, nb_machines, nombre);
            //Adresse Mac
            str_to_mac(&sw->sw_MAC, info1);
            sw->nb_port = nombre;
            sw->priorite = atoi(info3);
            sw->port_utilises = 0;

            m->equipement = sw;

            sw->port_etat = malloc(sizeof(etatPort)*sw->nb_port);
            for (int p = 0; p<sw->nb_port; p++){
                sw->port_etat[p] = DESIGNE;
            }
            sw->bridge_protocol.bridge_id = concat_bridge_id(sw);
            sw->bridge_protocol.root_id = sw->bridge_protocol.bridge_id;
            sw->bridge_protocol.cost = 0;
        }
        else {
            printf("Type inconnu pour machine %d : %d\n", i, type);
            fclose(fichier);
            return ERROR;
        }
        
    }

    //création des associations (src;dst;poids)
    for (int i = 0; i < nb_liens; i++) {
        int src;
        int dst; 
        int poids;

        if (fscanf(fichier, "%d;%d;%d\n", &src, &dst, &poids) != 3) {
            perror("Erreur de lecture des arêtes");
            return ERROR;
        }

        machine *m1 = &g->machines[src];
        machine *m2 = &g->machines[dst];

        g->aretes[i].m1 = m1;
        g->aretes[i].m2 = m2;
        g->aretes[i].poids = poids;

        // si un switch et un station
        if (m1->tp_equip == TYPE_SWITCH && m2->tp_equip == TYPE_STATION) {
            swtch *sw = (swtch *)m1->equipement;
            sw->port_utilises++;
            
        } 
        else if (m2->tp_equip == TYPE_SWITCH && m1->tp_equip == TYPE_STATION) {
            swtch *sw = (swtch *)m2->equipement;
            sw->port_utilises++; 
        }
    }

    //tab connecté
    for (int j=0; j<g->nb_machines; j++){
        if (g->machines[j].tp_equip == TYPE_SWITCH){
            machine* m = (machine*) &g->machines[j];
            swtch* sw = (swtch*) m->equipement;
            printf(">>>>>>switch %d\n", m->id);
            for (int i = 0; i<g->nb_aretes; i++){
                arete *art = &g->aretes[i];
                if(art->m1->id == m->id){
                    for (int p=0; p<sw->nb_port; p++){
                        if(sw->connectes[p]==-1){
                            sw->connectes[p] = art->m2->id;
                            printf("port %d -> %d\n", p, art->m2->id);
                            break;
                        }
                    }
                }
                else if(art->m2->id == m->id){
                    for (int p=0; p<sw->nb_port; p++){
                        if(sw->connectes[p]==-1){
                            sw->connectes[p] = art->m1->id;
                            printf("port %d -> %d\n", p, art->m1->id);
                            break;
                            
                        }
                    }
                }
            }
        }
    }
    
    fclose(fichier);
    return SUCCESS;
}

void affichage_reseau(reseau *g){
    size_t nb = g->nb_machines;
    printf("----------------RESEAU----------------\n");
    printf("Nombre de machines : \t%zu\n\n", nb);
    for (size_t i=0; i<nb; i++){
        machine m = g->machines[i];
        printf("-------------Machine n°%ld-------------\n",i);
        printf("Id de la machine : %d\n", m.id);
        if (m.tp_equip == TYPE_STATION)
        {
            printf("Type de la machine : STATION\n");
            station* equip = (station*)  m.equipement;
            char str[18];
            mac_to_str(equip->st_MAC, str);
            printf("L'adresse MAC : %s\n", str);
            char str1[18];
            ip_to_str(equip->st_IP, str1);
            printf("L'adresse IP : %s\n", str1);
        }
        else if (m.tp_equip == TYPE_SWITCH)
        {
            swtch* equip = (swtch*)  m.equipement;
            printf("Type de la machine : SWITCH\n");
            char str[18];
            mac_to_str(equip->sw_MAC, str);
            printf("L'adresse MAC : %s\n", str);
            printf("Nombre de ports : %d\n", equip->nb_port);
            printf("La priorité : %u\n", equip->priorite);
            affichage_tab_commutation(equip);
        }
        printf("\n");
    }
    
    printf("---------------LIAISONS--------------\n");
    for (size_t p=0; p<g->nb_aretes; p++){
        arete *art = &g->aretes[p];
        printf("%d --> %d : %ld\n", art->m1->id, art->m2->id, art->poids);
    }
}

void affichage_tab_commutation(swtch *sw){
    if(sw->port_utilises!=0){
        printf("---------TABLEAU DE COMMUTATION---------\n");
        for (int j=0; j<sw->port_utilises; j++){
            char str1[18];
            association asso = sw->tab_association[j];
            mac_to_str(asso.st_MAC, str1);
            printf("Addresse (%s) --> port %d\n", str1, asso.port);
        }
    }
    else{
        printf("Table vide\n");
    }
}

void afficher(swtch* sw){
    for (int i=0; i<sw->nb_port; i++){
        printf("port %d -> %d\n", i, sw->connectes[i]);
        
    }
    printf("%d\n", sw->nb_port);
}


int nb_voisin(machine *m, reseau *r) {
    if (m==NULL || r==NULL) return -1;

    int count = 0;
    for (size_t i = 0; i < r->nb_aretes; i++) {

        if (r->aretes[i].m1->id == m->id || r->aretes[i].m2->id == m->id){
            count++;
        }
    }
    return count;
}

void tab_voisin(machine *m, machine *voisins[], reseau *r) {
    if (m == NULL || r==NULL) return;

    size_t index = 0;
    for (size_t i = 0; i < r->nb_aretes; i++) {
        arete *a = &r->aretes[i];

        if (a->m1->id == m->id) {
            voisins[index] = r->aretes[i].m2;
            index++;
        } 
        else if (a->m2->id == m->id) {
            voisins[index] = r->aretes[i].m1;
            index++;
        }
    }
}