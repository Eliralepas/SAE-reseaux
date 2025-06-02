#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "machines.h"
#include "reseau.h"

#define MAX_BUFFER_SIZE 255

void init_reseau(reseau *r)
{
    r->nbr_machines = 0;
    r->nb_aretes = 0;
    r->machines = NULL;
    r->aretes = NULL;
    r->aretes_capacite = 0;
}

//Lecture du fichier de configuration renvoie un structure reseau, si celui ci n'est pas possible alors renvoie null
int charger_reseau(const char* nom_fichier, reseau* g) {
    FILE* fichier = fopen(nom_fichier, "r");

    //Si le fichier n'existe pas ou échec d'ouverture 
    if (fichier == NULL) {
        perror("Erreur d'ouverture du fichier");
        return -1;
    }

    int nb_machines;
    int nb_liens;
    //lire des données de la première ligne 
    if (fscanf(fichier, "%u %d\n", &nb_machines, &nb_liens)!=2){
        //si ne lis pas 2 entiers pour la première ligne
        perror("Erreur dans la lecture du fichier");
        fclose(fichier);
        return -1;
    }

    //Initialisation de ma structure reseau
    g->nbr_machines = nb_machines;
    g->nb_aretes = nb_liens;
    g->aretes = malloc(nb_liens*sizeof(arete));
    if (!g->aretes) {
        perror("Erreur allocation aretes");
        return -1;
    }
    g->aretes_capacite = nb_liens;
    g->machines = malloc(nb_machines * sizeof(machine));
    if (!g->machines) {
        perror("Erreur allocation machines");
        free(g->aretes);
        return -1;
    }   
    
    //config des machines a partir du fichier 
    for (int i = 0; i < nb_machines; i++) {
        //récupérer la ligne dans une variable ligne
        char ligne[MAX_BUFFER_SIZE];
        if (fgets(ligne, MAX_BUFFER_SIZE, fichier) == NULL) 
        {
            perror("Erreur de lecture de la machine");
            fclose(fichier);
            return ERROR;
        }

        int type;   //1 pour station et 2 pour switch
        char info1[32];
        char info2[32];
        char info3[32];
        int nb = sscanf(ligne, "%d;%31[^;];%31[^;];%31[^\n]", &type, info1, info2, info3);
        //%31 -> Limite à 31 caractères maximum
        // [^;]	lit tout sauf ;

        if (nb != 4 && nb != 3) {
            fprintf(stderr, "Ligne mal formée pour machine %d : %s\n", i, ligne);
            fclose(fichier);
            return ERROR;
        }

        machine* m = &g->machines[i];
        m->id = i;
        //si 1 alors station, 2 si switch
        m->tp_equip = (type == 1) ? TYPE_STATION : TYPE_SWITCH;
        
        if (m->tp_equip == TYPE_STATION) {  
            station* st = malloc(sizeof(station));
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

            swtch* sw = malloc(sizeof(swtch)); // tab_voisins
            //Adresse Mac
            str_to_mac(&sw->sw_MAC, info1);
            sw->nb_port = nombre;
            sw->priorite = atoi(info3);
            sw->tab_association = malloc(sizeof(association)*nombre);
            sw->port_utilises = 0;
            m->equipement = sw;
        }
        else {
            fprintf(stderr, "Type inconnu pour machine %d : %d\n", i, type);
            fclose(fichier);
            return ERROR;
        }
        
    }

    //création des associations (src;dst;poids)
    for (int i = 0; i < nb_liens; i++) {
        int src;
        int dst; 
        int poids;
        bool trv = false;

        if (fscanf(fichier, "%d;%d;%d\n", &src, &dst, &poids) != 3) {
            perror("Erreur de lecture des arêtes");
            return ERROR;
        }

        machine *m1 = &g->machines[src];
        machine *m2 = &g->machines[dst];

        g->aretes[i].m1 = *m1;
        g->aretes[i].m2 = *m2;
        g->aretes[i].poids = poids;

        // si un switch et un station
        if (m1->tp_equip == TYPE_SWITCH && m2->tp_equip == TYPE_STATION) {
            swtch *sw = (swtch *)m1->equipement;
            station *st = (station *)m2->equipement;
            if (sw->port_utilises < sw->nb_port) {
                int port = sw->port_utilises;
                sw->tab_association[port].port = port;
                sw->tab_association[port].st_MAC = st->st_MAC;
                sw->port_utilises++;
            }
            else {
                perror("Erreur : trop de connexions sur le switch\n");
                return ERROR;
            }  
        }
        else if (m2->tp_equip == TYPE_SWITCH && m1->tp_equip == TYPE_STATION) {
            swtch *sw = (swtch *)m2->equipement;
            station *st = (station *)m1->equipement;
            if (sw->port_utilises < sw->nb_port) {
                int port = sw->port_utilises;
                sw->tab_association[port].port = port;
                sw->tab_association[port].st_MAC = st->st_MAC;
                sw->port_utilises++;
            }
            else {
                perror("Erreur : trop de connexions sur le switch\n");
                return ERROR;
            }  
        }
        // si deux switch 
        else if (m1->tp_equip == TYPE_SWITCH && m2->tp_equip == TYPE_SWITCH) {
            swtch *sw1 = (swtch *)m1->equipement;
            swtch *sw2 = (swtch *)m2->equipement;
            if (sw1->port_utilises < sw1->nb_port) {
                int port1 = sw1->port_utilises;
                sw1->tab_association[port1].port = port1;
                sw1->tab_association[port1].st_MAC = sw2->sw_MAC;
                sw1->port_utilises++;
            }
            else {
                perror("Erreur : trop de connexions sur le switch\n");
                return ERROR;
            }  

            if (sw2->port_utilises < sw2->nb_port) {
                int port2 = sw2->port_utilises;
                sw2->tab_association[port2].port = port2;
                sw2->tab_association[port2].st_MAC = sw1->sw_MAC;
                sw2->port_utilises++;
            }
            else {
                perror("Erreur : trop de connexions sur le switch\n");
                return ERROR;
            }  
        }
    }
    
    fclose(fichier);
    return SUCCESS;
}

void affichage_reseau(reseau *g){
    size_t nb = g->nbr_machines;
    printf("----------------RESEAU----------------\n");
    printf("Nombre de machines : \t%zu\n\n", nb);
    for (int i=0; i<nb; i++){
        machine m = g->machines[i];
        printf("-------------Machine n°%d-------------\n",i);
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
            printf("---------TABLEAU DE COMMUTATION---------\n");
            for (int j=0; j<equip->port_utilises; j++){
                char str1[18];
                association asso = equip->tab_association[j];
                mac_to_str(asso.st_MAC, str1);
                printf("Addresse (%s) --> port %d\n", str1, asso.port);
            }

        }
        printf("\n");
    }
    
    printf("---------------LIAISONS--------------\n");
    for (int p=0; p<g->nb_aretes; p++){
        arete art = g->aretes[p];
        printf("%d --> %d : %ld\n", art.m1.id, art.m2.id, art.poids);
    }
}

void deinit_reseau(reseau *r) {
    if (r == NULL) return;
    
    // libération des machines
    for (int i = 0; i < r->nbr_machines; i++) {
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
    r->nbr_machines=0;
    if (r->machines != NULL) {
        free(r->machines);
        r->machines = NULL;
    }
    if (r->aretes != NULL) {
        free(r->aretes);
        r->aretes = NULL;
    }
    r->nbr_machines = 0;
    r->nb_aretes = 0;
    r->aretes_capacite=0;
}

