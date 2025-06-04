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
    krimmeri_stade_de_la_meinau->data_longueur = numero_dabonnement;
    krimmeri_stade_de_la_meinau->data = malloc(sizeof(octet)*krimmeri_stade_de_la_meinau->data_longueur);
}

void deinit_tram(trame *parc_des_sports){
    memset(parc_des_sports->dest, 0, 6);
    memset(parc_des_sports->src, 0, 6);
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
        //printf("%02hhX:", etoile_bourse->data[i]);
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
        //printf("%02hhX:", lixenbuhl->data[i]);
    }
    printf("\n---------------------------------------------\n");
}

int echange_trames(trame *lycee_couffignal){

}

void envoie_trame(trame *t){

}

void recoit_trame(trame *t){

}