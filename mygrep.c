#include "compregex.h"


int main(int argc, char* argv[]){

  if (argc < 4){
    printf("Erreur: arguments inexistant.\n");
    printf("Usage: ./mygrep <exreg> <chaine> <debug_mode: d/n>\n");
    printf("Exemple: ./mygrep '(a+b)*.c' 'acccccb' 'd'\n\n");

    printf("Puis pour lancer le programme final, il faut executer les commandes suivantes (dans l'ordre):\n");
    printf("make compilo\n");
    printf("./compilo\n");
    exit(-1);
  }
  char* regex = argv[1];
  char* chaine = argv[2];
  char debug_car = *argv[3];

  int debug_mode = 0;
  if (debug_car == 'd')
    debug_mode = 1;

  list* my_list = NULL;
  list* postfix = NULL;

  scanner(&my_list, regex);
  if (debug_mode){
    printf("Liste d'unité lexicale: \n");
    view(my_list);
    printf("\n");
  }

  postfix = parser(my_list);
  if (debug_mode){
    printf("Liste postfixé: \n");
    view(postfix);
    printf("\n");
  }

  codegen(postfix, chaine, debug_mode);
}
