// main.c
#include <stdio.h>
#include "reseau.h"

int main()
{
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

  // Initialiser un lien entre sommet 0 et 1
  reseau.liens[0].s1 = reseau.sommets[0];
  reseau.liens[0].s2 = reseau.sommets[1];

  // Affichage simple
  printf("Reseau:\n");
  printf("- Sommets: %zu\n", reseau.nb_sommets);
  printf("  Sommet 0: %s\n", reseau.sommets[0].objet.station.nom);
  printf("  Sommet 1: %s\n", reseau.sommets[1].objet.sw.nom);
  printf("- Liens: %zu\n", reseau.nb_liens);

  deinit_reseau(&reseau);

  return 0;
}
