#include <stdio.h>
#include "reseau.h" 
#include "algos.h"

int main()
{    
  Reseau reseau;
  creer_reseau("/home/legerh/Base/kk//mylan", &reseau);

  afficher_reseau(&reseau);
  deinit_reseau(&reseau);

  return 0;
}
