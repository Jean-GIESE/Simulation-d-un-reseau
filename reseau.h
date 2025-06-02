#pragma once

#include <stdlib.h>
#include <stdint.h>

typedef uint8_t MAC;
typedef uint8_t IP;

typedef struct Station {
    MAC adrMAC;
    IP adrIP;
    char nom[32];
} Station;

typedef struct Hub {
    char nom[32];
    size_t nb_ports;
} Hub;

typedef struct Trame {
    MAC destination;
    MAC source;
    char data[1500];
} Trame;

typedef struct Commutation {
    MAC adrMAC;
    uint32_t port;
} Commutation;
    
typedef struct Switch {
    char nom[32];
    MAC adrMAC;
    size_t nb_ports;
    uint16_t priorite;
    Commutation *tabCommutation;
} Switch;

typedef struct Sommet {
    MAC adrMAC;
    void* type;
} Sommet;

typedef enum TypeSommet {
    SWITCH,
    HUB,
    STATION
} TypeSommet;

void afficher_Switch(Switch const sw);









