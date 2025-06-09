#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include "machines.h" 
#include "reseau.h"
#include "paquet_IP.h"

typedef struct bpdu{
    uint16_t root_id;
    uint16_t bridge_id;
    uint8_t  port_id;
    uint8_t  cost;
} bpdu;