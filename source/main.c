#include "machines.h"
#include "reseau.h"
#include "tramway.h" // Assurez-vous d'inclure le fichier d'en-tête pour les trames

int main() {
    // Initialisation du réseau
    reseau *r = malloc(sizeof(reseau));
    if (r == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour le réseau\n");
        return -1;
    }

    init_reseau(r);
    if (charger_reseau("fichier.txt", r) == ERROR) {
        fprintf(stderr, "Échec du chargement du réseau\n");
        deinit_reseau(r);
        free(r);
        return -1;
    }

    // Affichage du réseau pour vérification
    affichage_reseau(r);

    // Création d'une trame pour le test
    trame *testTrame = malloc(sizeof(trame));
    if (testTrame == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour la trame\n");
        deinit_reseau(r);
        free(r);
        return -1;
    }

    // Initialisation de la trame avec des valeurs fictives pour le test
    // Supposons que vous avez des fonctions pour obtenir des machines source et destination
    machine source = r->machines[0]; // Exemple : première machine comme source
    machine destination = r->machines[1]; // Exemple : deuxième machine comme destination
    init_tram(testTrame, 100, source, destination); // 100 est un exemple de taille de données

    // Échange de la trame
    if (echange_trames(testTrame, r) == ERROR) {
        fprintf(stderr, "Échec de l'échange de trames\n");
    } else {
        printf("Échange de trames réussi\n");
    }

    // Affichage de la trame pour vérification
    affich_tram_utilisasteur(testTrame);

    // Libération de la mémoire
    deinit_tram(testTrame);
    free(testTrame);
    deinit_reseau(r);
    free(r);

    return 0;
}
