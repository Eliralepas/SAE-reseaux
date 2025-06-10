#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include "machines.h" 
#include "reseau.h"
#include "paquet_IP.h"

void protocole_STP_chemin(reseau *r);
uint8_t calcul_cout(reseau *r, machine *m, int* visites);