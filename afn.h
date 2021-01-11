#ifndef AFN_H
#define AFN_H

#include <stdlib.h>
#include <stdio.h>
#include "afd.h"
#include "abr.h"


#define INT_ETAT(q) (1ULL<< (q))
#define IN(q,X) ((INT_ETAT(q) & X ) > 0)

typedef unsigned int uint;
typedef unsigned long long int ullong;

extern char ALL_ALPHA[64];


typedef struct{
  uint nbetat, nbsymb;
  char * alphabet;
  uint tsymb[SYMB_NB_MAX];
  ullong init, finals;
  ullong **delta;
} afn;

void afn_init(afn *A, uint nbetat, char * alphabet, ullong init, ullong finals);
void afn_add_trans(afn *A, uint q1, uint s, uint q2);
void afn_free(afn *A);

void afn_print(afn A);
void afn_finit(afn *A, char *f);

ullong afn_epsilon_fermeture(afn A, ullong R);
void afn_determinisation(afn A, afd *D);

void afn_char(afn*, char);
void afn_union(afn*, afn, afn);
void afn_concat(afn*, afn, afn);
void afn_kleene(afn*, afn);

#endif
