#pragma once

#include <stdlib.h>
#include <stdint.h>

typedef uint8_t MAC;
typedef uint8_t IP;

typedef struct Station {
    MAC adrMAC[6];
    IP adrIP[4];
    char nom[32];
} Station;

typedef struct Hub {
    char nom[32];
    size_t nb_ports;
} Hub;

typedef struct Trame {
    MAC destination[6];
    MAC source[6];
} Trame;

typedef struct Commutation {
    MAC adrMAC[6];
    uint32_t port;
} Commutation;

typedef struct Switch {
    char nom[32];
    MAC adrMAC[6];
    size_t nb_ports;
    uint16_t priorite;
    Commutation *tabCommutation;
} Switch;

typedef enum {
    TYPE_HUB,
    TYPE_SWITCH,
    TYPE_STATION
} TypeObjet;

typedef struct Sommet {
    TypeObjet type;
    union {
        Station station;
        Switch sw;
        Hub hub;
    } objet;
} Sommet;

typedef enum TypeSommet {
    SWITCH,
    HUB,
    STATION
} TypeSommet;

typedef struct Lien
{
    Sommet s1;
    Sommet s2;
} Lien;

//void afficher_Switch(Switch const sw);









