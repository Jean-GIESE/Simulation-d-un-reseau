#include <stdio.h>
#include <stdlib.h>

#include "reseau.h"
#include <string.h>

// void afficher_Switch(Switch sw)
// {
//     
// }
void init_reseau(Reseau *r) {
    if (!r) return;
    memset(r, 0, sizeof(Reseau));
}

void deinit_reseau(Reseau *r) {
    if (!r) return;

    for (size_t i = 0; i < r->nb_sommets; i++) {
        deinit_sommet(&r->sommets[i]);  // libère les ressources dynamiques internes
    }

    free(r->sommets);
    free(r->liens);
    
    r->sommets = NULL;
    r->liens = NULL;
    r->nb_sommets = 0;
    r->nb_liens = 0;
}

int allouer_reseau(Reseau *r, size_t nb_sommets, size_t nb_liens) {
    if (!r) return -1;

    r->sommets = malloc(nb_sommets * sizeof(Sommet));
    if (!r->sommets) return -1;
    memset(r->sommets, 0, nb_sommets * sizeof(Sommet));

    r->liens = malloc(nb_liens * sizeof(Lien));
    if (!r->liens) {
        free(r->sommets);
        r->sommets = NULL;
        return -1;
    }
    memset(r->liens, 0, nb_liens * sizeof(Lien));

    r->nb_sommets = nb_sommets;
    r->nb_liens = nb_liens;

    return 0;
}


void creer_Reseaux(char* nomFichier)
{
    FILE * fichier = fopen(nomFichier,"r");
    
    char ligne[32];
    
    if (fichier != NULL) {
        while (fgets(ligne, sizeof(ligne), fichier)) {
            printf("%s", ligne);
        }
        fclose(fichier);
    }
    else {
        fprintf(stderr, "Impossible d'ouvrir le fichier!\n");
    }
}

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
