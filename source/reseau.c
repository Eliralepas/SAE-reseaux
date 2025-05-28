#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machines.h"
#include "reseau.h"


//Lecture du fichier de configuration renvoie un structure reseau, si celui ci n'est pas possible alors renvoie null
reseau* charger_reseau(const char* nom_fichier) {

    //Si le fichier n'existe pas
    FILE* fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        perror("Erreur d'ouverture du fichier");
        return NULL;
    }

    size_t nb_machines;
    size_t nb_liens;
    //lire des données de la première ligne 
    fscanf(fichier, "%zu %zu\n", &nb_machines, &nb_liens);

    //instanciation de ma structure reseau
    reseau* g = malloc(sizeof(reseau));
    g->nbr_machines = nb_machines;
    g->nb_aretes = nb_liens;
    g->aretes_capacite = nb_liens;
    g->machines = malloc(nb_machines * sizeof(machine));

    //config des machines a partir du fichier 
    for (size_t i = 0; i < nb_machines; i++) {
        //récupérer la ligne dans une variable ligne
        char ligne[255];
        fgets(ligne, 255, fichier);

        int type;   //1 pour station et 2 pour switch

        char info1[32];
        char info2[32];
        char info3[32];
        sscanf(ligne, "%d;%31[^;];%31[^;];%31[^\n]", &type, info1, info2, info3);
        //%31 -> Limite à 31 caractères maximum
        // [^;]	lit tout sauf ;

        machine* m = &g->machines[i];
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
    }

    // Sauter les \n potentiels après dernière machine
    while (fgetc(fichier) != '\n' && !feof(fichier));

    //création des associations (src;dst;poids)
    for (size_t i = 0; i < nb_liens; i++) {
        int src;
        int dst;
        int poids;
        fscanf(fichier, "%d;%d;%d\n", &src, &dst, &poids);
        //association a les foutre dans la tab_voisin
        //donner des id au machine pour pas se perdre
    }

    fclose(fichier);
    return g;
}
