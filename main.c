// main.c
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
    
    creer_Reseaux("/adhome/j/jg/jgiese/S21/SAE/kk/mylan");
  return 0;
}
