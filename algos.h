#pragma once
#ifndef ALGOS_H
#define ALGOS_H
#include "reseau.h"

int trouver_station_par_nom(Reseau *r, const char *nom);
int trouver_station_par_mac(Reseau *r, const MAC mac[6]);
void envoyer_trame(Reseau *r, size_t id_station, Trame *t);
void transmettre_trame(Reseau *r, Sommet *current, Trame *t, uint32_t port_entree);
void initialiser_bpdu(Switch *sw, BPDU *bpdu, uint16_t port);
int comparer_bpdu(const BPDU *b1, const BPDU *b2);
int traiter_bpdu(Switch *sw, BPDU *courant, const BPDU *recu, uint16_t port);
void envoyer_bpdu(Switch *sw, Reseau *reseau, size_t id_sw, BPDU *bpdu_table);
void stp(Reseau *reseau);
void apprendre_mac(Switch *sw, const MAC adr_source, size_t port_entree);
int creer_trame_depuis_noms(const Reseau *r, const char *nom_source, const char *nom_dest, Trame *t);

#endif
