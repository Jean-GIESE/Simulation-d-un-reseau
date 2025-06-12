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
    
    
    Reseau reseau;
    creer_reseau("/adhome/j/jg/jgiese/S21/SAE/kk/mylan_nocycle", &reseau);
    afficher_reseau(&reseau);

    Trame t;
    
    init_trame(&t); // Trame vide, propre, avec 0 partout

    afficher_trame_user(&t); // Affiche des zéros partout
    afficher_trame(&t);      // Affichage brut : que des 00
    
    deinit_reseau(&reseau);

    return 0;
}
