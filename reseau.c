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
    FILE * fichier = fopen(nomFichier,"r");
    
    char ligne[128];
    
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
        
        fgets(ligne, sizeof(ligne), fichier);
        int numSwitch = 1;
        int numSt = 1;
        for (int i=0; i<nbEquipements; i++)
        {
            if (fgets(ligne, sizeof(ligne), fichier) == NULL) {
                fprintf(stderr, "Erreur de lecture de la deuxième ligne\n");
                fclose(fichier);
                return;
            }
            //printf("Ligne brute (équipement %d): %s\n", i, ligne);

            size_t type, nbPorts, priorite;
            char adrMACChar[32];

            // Découper la ligne avec strtok
            char *token = strtok(ligne, ";");
            if (token) type = atoi(token);
            else {
                fprintf(stderr, "Erreur de lecture d'une ligne1\n");
                fclose(fichier);
                return;
            }
            //printf("Champ type: %s\n", token);
            
            MAC mac[6];
            char *token_ctx = NULL; // contexte pour strtok_r

            // Lire l'adresse MAC (token 2)
            token = strtok(NULL, ";");
            if (token) {
                strncpy(adrMACChar, token, sizeof(adrMACChar) - 1);
                adrMACChar[sizeof(adrMACChar) - 1] = '\0';
                
                // Faire une copie pour découper sans casser strtok global
                char adrMACCopy[32];
                strncpy(adrMACCopy, adrMACChar, sizeof(adrMACCopy) - 1);
                adrMACCopy[sizeof(adrMACCopy) - 1] = '\0';

                char *tokenMAC = strtok_r(adrMACCopy, ":", &token_ctx);
                int j = 0;
                while (tokenMAC != NULL && j < 6) {
                    mac[j++] = (uint8_t)strtol(tokenMAC, NULL, 16);
                    tokenMAC = strtok_r(NULL, ":", &token_ctx);
                }
            } else {
                fprintf(stderr, "Erreur de lecture d'une ligne2\n");
                fclose(fichier);
                return;
            }
            //printf("Champ adrMAC: %s\n", adrMACChar);
            
            /*token = strtok(NULL, ";");
            if (token) strncpy(adrMACChar, token, sizeof(adrMACChar)-1);
            else {
                fprintf(stderr, "Erreur de lecture d'une ligne2\n");
                fclose(fichier);
                return;
            }
            
            MAC mac[6];
            char *tokenMAC = strtok(adrMACChar, ":");
            int j = 0;

            while (tokenMAC != NULL && j < 6) {
                mac[j] = (uint8_t)strtol(tokenMAC, NULL, 16);  // Conversion base 16
                tokenMAC = strtok(NULL, ":");
                j++;
            }*/
            
            if (type == 2) {
                reseau->sommets[i].type = TYPE_SWITCH;
                init_sommet(&reseau->sommets[i]);
      
                for (int k=0; k<6; k++) { reseau->sommets[i].objet.sw.adrMAC[k] = mac[k]; }
                
                token = strtok(NULL, ";");
                //printf("Champ nbPorts: %s\n", token);
                if (token) nbPorts = atoi(token);
                else {
                    fprintf(stderr, "Erreur de lecture d'une ligne3\n");
                    fclose(fichier);
                    return;
                }

                token = strtok(NULL, ";");
                //printf("Champ priorite: %s\n", token);
                if (token) priorite = atoi(token);
                else {
                    fprintf(stderr, "Erreur de lecture d'une ligne4\n");
                    fclose(fichier);
                    return;
                }
                
                char nomSW[32];
                sprintf(nomSW, "sw%d", numSwitch++);
                
                reseau->sommets[i].objet.sw.nb_ports = nbPorts;
                reseau->sommets[i].objet.sw.priorite = priorite;
                strcpy(reseau->sommets[i].objet.sw.nom, nomSW);
                
                reseau->sommets[i].objet.sw.tabCommutation = realloc(reseau->sommets[i].objet.sw.tabCommutation, sizeof(Commutation) * reseau->sommets[i].objet.sw.nb_ports);
                reseau->sommets[i].objet.sw.etat_ports = malloc(nbPorts * sizeof(EtatPort));
                if ((reseau->sommets[i].objet.sw.tabCommutation != NULL) && (reseau->sommets[i].objet.sw.etat_ports != NULL)) {
                    for (size_t j = 0; j < nbPorts; j++) {
                        memset(reseau->sommets[i].objet.sw.tabCommutation[j].adrMAC, 0, 6);
                        reseau->sommets[i].objet.sw.tabCommutation[j].port = 0;
                        reseau->sommets[i].objet.sw.etat_ports[j] = BLOQUE;
                    }
                }
                reseau->sommets[i].objet.sw.capacite = nbPorts;
                reseau->sommets[i].objet.sw.nb_entrees = 0;
            }
            
            if (type == 1) {
                reseau->sommets[i].type = TYPE_STATION;
                init_sommet(&reseau->sommets[i]);
                
                for (int k=0; k<6; k++) { reseau->sommets[i].objet.station.adrMAC[k] = mac[k]; }
                
                char adrIPChar[32];
                token = strtok(NULL, ";");
                if (token) strncpy(adrIPChar, token, sizeof(adrIPChar)-1);
                else {
                    fprintf(stderr, "Erreur de lecture d'une ligne5\n");
                    fclose(fichier);
                    return;
                }
                //printf("Champ adrIP: %s\n", adrIPChar);
                
                IP ip[4];
                char *tokenIP = strtok(adrIPChar, ".");
                int j = 0;
                while (tokenIP != NULL && j < 4) {
                    ip[j] = (uint8_t)strtol(tokenIP, NULL, 10);  // Conversion base 10
                    tokenIP = strtok(NULL, ".");
                    j++;
                }
                for (int k=0; k<4; k++) { reseau->sommets[i].objet.station.adrIP[k] = ip[k]; }
                
                char nomSt[32];
                sprintf(nomSt, "st%d", numSt++);
                strcpy(reseau->sommets[i].objet.station.nom, nomSt);
            }
        }
        
        size_t i=0;
        while (fgets(ligne, sizeof(ligne), fichier)) {
            size_t numS1, numS2, poidsLien;
            
            char *token = strtok(ligne, ";");
            if (token) numS1 = atoi(token);
            else {
                fprintf(stderr, "Erreur de lecture d'une ligne6\n");
                fclose(fichier);
                return;
            }
            //printf("Champ s1: %s\n", token);
            
            token = strtok(NULL, ";");
            if (token) numS2 = atoi(token);
            else {
                fprintf(stderr, "Erreur de lecture d'une ligne7\n");
                fclose(fichier);
                return;
            }
            //printf("Champ s2: %s\n", token);
            
            token = strtok(NULL, ";");
            if (token) poidsLien = atoi(token);
            else {
                fprintf(stderr, "Erreur de lecture d'une ligne8\n");
                fclose(fichier);
                return;
            }
            //printf("Champ poids: %s\n", token);
            
            reseau->liens[i].s1 = &reseau->sommets[numS1];
            reseau->liens[i].s2 = &reseau->sommets[numS2];
            reseau->liens[i].poids = poidsLien;
            
            if (reseau->liens[i].s1->type == TYPE_SWITCH) {
                Switch *sw1 = &reseau->liens[i].s1->objet.sw;
                if (sw1->ports_utilises < sw1->nb_ports) {
                    reseau->liens[i].port_s1 = sw1->ports_utilises++;
                } else {
                    fprintf(stderr, "Plus de ports disponibles sur %s\n", sw1->nom);
                    exit(1);
                }
            } else {
                reseau->liens[i].port_s1 = 0; // station, pas de port
            }

            if (reseau->liens[i].s2->type == TYPE_SWITCH) {
                Switch *sw2 = &reseau->liens[i].s2->objet.sw;
                if (sw2->ports_utilises < sw2->nb_ports) {
                    reseau->liens[i].port_s2 = sw2->ports_utilises++;
                } else {
                    fprintf(stderr, "Plus de ports disponibles sur %s\n", sw2->nom);
                    exit(1);
                }
            } else {
                reseau->liens[i].port_s2 = 0;
            }

            
            i++;
        }
        
        fclose(fichier);
    }
    else {
        fprintf(stderr, "Impossible d'ouvrir le fichier!\n");
    }

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
