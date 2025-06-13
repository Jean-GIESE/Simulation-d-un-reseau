#pragma once
#ifndef ALGOS_H
#define ALGOS_H
#include "reseau.h"

int trouver_station_par_nom(Reseau *r, const char *nom);
int trouver_station_par_mac(Reseau *r, const MAC mac[6]);
void envoyer_trame(Reseau *r, size_t id_station, Trame *t);
void transmettre_trame(Reseau *r, Sommet *current, Trame *t, uint32_t port_entree);

#endif
