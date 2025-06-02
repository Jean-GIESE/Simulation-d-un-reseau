#include <stdio.h>
#include <stdlib.h>

#include "reseau.h"

// void afficher_Switch(Switch sw)
// {
//     
// }

void creer_Reseaux(char* nomFichier)
{
    FICHIER * fichier = fopen(nomFichier,"r");
    
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

// void init_sommet(Sommet *s)
// {
//     s->type = TYPE_STATION;
//     s->objet = malloc(sizeof(objet));
// }
// 
// void deinit_sommet(Sommet *s)
// {
//     free(s->objet);
//     s->objet=NULL;
//     s->type=TYPE_STATION;
// }
