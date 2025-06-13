#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include "machines.h" 
#include "reseau.h"

void protocole_STP_chemin(reseau *r);
void affichage_port_etat(reseau *r);