#ifndef _ABR
#define _ABR

#include <stdio.h>
#include <stdlib.h>

typedef unsigned int uint;
typedef unsigned long long int ullong;

struct ensemble{
  ullong key;
  uint index;
  struct ensemble* left;
  struct ensemble* right;
};
typedef struct ensemble* ensemble;

void ens_ajouter(ensemble*, ullong, uint);

ensemble ens_rechercher(ensemble, ullong);

void ens_free(ensemble);

#endif
