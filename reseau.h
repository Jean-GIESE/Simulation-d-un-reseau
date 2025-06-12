#pragma once
#ifndef RESEAU_H
#define RESEAU_H

#include <stdlib.h>
#include <stdint.h>

typedef uint8_t MAC;
typedef uint8_t IP;

typedef enum EtatPort {
    BLOQUE,
    ACTIF
} EtatPort;

typedef struct Station {
    MAC adrMAC[6];
    IP adrIP[4];
    char nom[32];
} Station;

typedef struct Trame {
    uint8_t preambule[7];
    uint8_t sfd;
    MAC destination[6];
    MAC source[6];
    uint16_t type;
    uint8_t donnees[46];
    uint32_t fcs;
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
    EtatPort *etat_ports;
    Commutation *tabCommutation;
    size_t nb_entrees;
    size_t capacite;
} Switch;

typedef enum TypeObjet{
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

typedef struct Lien {
    Sommet *s1;
    Sommet *s2;
    uint16_t poids;
    uint32_t port_s1;
    uint32_t port_s2;
} Lien;

typedef struct Reseau {
  size_t nb_sommets;
  size_t nb_liens;
  Sommet *sommets;
  Lien *liens;
} Reseau;

typedef struct BPDU{
    uint16_t priorite_root;
    MAC mac_root[6];
    uint16_t cout_chemin;
    MAC mac_emetteur[6];
    uint16_t port_emetteur;
} BPDU;

void init_reseau(Reseau *r);
void deinit_reseau(Reseau *r);
int allouer_reseau(Reseau *r, size_t nb_sommets, size_t nb_liens);
void creer_reseau(char* nomFichier, Reseau *reseau);
void init_sommet(Sommet *s);
void deinit_sommet(Sommet *s);
void init_trame(Trame *t);
void init_lien(Lien *l);
void deinit_lien(Lien *l);
void init_bpdu(BPDU *b);
void print_mac(const MAC mac[6]);
void print_ip(const IP ip[4]);
void afficher_station(const Station *st);
void afficher_switch(const Switch *sw);
void afficher_sommet(const Sommet *s);
void afficher_lien(const Lien *l);
void afficher_reseau(const Reseau *r);
void afficher_trame_user(const Trame *t);
void afficher_trame(const Trame *t);

#endif
