#pragma once
#ifndef RESEAU_H
#define RESEAU_H

#include <stdlib.h>
#include <stdint.h>

typedef uint8_t MAC;
typedef uint8_t IP;

typedef struct Station {
    MAC adrMAC[6];
    IP adrIP[4];
    char nom[32];
} Station;

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
    TYPE_SWITCH,
    TYPE_STATION
} TypeObjet;

typedef struct Sommet {
    TypeObjet type;
    union {
        Station station;
        Switch sw;
    } objet;
} Sommet;

typedef enum TypeSommet {
    SWITCH,
    STATION
} TypeSommet;

typedef struct Lien {
    Sommet s1;
    Sommet s2;
} Lien;

typedef struct Reseau {
  size_t nb_sommets;
  size_t nb_liens;
  Sommet *sommets;
  Lien *liens;
} Reseau;

//void afficher_Switch(Switch const sw);
void init_reseau(Reseau *r);
void deinit_reseau(Reseau *r);
int allouer_reseau(Reseau *r, size_t nb_sommets, size_t nb_liens);
void creer_Reseaux(char* nomFichier);
void init_sommet(Sommet *s);
void deinit_sommet(Sommet *s);
void print_mac(MAC mac[6]);
void print_ip(IP ip[4]);

#endif
