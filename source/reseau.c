#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machines.h"
#include "reseau.h"

#define MAX_BUFFER_SIZE 255


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
    if (fscanf(fichier, "%zu %d\n", &nb_machines, &nb_liens)!=2){
        //si ne lis pas 2 entiers pour la première ligne
        perror("Erreur dans la lecture du fichier");
        fclose(fichier);
        return ERROR;
    }

    //Initialisation de ma structure reseau
    g->nbr_machines = nb_machines;
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
        m->tp_equip = (type == 1) ? TYPE_STATION : TYPE_SWITCH;

        if (type == 1) {  //si 1 alors station
            station* st = malloc(sizeof(station));
            //Adresse mac
            str_to_mac(&st->st_MAC, info1);

            //Adresse IP
            str_to_ip(&st->st_IP, info2);

            //rajouter la station dans equipement
            m->equipement = st;

        } 
        else if (type == 2) {  //si 2 alors switch
            int nombre = atoi(info2);   //convertir une chaine de caractère en entier
            swtch* sw = malloc(sizeof(swtch)); // tab_voisins
            //Adresse Mac
            str_to_mac(&sw->sw_MAC, info1);
            sw->nb_port = nombre;
            sw->priorite = atoi(info3);
            sw->tab_association = malloc(sizeof(association)*nombre);
            sw->port_utilises = 0;
            m->equipement = sw;
        }
        
    }

    //création des associations (src;dst;poids)

    //tab_voisin !!

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
    printf("Nombre de machines : \t%zu\n", nb);
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
            printf("La priorité : %hhn\n", equip->priorite);
            printf("---------TABLEAU DE COMMUTATION---------\n");
            for (int i=0; i<equip->nb_port; i++){
                char str1[18];
                association asso = equip->tab_association[i];
                mac_to_str(asso.st_MAC, str1);
                printf("Addresse (%s) --> port %d\n", str1, asso.port);
            }

        }

    }

}
