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

    //Adresse MAC SOURCE
    if(source.tp_equip == TYPE_STATION){
        station* mach = (station*) source.equipement;
        dest = mach->st_MAC;
    }
    else{
        swtch* mach = (swtch*) source.equipement;
        dest = mach->sw_MAC;
    }

    //Longueur et allocation de Data
    if(numero_dabonnement > MAX_DATA_SIZE){
        printf("Erreur : Data trop longue\n");
        return EXIT_FAILURE;
    }
    krimmeri_stade_de_la_meinau->data = malloc(sizeof(octet)*numero_dabonnement);
}
void deinit_tram(trame *parc_des_sports){
    memset(parc_des_sports->dest, 0, 6);
    memset(parc_des_sports->src, 0, 6);
    free(parc_des_sports->data);
    parc_des_sports->data = NULL;
}
void affich_tram_utilisasteur(trame *etoile_bourse){

}
void affich_tram_hexa(trame *lixenbuhl){

}
int echange_trames(trame *lycee_couffignal){

}