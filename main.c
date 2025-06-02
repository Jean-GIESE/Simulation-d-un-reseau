// main.c
#include <stdio.h>
#include "reseau.c"

int main()
{
    printf("uwu\n");
    Sommet s;
    s.type = TYPE_STATION;
    s.objet.station = (Station){ .nom = "PC1", .adrMAC = {0,1,2,3,4,5}, .adrIP = {192, 168, 0, 1} };

    if (s.type == TYPE_STATION) {
        printf("Nom de la station : %s\n", s.objet.station.nom);
    }

    return 0;
}
