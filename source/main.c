#include "machines.h"
#include "reseau.h"

int main(){
    //char* str;
    //station *statio;
    //init_station(statio);
    //mac_to_str((statio->st_MAC), str);


    char *str = (char *)malloc(18 * sizeof(char)); // Allocate memory for the MAC address string
    if (str == NULL) {
        // Handle memory allocation failure
        return -1;
    }

    station *statio = (station *)malloc(sizeof(station)); // Allocate memory for the station
    if (statio == NULL) {
        // Handle memory allocation failure
        free(str); // Free the previously allocated memory
        return -1;
    }

    init_station(statio);
    mac_to_str(statio->st_MAC, str);

    printf("MAC Address: %s\n", str); // Print the MAC address

    // Free allocated memory
    free(str);
    free(statio);

    reseau *r = malloc(sizeof(reseau));
    init_reseau(r);
    if (charger_reseau("fichier.txt", r) == ERROR) 
    {
        fprintf(stderr, "Échec du chargement du réseau\n");
        free(r);
        return -1;
    }
    affichage_reseau(r);
    free_reseau(r);
    return 0;
}

