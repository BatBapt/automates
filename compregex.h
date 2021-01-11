#ifndef COMPREGEX_H
#define COMPREGEX_H

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <ctype.h>

#include "afn.h"
#include "afd.h"

typedef struct list{
  char* type;
  char car;
  struct list* svt;
}list;

typedef struct _pile_{
    int index;
    struct _pile_* svt;
}enrpile, *pile;

void view(list* my_list);
void scanner(list** my_list,char *str);
void ajouter(list** head, char* ptype, char pcar);

list* parser(list*);

void debug(char*);

int reste1();

int reconnaitre(char);

int expr();

int facteur();

int reconnaitre(char);

int facteur2();

int reste2();

int terme();

int expr();

void empiler(pile*, int);

int depiler(pile*);

void freepile(pile*);

void codegen(list*, char*, int);
#endif
