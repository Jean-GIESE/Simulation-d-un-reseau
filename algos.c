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

int creer_trame_depuis_noms(const Reseau *r, const char *nom_source, const char *nom_dest, Trame *t) {
    if (!r || !nom_source || !nom_dest || !t) return -1;

    MAC mac_source[6], mac_dest[6];
    int found_src = 0, found_dst = 0;

    for (size_t i = 0; i < r->nb_sommets; i++) {
        if (r->sommets[i].type != TYPE_STATION) continue;
        if (found_src && found_dst) break;
        Station *st = &r->sommets[i].objet.station;
        if (strcmp(st->nom, nom_source) == 0) {
            memcpy(mac_source, st->adrMAC, 6);
            found_src = 1;
        }
        if (strcmp(st->nom, nom_dest) == 0) {
            memcpy(mac_dest, st->adrMAC, 6);
            found_dst = 1;
        }
    }

    if (!found_src || !found_dst) {
        fprintf(stderr, "Erreur : station %s ou %s introuvable\n", nom_source, nom_dest);
        return -1;
    }

    init_trame(t);
    memcpy(t->source, mac_source, 6);
    memcpy(t->destination, mac_dest, 6);
    strcpy((char*)t->donnees, "Hello depuis creer_trame_depuis_noms");
    return 0;
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
    if (!connue) {
      // Si la table est pleine, doubler sa capacité
      if (sw->nb_entrees >= sw->capacite) {
          size_t nouvelle_capacite = sw->capacite * 2;
          Commutation *new_tab = realloc(sw->tabCommutation, nouvelle_capacite * sizeof(Commutation));
          if (new_tab == NULL) {
              fprintf(stderr, "[%s] ERREUR : Échec du realloc, apprentissage impossible\n", sw->nom);
              return;
          }
          sw->tabCommutation = new_tab;
          sw->capacite = nouvelle_capacite;
          printf("[%s] Table de commutation agrandie à %zu entrées\n", sw->nom, sw->capacite);
      }

      // Ajout de l'entrée après avoir assuré la place
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



// Initialise un BPDU avec les infos du switch (considéré comme racine)
void initialiser_bpdu(Switch *sw, BPDU *bpdu, uint16_t port) {
    init_bpdu(bpdu);
    bpdu->priorite_root = sw->priorite;
    memcpy(bpdu->mac_root, sw->adrMAC, 6);
    bpdu->cout_chemin = 0;
    memcpy(bpdu->mac_emetteur, sw->adrMAC, 6);
    bpdu->port_emetteur = port;
}



// Compare deux BPDU (retourne -1 si b1 est meilleur, 1 si b2 est meilleur, 0 si égaux)
int comparer_bpdu(const BPDU *b1, const BPDU *b2) {
    if (b1->priorite_root != b2->priorite_root)
        return (b1->priorite_root < b2->priorite_root) ? -1 : 1;
    
    int cmp = memcmp(b1->mac_root, b2->mac_root, 6);
    if (cmp != 0) return (cmp < 0) ? -1 : 1;

    if (b1->cout_chemin != b2->cout_chemin)
    return (b1->cout_chemin < b2->cout_chemin) ? -1 : 1;
    
    cmp = memcmp(b1->mac_emetteur, b2->mac_emetteur, 6);
    if (cmp != 0) return (cmp < 0) ? -1 : 1;

    return 0;
}

// Traite un BPDU reçu, retourne 1 si le switch a mis à jour sa configuration
int traiter_bpdu(Switch *sw, BPDU *courant, const BPDU *recu, uint16_t port, uint16_t cout_lien) {
    BPDU nouveau = *recu;
    nouveau.cout_chemin += cout_lien;

    nouveau.port_emetteur = port;

    if (comparer_bpdu(&nouveau, courant) < 0) {
        *courant = nouveau;
        return 1;
    }
    return 0; 
}

// Envoie un BPDU à tous les voisins d'un switch
// Envoie un BPDU à tous les voisins d'un switch
void envoyer_bpdu(Switch *sw, Reseau *reseau, size_t id_sw, BPDU *bpdu_table) {
    for (size_t i = 0; i < reseau->nb_liens; i++) {
        Lien *lien = &reseau->liens[i];

        // On s'intéresse uniquement aux liens entre switches
        if (lien->s1->type == TYPE_SWITCH && lien->s2->type == TYPE_SWITCH) {
            size_t voisin_id = (lien->s1 == &reseau->sommets[id_sw]) ? 
                               (lien->s2 - reseau->sommets) : 
                               (lien->s1 == &reseau->sommets[id_sw] ? (lien->s1 - reseau->sommets) : SIZE_MAX);

            if (voisin_id == SIZE_MAX) continue; // Pas un voisin du switch courant

            Switch *voisin = &reseau->sommets[voisin_id].objet.sw;

            // On prépare un BPDU "envoyé" : celui du switch courant,
            // avec coût chemin augmenté du coût du lien entre sw et voisin
            BPDU bpdu_envoye = bpdu_table[id_sw];
            
            // Ajout du coût du lien au BPDU envoyé
            bpdu_envoye.cout_chemin += lien->poids;

            // On indique que c’est le port d’émission de sw sur ce lien
            bpdu_envoye.port_emetteur = (lien->s1 == &reseau->sommets[id_sw]) ? lien->port_s1 : lien->port_s2;

            // Traiter le BPDU reçu par le voisin sur son port correspondant
            uint16_t port_entrant_voisin = (lien->s1 == &reseau->sommets[voisin_id]) ? lien->port_s1 : lien->port_s2;

            int updated = traiter_bpdu(voisin, &bpdu_table[voisin_id], &bpdu_envoye, port_entrant_voisin, 0 /* on ajoute plus le cout ici car déjà dans bpdu_envoye*/);

        }
    }
}


// Lancement du protocole STP jusqu'à convergence
void stp(Reseau *reseau) {
    BPDU *bpdu_table = malloc(reseau->nb_sommets * sizeof(BPDU));
    if (!bpdu_table) {
        fprintf(stderr, "Erreur allocation mémoire\n");
        return;
    }

    // Initialisation des BPDU avec infos des switches
    for (size_t i = 0; i < reseau->nb_sommets; i++) {
        if (reseau->sommets[i].type == TYPE_SWITCH) {
            initialiser_bpdu(&reseau->sommets[i].objet.sw, &bpdu_table[i], 0);
        }
    }

    int convergence = 0;
    while (!convergence) {
        convergence = 1; // On part du principe que c’est convergé

        // Parcours de tous les switches
        for (size_t i = 0; i < reseau->nb_sommets; i++) {
            if (reseau->sommets[i].type == TYPE_SWITCH) {
                Switch *sw = &reseau->sommets[i].objet.sw;

                // Avant d’envoyer les BPDU aux voisins, on mémorise l’état courant
                BPDU bpdu_avant = bpdu_table[i];

                // Envoie BPDU aux voisins et traitement des mises à jour
                envoyer_bpdu(sw, reseau, i, bpdu_table);

                // Si le BPDU a changé, on n’est pas encore convergé
                if (comparer_bpdu(&bpdu_avant, &bpdu_table[i]) != 0) {
                    convergence = 0;
                }
            }
        }
    }
    appliquer_etats_ports(reseau, bpdu_table);
    printf("\nSTP converge. Switch racines :\n");
    for (size_t i = 0; i < reseau->nb_sommets; i++) {
        if (reseau->sommets[i].type == TYPE_SWITCH) {
            printf("%s : root = ", reseau->sommets[i].objet.sw.nom);
            print_mac(bpdu_table[i].mac_root);
            printf(", coût = %d\n", bpdu_table[i].cout_chemin);
        }
    }

    free(bpdu_table);
}


void apprendre_mac(Switch *sw, const MAC adr_source, size_t port_entree) {
    // Vérifier si déjà connue
    for (size_t i = 0; i < sw->nb_entrees; i++) {
        if (memcmp(sw->tabCommutation[i].adrMAC, adr_source, 6) == 0) {
            return; // Déjà connue
        }
    }

    // Nouvelle entrée
    if (sw->nb_entrees < sw->capacite) {
        memcpy(sw->tabCommutation[sw->nb_entrees].adrMAC, adr_source, 6);
        sw->tabCommutation[sw->nb_entrees].port = port_entree;
        sw->nb_entrees++;
    } else {
        fprintf(stderr, "Table de commutation pleine pour %s\n", sw->nom);
    }
}
void appliquer_etats_ports(Reseau *reseau, BPDU *bpdu_table) {
    // Réinitialiser tous les ports à BLOQUE par défaut
    for (size_t i = 0; i < reseau->nb_sommets; i++) {
        if (reseau->sommets[i].type == TYPE_SWITCH) {
            Switch *sw = &reseau->sommets[i].objet.sw;
            for (size_t p = 0; p < sw->nb_ports; p++) {
                sw->etat_ports[p] = BLOQUE;
            }
        }
    }

    // Identifier les ports racines et désignés
    for (size_t i = 0; i < reseau->nb_liens; i++) {
        Lien *lien = &reseau->liens[i];
        Sommet *s1 = lien->s1;
        Sommet *s2 = lien->s2;

        if (s1->type == TYPE_SWITCH && s2->type == TYPE_SWITCH) {
            size_t id1 = s1 - reseau->sommets;
            size_t id2 = s2 - reseau->sommets;
            Switch *sw1 = &s1->objet.sw;
            Switch *sw2 = &s2->objet.sw;

            BPDU *bpdu1 = &bpdu_table[id1];
            BPDU *bpdu2 = &bpdu_table[id2];

            // On choisit le meilleur BPDU sur ce lien
            int cmp = comparer_bpdu(bpdu1, bpdu2);

            if (cmp < 0) {
                // sw1 est meilleur, donc sw1 a un port désigné, sw2 port racine
                sw1->etat_ports[lien->port_s1] = ACTIF;
                sw2->etat_ports[lien->port_s2] = ACTIF;
            } else if (cmp > 0) {
                // sw2 est meilleur
                sw2->etat_ports[lien->port_s2] = ACTIF;
                sw1->etat_ports[lien->port_s1] = ACTIF;
            } else {
                // Égalité stricte : cas rare, on active les deux par défaut
                sw1->etat_ports[lien->port_s1] = ACTIF;
                sw2->etat_ports[lien->port_s2] = ACTIF;
            }
        }
    }
}
