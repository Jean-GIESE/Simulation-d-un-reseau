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


void creer_reseau(char* nomFichier, Reseau *reseau)
{
    FILE * fichier = fopen(nomFichier,"r");
    
    char ligne[32];
    
    if (fichier != NULL) {
        int nbEquipements, nbLiens;
        if (fscanf(fichier, "%d %d", &nbEquipements, &nbLiens) != 2) {
            fprintf(stderr, "Erreur de lecture des deux entiers\n");
            fclose(fichier);
            return;
        }
        
        init_reseau(reseau);
        if (allouer_reseau(reseau, nbEquipements, nbLiens) != 0)
        {
            fprintf(stderr, "Erreur allocation reseau!\n");
            fclose(fichier);
            return;
        }
        
        for (int i=0; i<nbEquipements; i++)
        {
            if (fgets(ligne, sizeof(ligne), fichier) == NULL) {
                fprintf(stderr, "Erreur de lecture de la deuxième ligne\n");
                fclose(fichier);
                return 1;
            }

            int type, nbPorts, priorite;
            char adrMAC[32];

            // Découper la ligne avec strtok
            char *token = strtok(ligne, ";");
            if (token) type = atoi(token);
            
            token = strtok(NULL, ";");
            if (token) strncpy(adrMAC, token, sizeof(adrMAC));
            
            if (type == 2) {
                reseau->sommets[i].type = TYPE_SWITCH;
                init_sommet(&reseau->sommets[i]);
                
                token = strtok(NULL, ";");
                if (token) nbPorts = atoi(token);

                token = strtok(NULL, ";");
                if (token) priorite = atoi(token);
                
                char *nomSW = "sw ";
                nomSW[3] = i;
                
                reseau->sommets->objet.sw.nb_ports = nbPorts;
                reseau->sommets->objet.sw.priorite = priorite;
                strcpy(reseau->sommets->objet.sw.nom, nomSW);
            }
            else {
                reseau->sommets[i].type = TYPE_STATION;
                init_sommet(&reseau->sommets[i]);
            }
        }
        
        while (fgets(ligne, sizeof(ligne), fichier)) {
            printf("%s", ligne);
        }
        
        deinit_reseau(reseau);
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
    printf("  Priorite : %d\n", sw->priorite);

    if (sw->tabCommutation) {
        printf("  Table de commutation :\n");
        for (size_t i = 0; i < sw->nb_ports; i++) {
            printf("   Port %zu : MAC = ", i);
            print_mac(sw->tabCommutation[i].adrMAC);
            printf(", port associé = %d\n", sw->tabCommutation[i].port);
        }
    } else {
        printf("  Table de commutation : NULL\n");
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
