#include "reseau.h"
#include <string.h>
#include <stdio.h>

void init_sommet(Sommet *s) {
    if (!s) return;

    // Nettoyer la mémoire de la structure (précaution)
    memset(s, 0, sizeof(Sommet));

    switch (s->type) {
        case TYPE_STATION:
            // Initialisation d’une station
            memset(s->objet.station.adrMAC, 0, 6);
            memset(s->objet.station.adrIP, 0, 4);
            strcpy(s->objet.station.nom, "Station par défaut");
            break;

        case TYPE_SWITCH:
            // Initialisation d’un switch
            memset(s->objet.sw.adrMAC, 0, 6);
            s->objet.sw.nb_ports = 4; // valeur par défaut
            s->objet.sw.priorite = 0;
            strcpy(s->objet.sw.nom, "Switch par défaut");

            // Allocation dynamique de la table de commutation
            s->objet.sw.tabCommutation = malloc(sizeof(Commutation) * s->objet.sw.nb_ports);
            if (s->objet.sw.tabCommutation != NULL) {
                for (size_t i = 0; i < s->objet.sw.nb_ports; i++) {
                    memset(s->objet.sw.tabCommutation[i].adrMAC, 0, 6);
                    s->objet.sw.tabCommutation[i].port = 0;
                }
            }
            break;

        default:
            break;
    }
}

void deinit_sommet(Sommet *s) {
    if (!s) return;

    if (s->type == TYPE_SWITCH && s->objet.sw.tabCommutation != NULL) {
        free(s->objet.sw.tabCommutation);
        s->objet.sw.tabCommutation = NULL;
    }
}

void print_mac(MAC mac[6]) {
    for (int i = 0; i < 6; i++) {
        printf("%02X", mac[i]);
        if (i < 5) printf(":");
    }
}

void print_ip(IP ip[4]) {
    for (int i = 0; i < 4; i++) {
        printf("%d", ip[i]);
        if (i < 3) printf(".");
    }
}
