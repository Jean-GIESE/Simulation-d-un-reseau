#include "algos.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

int trouver_station_par_nom(Reseau *r, const char *nom) {
    for (size_t i = 0; i < r->nb_sommets; i++) {
        if (r->sommets[i].type == TYPE_STATION &&
            strcmp(r->sommets[i].objet.station.nom, nom) == 0) {
            return i;
        }
    }
    return -1; // Pas trouvé
}

int trouver_station_par_mac(Reseau *r, const MAC mac[6]) {
    for (size_t i = 0; i < r->nb_sommets; i++) {
        if (r->sommets[i].type == TYPE_STATION &&
            memcmp(r->sommets[i].objet.station.adrMAC, mac, 6) == 0) {
            return i;
        }
    }
    return -1;
}
void envoyer_trame(Reseau *r, size_t id_station, Trame *t) {
    if (!r || !t || id_station >= r->nb_sommets) return;

    Sommet *src = &r->sommets[id_station];
    if (src->type != TYPE_STATION) return;

    printf("Station %s envoie une trame\n", src->objet.station.nom);

    for (size_t i = 0; i < r->nb_liens; i++) {
        Lien *lien = &r->liens[i];
        if (lien->s1 == src) {
            transmettre_trame(r, lien->s2, t, lien->port_s2);
        } else if (lien->s2 == src) {
            transmettre_trame(r, lien->s1, t, lien->port_s1);
        }
    }
}



void transmettre_trame(Reseau *r, Sommet *current, Trame *t, uint32_t port_entree) {
    if (!r || !current || !t) return;

    if (current->type == TYPE_STATION) {
        Station *st = &current->objet.station;
        if (memcmp(st->adrMAC, t->destination, 6) == 0) {
            printf("Station %s a reçu la trame !\n", st->nom);
        }
        return;
    }

    Switch *sw = &current->objet.sw;
    printf("Switch %s reçoit la trame sur le port %u\n", sw->nom, port_entree);

    // Apprentissage : mémoriser l'adresse source si inconnue
    bool connue = false;
    for (size_t i = 0; i < sw->nb_entrees; i++) {
        if (memcmp(sw->tabCommutation[i].adrMAC, t->source, 6) == 0) {
            connue = true;
            printf("[%s] Adresse source déjà connue sur le port %u\n", sw->nom, sw->tabCommutation[i].port);
            break;
        }
    }
    if (!connue && sw->nb_entrees < sw->capacite) {
        memcpy(sw->tabCommutation[sw->nb_entrees].adrMAC, t->source, 6);
        sw->tabCommutation[sw->nb_entrees].port = port_entree;
        sw->nb_entrees++;
        printf("[%s] Nouvelle adresse source apprise sur le port %u, total entrées : %zu\n",
               sw->nom, port_entree, sw->nb_entrees);
    }

    // Recherche du port correspondant à l'adresse destination
    uint32_t port_dest = UINT32_MAX;
    for (size_t i = 0; i < sw->nb_entrees; i++) {
        if (memcmp(sw->tabCommutation[i].adrMAC, t->destination, 6) == 0) {
            port_dest = sw->tabCommutation[i].port;
            printf("[%s] Adresse destination connue sur le port %u\n", sw->nom, port_dest);
            break;
        }
    }

    if (port_dest == UINT32_MAX) {
        printf("[%s] Adresse destination inconnue, diffusion sur tous les ports sauf %u\n", sw->nom, port_entree);
    } else {
        printf("[%s] Envoi de la trame uniquement sur le port %u\n", sw->nom, port_dest);
    }

    // Transmission de la trame sur les ports adéquats
    for (size_t i = 0; i < r->nb_liens; i++) {
        Lien *lien = &r->liens[i];
        if (lien->s1 != current && lien->s2 != current) {
            continue;
        }
        bool is_s1 = (lien->s1 == current);
        uint32_t port_local = is_s1 ? lien->port_s1 : lien->port_s2;
        uint32_t port_voisin = is_s1 ? lien->port_s2 : lien->port_s1;
        Sommet *voisin = is_s1 ? lien->s2 : lien->s1;

        // Ne pas renvoyer sur le port d'entrée (évite boucle)
        if (port_local == port_entree) {
            printf("[%s] Ignorer le port d'entrée %u pour éviter boucle\n", sw->nom, port_local);
            continue;
        }
        // Vérifier que le port est actif
        if (sw->etat_ports[port_local] != ACTIF) {
            printf("[%s] Port %u non actif, on ne transmet pas\n", sw->nom, port_local);
            continue;
        }

        // Diffuser si destination inconnue, ou envoyer uniquement sur le port destination
        if (port_dest == UINT32_MAX || port_dest == port_local) {
            printf("[%s] Transmission de la trame au sommet sur le port %u\n", sw->nom, port_local);
            transmettre_trame(r, voisin, t, port_voisin);
        }
    }
}