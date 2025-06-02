#include <stdio.h>
#include "reseau.h"

int main()
{
//   === Test station ===
//   Sommet station;
//   station.type = TYPE_STATION;
//   init_sommet(&station);
// 
//   printf("== Station ==\n");
//   printf("Nom: %s\n", station.objet.station.nom);
//   printf("MAC: "); print_mac(station.objet.station.adrMAC); printf("\n");
//   printf("IP : "); print_ip(station.objet.station.adrIP); printf("\n");
// 
//   === Test switch ===
//   Sommet sw;
//   sw.type = TYPE_SWITCH;
//   init_sommet(&sw);
// 
//   printf("\n== Switch ==\n");
//   printf("Nom: %s\n", sw.objet.sw.nom);
//   printf("MAC: "); print_mac(sw.objet.sw.adrMAC); printf("\n");
//   printf("Ports: %zu\n", sw.objet.sw.nb_ports);
//   printf("Priorité: %hu\n", sw.objet.sw.priorite);
// 
//   for (size_t i = 0; i < sw.objet.sw.nb_ports; i++) {
//       printf("Commutation[%zu] → Port: %u, MAC: ", i, sw.objet.sw.tabCommutation[i].port);
//       print_mac(sw.objet.sw.tabCommutation[i].adrMAC);
//       printf("\n");
//   }
// 
//   Libération mémoire
//   deinit_sommet(&sw);
    
    
    
//     creer_reseau("/adhome/j/jg/jgiese/S21/SAE/kk/mylan", );
    return 0;
}
// int main() {
//     Reseau reseau;
// 
//     Initialisation
//     init_reseau(&reseau);
// 
//     Allouer 2 sommets et 1 lien
//     if (allouer_reseau(&reseau, 2, 1) != 0) {
//         fprintf(stderr, "Erreur allocation reseau\n");
//         return 1;
//     }
// 
//     Initialiser les sommets
//     reseau.sommets[0].type = TYPE_STATION;
//     init_sommet(&reseau.sommets[0]);
// 
//     reseau.sommets[1].type = TYPE_SWITCH;
//     init_sommet(&reseau.sommets[1]);
// 
//     Initialiser un lien entre sommet 0 et 1 (pointeurs vers les sommets)
//     reseau.liens[0].s1 = &reseau.sommets[0];
//     reseau.liens[0].s2 = &reseau.sommets[1];
// 
//     Affichage complet
//     printf("Reseau:\n");
//     afficher_reseau(&reseau);
// 
//     Libération mémoire
//     deinit_reseau(&reseau);
// 
//     return 0;
// }
