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
    if (fscanf(fichier, "%d %d\n", &nb_machines, &nb_liens)!=2){
        //si ne lis pas 2 entiers pour la première ligne
        perror("Erreur dans la lecture du fichier");
        return ERROR;
    }

    //Initialisation de ma structure reseau
    g->nbr_machines = nb_machines;
    g->nb_aretes = nb_liens;
    g->aretes = malloc(nb_liens*sizeof(arete));
    g->aretes_capacite = nb_liens;
    g->machines = malloc(nb_machines * sizeof(machine));

    //config des machines a partir du fichier 
    for (size_t i = 0; i < nb_machines; i++) {
        //récupérer la ligne dans une variable ligne
        char ligne[MAX_BUFFER_SIZE];
        fgets(ligne, MAX_BUFFER_SIZE, fichier);

        int type;   //1 pour station et 2 pour switch

        char info1[32];
        char info2[32];
        char info3[32];
        int nb = sscanf(ligne, "%d;%31[^;];%31[^;];%31[^\n]", &type, info1, info2, info3);
        //%31 -> Limite à 31 caractères maximum
        // [^;]	lit tout sauf ;

        machine* m = &g->machines[i];
        m->id = i;
        m->tp_equip = (type == 1) ? TYPE_STATION : TYPE_SWITCH;

        if (type == 1) {  //si 1 alors station
            station* st = malloc(sizeof(station));
            //Adresse mac
            str_to_mac(&st->st_MAC, info1);

            //Adresse IP
            str_to_mac(&st->st_IP, info2);

            //rajouter la station dans equipement
            m->equipement = st;

        } 
        else if (type == 2) {  //si 2 alors switch
            int nombre;
            nombre = atoi(info2);   //convertir une chaine de caractère en entier
            swtch* sw = malloc(sizeof(swtch) + sizeof(adresse_MAC) * nombre); // tab_voisins

            //Adresse Mac
            str_to_mac(&sw->sw_mac, info1);

            sw->nb_port = nombre;
            sw->priorite = atoi(info3);
            sw->tab_association = malloc(sizeof(association)*nombre);
            m->equipement = sw;
        }
        else if(nb!=4 && nb!=3){
            perror("La ligne est mal formé");
        }
    }

    //création des associations (src;dst;poids)
    for (int i = 0; i < nb_liens; i++) {
        int src;
        int dst;
        int poids;

        bool trv = false;

        fscanf(fichier, "%d;%d;%d\n", &src, &dst, &poids);
        for (int j=0; j<nb_machines; j++){
            arete art = g->aretes[i];   
            machine m1 = g->machines[j];
            //Rajouter le poids
            art.poids = poids;

            //recherche de l'id de la machine src
            if (m1.id == src){
                //si celui trouve la source alors cherche la destination 
                for (int p=0; p<nb_machines; p++){
                machine m2 = g->machines[p];
                    if(m2.id == dst){
                        trv = true;
                        //affetcer a la n-ième arrete les données
                        art.m1 = m1;
                        art.m2 = m2;
                    }
                }
            }
        }
        if(!trv){
            perror("La source/destination n'a pas était trouvée (Index de la machine inconnu)");
            return ERROR;
        }
    }

    fclose(fichier);

    //free sinon on va nous taper
    free(fichier);
    fichier = NULL;

    return SUCCESS;
}
