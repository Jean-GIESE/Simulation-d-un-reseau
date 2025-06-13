#include <stdio.h>
#include "reseau.h" 
#include "algos.h"

int main()
{    
  //Reseau reseau;
  //creer_reseau("/home/legerh/Base/kk//mylan", &reseau);
  //afficher_reseau(&reseau);
Reseau r;
    // 5 sommets : 3 stations + 2 switches, 5 liens (A-S1, S1-S2, S2-B, S2-C)
    allouer_reseau(&r, 5, 5);

    // Stations
    Sommet *sta1 = &r.sommets[0];
    sta1->type = TYPE_STATION;
    strcpy(sta1->objet.station.nom, "StationA");
    MAC macA[6] = {0x00,0x11,0x22,0xAA,0xAA,0xAA};
    memcpy(sta1->objet.station.adrMAC, macA, 6);

    Sommet *sta2 = &r.sommets[1];
    sta2->type = TYPE_STATION;
    strcpy(sta2->objet.station.nom, "StationB");
    MAC macB[6] = {0x00,0x11,0x22,0xBB,0xBB,0xBB};
    memcpy(sta2->objet.station.adrMAC, macB, 6);

    Sommet *sta3 = &r.sommets[2];
    sta3->type = TYPE_STATION;
    strcpy(sta3->objet.station.nom, "StationC");
    MAC macC[6] = {0x00,0x11,0x22,0xCC,0xCC,0xCC};
    memcpy(sta3->objet.station.adrMAC, macC, 6);

    // Switch 1
    Sommet *sw1 = &r.sommets[3];
    sw1->type = TYPE_SWITCH;
    strcpy(sw1->objet.sw.nom, "Switch1");
    sw1->objet.sw.nb_ports = 2; // connecté à StationA + Switch2
    sw1->objet.sw.priorite = 1;
    sw1->objet.sw.etat_ports = malloc(sizeof(EtatPort) * 2);
    sw1->objet.sw.tabCommutation = malloc(sizeof(Commutation) * 10);
    sw1->objet.sw.nb_entrees = 0;
    sw1->objet.sw.capacite = 10;
    for (size_t i = 0; i < 2; i++) {
        sw1->objet.sw.etat_ports[i] = ACTIF;
    }

    // Switch 2
    Sommet *sw2 = &r.sommets[4];
    sw2->type = TYPE_SWITCH;
    strcpy(sw2->objet.sw.nom, "Switch2");
    sw2->objet.sw.nb_ports = 3; // connecté à Switch1 + StationB + StationC
    sw2->objet.sw.priorite = 1;
    sw2->objet.sw.etat_ports = malloc(sizeof(EtatPort) * 3);
    sw2->objet.sw.tabCommutation = malloc(sizeof(Commutation) * 10);
    sw2->objet.sw.nb_entrees = 0;
    sw2->objet.sw.capacite = 10;
    for (size_t i = 0; i < 3; i++) {
        sw2->objet.sw.etat_ports[i] = ACTIF;
    }

    // Liens :
    // StationA port 0 <-> Switch1 port 0
    r.liens[0].s1 = sta1; r.liens[0].port_s1 = 0;
    r.liens[0].s2 = sw1;  r.liens[0].port_s2 = 0;

    // Switch1 port 1 <-> Switch2 port 0
    r.liens[1].s1 = sw1;  r.liens[1].port_s1 = 1;
    r.liens[1].s2 = sw2;  r.liens[1].port_s2 = 0;

    // Switch2 port 1 <-> StationB port 0
    r.liens[2].s1 = sw2;  r.liens[2].port_s1 = 1;
    r.liens[2].s2 = sta2; r.liens[2].port_s2 = 0;

    // Switch2 port 2 <-> StationC port 0
    r.liens[3].s1 = sw2;  r.liens[3].port_s1 = 2;
    r.liens[3].s2 = sta3; r.liens[3].port_s2 = 0;

    r.nb_sommets = 5;
    r.nb_liens = 4;

    // Création de la trame : StationA -> StationB
    Trame t;
    init_trame(&t);
    memcpy(t.source, macA, 6);
    memcpy(t.destination, macB, 6);
    strcpy((char*)t.donnees, "Hello StationB!");

    printf("=== Début de la simulation ===\n");
    envoyer_trame(&r, 0, &t);
    printf("=== Fin de la simulation ===\n");
    envoyer_trame(&r, 0, &t);
    // Libération mémoire

    deinit_reseau(&r);

    return 0;
}
