#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "machines.h"

#define ERROR 1
#define SUCCESS 0

typedef struct arete
{
	machine m1;
	machine m2;
	size_t poids;
} arete;

typedef struct reseau
{
	size_t nbr_machines;
	machine *machines;
	arete *aretes;
	size_t aretes_capacite;
	size_t nb_aretes;
} reseau;

int charger_reseau(const char* nom_fichier, reseau* g);
void affichage_reseau(reseau *g);