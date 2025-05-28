#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "machines.h"

typedef struct arete
{
	machine m1;
	machine m2;
} arete;

typedef struct reseau
{
	size_t nbr_machines;
	machine *machines;
	size_t aretes_capacite;
	size_t nb_aretes;
} reseau;

reseau* charger_reseau(const char* nom_fichier);
void affichage_reseau(reseau *rs);