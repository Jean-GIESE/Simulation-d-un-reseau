#include <stdio.h>
#include "reseau.h"

int main() {
    Reseau reseau;

    // Initialisation
    init_reseau(&reseau);

    // Allouer 2 sommets et 1 lien
    if (allouer_reseau(&reseau, 2, 1) != 0) {
        fprintf(stderr, "Erreur allocation reseau\n");
        return 1;
    }

    // Initialiser les sommets
    reseau.sommets[0].type = TYPE_STATION;
    init_sommet(&reseau.sommets[0]);

    reseau.sommets[1].type = TYPE_SWITCH;
    init_sommet(&reseau.sommets[1]);

    // Initialiser un lien entre sommet 0 et 1 (pointeurs vers les sommets)
    reseau.liens[0].s1 = &reseau.sommets[0];
    reseau.liens[0].s2 = &reseau.sommets[1];

    // Affichage complet
    printf("Reseau:\n");
    afficher_reseau(&reseau);

    // Libération mémoire
    deinit_reseau(&reseau);

    return 0;
}
