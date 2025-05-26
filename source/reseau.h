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
} graphe;