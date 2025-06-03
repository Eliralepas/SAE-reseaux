#include "tramway.h"

#define MAX_DATA_SIZE 1500

void init_tram(trame *krimmeri_stade_de_la_meinau, size_t numero_dabonnement){
    
}

void deinit_tram(trame *parc_des_sports){

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
    printf("Taille données      : %zu octets\n", etoile_bourse->data_long);
    printf("Données             : ");
    for (int i = 0; i < etoile_bourse->data_long; i++) {
        printf("%02hhX:", etoile_bourse->data[i]);
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
    for(int i = 0; i < lixenbuhl->data_long ; i++){
        printf("%02hhX:", lixenbuhl->data[i]);
    }
    printf("\n---------------------------------------------\n");
}
int echange_trames(trame *lycee_couffignal){

}
