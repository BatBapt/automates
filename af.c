/*
  I53 Compilation et théorie des langages
  Nicolas Méloni
  Nov -2020
 */
#include <stdio.h>
#include "afd.h"
#include "afn.h"
#include "abr.h"

int main(int argc, char *argv[]){

  afn A;
  afn E;

  afn_char(&A, 'a');
  afn_print(A);

  afn_kleene(&E, A);
  afn_print(E);

  /*
  afn_determinisation(A, &B);
  afd_print(B);
  return 0;


  afn A, B, C, D, E;

  afn_init(&A, 3,  ALL_ALPHA, 1, 6);
  afn_add_trans(&A, 1, 'd', 2);
  afn_add_trans(&A, 0, 'm', 1);
  printf("A:\n");
  afn_print(A);
  printf("\n");

  printf("B:\n");
  afn_char(&B, 'a');
  afn_print(B);
  printf("\n");

  printf("UNION de A et B:\n");
  afn_union(&C, A, B);
  afn_print(C);
  printf("\n");

  printf("CONCATENATION de A et B:\n");
  afn_concat(&D, A, B);
  afn_print(D);
  printf("\n");


  printf("KLEENE de Concat A et B:\n");
  afn_kleene(&E, D);
  afn_print(E);
  printf("\n");


  afn_free(&E);
  afn_free(&D);
  afn_free(&C);
  afn_free(&B);
  afn_free(&A);
  return 0;
  */
}
