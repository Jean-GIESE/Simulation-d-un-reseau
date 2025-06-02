#include "reseau.h"

void init_sommet(Sommet *s)
{
    s->type = TYPE_STATION;
    s->objet = malloc(sizeof(objet));
}

void deinit_sommet(Sommet *s)
{
    free(s->objet);
    s->objet=NULL;
    s->type=TYPE_STATION;
}