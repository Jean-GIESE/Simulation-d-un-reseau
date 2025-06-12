#include <stdio.h>
#include <stdlib.h>

#include "reseau.h"
#include <string.h>

void init_reseau(Reseau *r) {
    if (!r) return;
    memset(r, 0, sizeof(Reseau));
}

void deinit_reseau(Reseau *r) {
    if (!r) return;

    for (size_t i = 0; i < r->nb_sommets; i++) {
        deinit_sommet(&r->sommets[i]);
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
void creer_reseau(char* nomFichier, Reseau *reseau)
{
    FILE *fichier = fopen(nomFichier, "r");
    char ligne[128];

    if (!fichier) {
        fprintf(stderr, "Impossible d'ouvrir le fichier %s\n", nomFichier);
        return;
    }

    int nbEquipements, nbLiens;
    if (fscanf(fichier, "%d %d", &nbEquipements, &nbLiens) != 2) {
        fprintf(stderr, "Erreur de lecture du nombre d’équipements et de liens\n");
        fclose(fichier);
        return;
    }

    fgets(ligne, sizeof(ligne), fichier); // Consommer la fin de ligne

    init_reseau(reseau);
    if (allouer_reseau(reseau, nbEquipements, nbLiens) != 0) {
        fprintf(stderr, "Erreur d’allocation du réseau\n");
        fclose(fichier);
        return;
    }

    int numSwitch = 1, numStation = 1;

    for (int i = 0; i < nbEquipements; i++) {
        if (!fgets(ligne, sizeof(ligne), fichier)) break;

        int type = atoi(strtok(ligne, ";"));
        char *macStr = strtok(NULL, ";");
        MAC mac[6];
        char *ctx = NULL;
        char macCopy[32];
        strncpy(macCopy, macStr, sizeof(macCopy) - 1);
        macCopy[sizeof(macCopy) - 1] = '\0';
        char *octet = strtok_r(macCopy, ":", &ctx);
        for (int j = 0; j < 6 && octet; j++) {
            mac[j] = (uint8_t)strtol(octet, NULL, 16);
            octet = strtok_r(NULL, ":", &ctx);
        }

        if (type == 2) { // Switch
            reseau->sommets[i].type = TYPE_SWITCH;
            init_sommet(&reseau->sommets[i]);  // important : APRES avoir mis le type

            memcpy(reseau->sommets[i].objet.sw.adrMAC, mac, 6);
            reseau->sommets[i].objet.sw.nb_ports = atoi(strtok(NULL, ";"));
            reseau->sommets[i].objet.sw.priorite = atoi(strtok(NULL, ";"));
            sprintf(reseau->sommets[i].objet.sw.nom, "sw%d", numSwitch++);
        }
        else if (type == 1) { // Station
            reseau->sommets[i].type = TYPE_STATION;
            init_sommet(&reseau->sommets[i]);

            memcpy(reseau->sommets[i].objet.station.adrMAC, mac, 6);

            char *ipStr = strtok(NULL, ";");
            IP ip[4];
            char ipCopy[32];
            strncpy(ipCopy, ipStr, sizeof(ipCopy) - 1);
            ipCopy[sizeof(ipCopy) - 1] = '\0';
            char *tok = strtok(ipCopy, ".");
            for (int j = 0; j < 4 && tok; j++) {
                ip[j] = (uint8_t)atoi(tok);
                tok = strtok(NULL, ".");
            }
            memcpy(reseau->sommets[i].objet.station.adrIP, ip, 4);
            sprintf(reseau->sommets[i].objet.station.nom, "st%d", numStation++);
        }
    }

    for (int i = 0; i < nbLiens && fgets(ligne, sizeof(ligne), fichier); i++) {
        size_t s1 = atoi(strtok(ligne, ";"));
        size_t s2 = atoi(strtok(NULL, ";"));
        uint16_t poids = atoi(strtok(NULL, ";"));

        reseau->liens[i].s1 = &reseau->sommets[s1];
        reseau->liens[i].s2 = &reseau->sommets[s2];
        reseau->liens[i].poids = poids;
        reseau->liens[i].port_s1 = 0;
        reseau->liens[i].port_s2 = 0;
    }

    fclose(fichier);
}


void init_sommet(Sommet *s) {
    if (!s) return;

    TypeObjet type_courant = s->type;
    memset(s, 0, sizeof(Sommet));
    s->type = type_courant;

    switch (s->type) {
        case TYPE_STATION:
            memset(s->objet.station.adrMAC, 0, 6);
            memset(s->objet.station.adrIP, 0, 4);
            strcpy(s->objet.station.nom, "Station par défaut");
            break;

        case TYPE_SWITCH:
            memset(s->objet.sw.adrMAC, 0, 6);
            s->objet.sw.capacite = 4;
            s->objet.sw.nb_ports = 0;
            s->objet.sw.priorite = 0;
            s->objet.sw.nb_entrees = 0;
            strcpy(s->objet.sw.nom, "Switch par défaut");

            s->objet.sw.tabCommutation = malloc(sizeof(Commutation) * s->objet.sw.capacite);
            if (s->objet.sw.tabCommutation != NULL) {
                for (size_t i = 0; i < s->objet.sw.capacite; i++) {
                    memset(s->objet.sw.tabCommutation[i].adrMAC, 0, 6);
                    s->objet.sw.tabCommutation[i].port = 0;
                }
            }

            s->objet.sw.etat_ports = malloc(sizeof(EtatPort) * s->objet.sw.capacite);
            if (s->objet.sw.etat_ports != NULL) {
                for (size_t i = 0; i < s->objet.sw.capacite; i++) {
                    s->objet.sw.etat_ports[i] = ACTIF;
                }
            }

            s->objet.sw.nb_entrees = 0;
            break;

        default:
            break;
    }
}

void deinit_sommet(Sommet *s) {
    if (!s) return;

    if (s->type == TYPE_SWITCH) {
      if(s->objet.sw.tabCommutation != NULL){
        free(s->objet.sw.tabCommutation);
        s->objet.sw.tabCommutation = NULL;
      }
      if(s->objet.sw.etat_ports != NULL){
        free(s->objet.sw.etat_ports);
        s->objet.sw.etat_ports = NULL;
      }
    }
}

void init_trame(Trame *t) {
    memset(t, 0, sizeof(Trame));
}

void init_lien(Lien *l) {
  if (!l) return;

  l->s1 = NULL;
  l->s2 = NULL;
  l->poids = 0;
  l->port_s1 = 0;
  l->port_s2 = 0;
}

void deinit_lien(Lien *l) {
  if (!l) return;

  l->s1 = NULL;
  l->s2 = NULL;
}

void init_bpdu(BPDU *b) {
  if (!b) return;

  b->priorite_root = 0;
  memset(b->mac_root, 0, 6);
  b->cout_chemin = 0;
  memset(b->mac_emetteur, 0, 6);
  b->port_emetteur = 0;
}

void print_mac(const MAC mac[6]) {
    for (int i = 0; i < 6; i++) {
        printf("%02X", mac[i]);
        if (i < 5) printf(":");
    }
}

void print_ip(const IP ip[4]) {
    for (int i = 0; i < 4; i++) {
        printf("%d", ip[i]);
        if (i < 3) printf(".");
    }
}

void afficher_station(const Station *st) {
  if (!st) return;
  printf("Station \"%s\"\n", st->nom);
  printf("  MAC : "); print_mac(st->adrMAC); printf("\n");
  printf("  IP  : "); print_ip(st->adrIP); printf("\n");
}

void afficher_switch(const Switch *sw) {
    if (!sw) return;
    printf("Switch \"%s\"\n", sw->nom);
    printf("  MAC      : "); print_mac(sw->adrMAC); printf("\n");
    printf("  Nb ports : %zu\n", sw->nb_ports);
    printf("  Priorite : %u\n", sw->priorite);

    if (sw->tabCommutation && sw->nb_entrees > 0) {
        printf("  Table de commutation :\n");
        for (size_t i = 0; i < sw->nb_entrees; i++) {
            printf("   Entrée %zu : MAC = ", i);
            print_mac(sw->tabCommutation[i].adrMAC);
            printf(", port associé = %u\n", sw->tabCommutation[i].port);
        }
    } else {
        printf("  Table de commutation : vide\n");
    }
}

void afficher_sommet(const Sommet *s) {
    if (!s) return;

    switch (s->type) {
        case TYPE_STATION:
            afficher_station(&s->objet.station);
            break;
        case TYPE_SWITCH:
            afficher_switch(&s->objet.sw);
            break;
        default:
            printf("Sommet de type inconnu\n");
    }
}

void afficher_lien(const Lien *l) {
    if (!l) return;
    printf("Lien entre :\n");
    if (l->s1) {
        printf("  - ");
        afficher_sommet(l->s1);
    } else {
        printf("  - sommet 1 NULL\n");
    }
    if (l->s2) {
        printf("  - ");
        afficher_sommet(l->s2);
    } else {
        printf("  - sommet 2 NULL\n");
    }
    printf("  Ports : %u <-> %u\n", l->port_s1, l->port_s2);
    printf("  Poids : %u\n", l->poids);
}

void afficher_reseau(const Reseau *r) {
    if (!r) return;

    printf("Réseau : %zu sommets, %zu liens\n", r->nb_sommets, r->nb_liens);

    printf("Sommets :\n");
    for (size_t i = 0; i < r->nb_sommets; i++) {
        printf("Sommet %zu :\n", i);
        afficher_sommet(&r->sommets[i]);
        printf("\n");
    }

    printf("Liens :\n");
    for (size_t i = 0; i < r->nb_liens; i++) {
        printf("Lien %zu :\n", i);
        afficher_lien(&r->liens[i]);
        printf("\n");
    }
}

void afficher_trame_user(const Trame *t) {
    if (!t) return;
    printf("Préambule : ");
    for (int i = 0; i < 7; i++) {
        printf("%02X ", t->preambule[i]);
    }
    printf("\n");

    printf("SFD        : %02X\n", t->sfd);

    printf("MAC Dest.  : ");
    print_mac(t->destination);
    printf("\n");

    printf("MAC Source : ");
    print_mac(t->source);
    printf("\n");

    printf("Type       : 0x%04X\n", t->type);

    printf("Données    : ");
    for (int i = 0; i < 46; i++) {
        printf("%02X ", t->donnees[i]);
    }
    printf("\n");

    printf("FCS        : %08X\n", t->fcs);
}


void afficher_trame(const Trame *t) {
    if (!t) return;
    const uint8_t *octets = (const uint8_t *)t;
    size_t taille = sizeof(Trame);

    for (size_t i = 0; i < taille; i++) {
        printf("%02X ", octets[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    printf("\n");
}