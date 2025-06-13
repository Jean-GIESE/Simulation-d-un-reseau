#include <stdio.h>
#include "reseau.h" 
#include "algos.h"

#include <string.h>

int main()
{ 
  
  Reseau reseau;
  creer_reseau("/home/legerh/Base/kk//mylan_nocycle", &reseau);
  //stp(&reseau);
  
  afficher_reseau(&reseau);
  Trame t;
  creer_trame_depuis_noms(&reseau, "st1", "st8", &t);
  envoyer_trame(&reseau, trouver_station_par_nom(&reseau, "st1"), &t);
  //afficher_reseau(&reseau);
  deinit_reseau(&reseau);

/*
    Reseau reseau;

    // Allouer un réseau avec 2 sommets (station + switch) et 1 lien
    allouer_reseau(&reseau, 2, 1);

    // Initialisation station (id 0)
    Sommet *st = &reseau.sommets[0];
    st->type = TYPE_STATION;
    uint8_t mac_station[6] = {0x00,0x11,0x22,0x33,0x44,0x55};
    memcpy(st->objet.station.adrMAC, mac_station, 6);
    uint8_t ip_station[4] = {192,168,0,1};
    memcpy(st->objet.station.adrIP, ip_station, 4);
    strcpy(st->objet.station.nom, "Station0");

    // Initialisation switch (id 1)
    Sommet *sw = &reseau.sommets[1];
    sw->type = TYPE_SWITCH;
    Switch *sw_obj = &sw->objet.sw;
    strcpy(sw_obj->nom, "Switch0");
    uint8_t mac_switch[6] = {0xaa,0xbb,0xcc,0xdd,0xee,0xff};
    memcpy(sw_obj->adrMAC, mac_switch, 6);
    sw_obj->nb_ports = 2;
    sw_obj->priorite = 1;
    sw_obj->etat_ports = malloc(sizeof(EtatPort) * sw_obj->nb_ports);
    for (size_t i = 0; i < sw_obj->nb_ports; i++) {
        sw_obj->etat_ports[i] = ACTIF;
    }
    sw_obj->capacite = 4;
    sw_obj->nb_entrees = 0;
    sw_obj->tabCommutation = malloc(sizeof(Commutation) * sw_obj->capacite);
    sw_obj->ports_utilises = 0;

    // Initialisation du lien entre station (port 0) et switch (port 0)
    Lien *lien = &reseau.liens[0];
    lien->s1 = st;
    lien->s2 = sw;
    lien->port_s1 = 0; // port station (pas utilisé mais on met 0)
    lien->port_s2 = 0; // port switch 0

    // Création d’une trame simple
    Trame trame;
    memset(&trame, 0, sizeof(Trame));
    // Source = station MAC
    memcpy(trame.source, mac_station, 6);
    // Destination = MAC switch (juste pour tester)
    memcpy(trame.destination, mac_switch, 6);

    printf("Début du test d'envoi de trame\n");
    envoyer_trame(&reseau, 0, &trame);
    printf("Fin du test\n");

    // Libérations
    free(sw_obj->etat_ports);
    free(sw_obj->tabCommutation);
    free(reseau.sommets);
    free(reseau.liens);
*/
    return 0;
}
