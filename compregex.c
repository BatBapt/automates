#include "compregex.h"
/*
  L'algo scanner prend une liste de liste en entree vide
  et écrit dedans grace a la fonction ajouter.
  On passe en parametre une chaine qui va au fur est a mesure
  de l'algo passer par plusieurs boucles pour determiner
  si dans la chaine à un indice i on a un char,une parenthese
  ouvrante ou fermante,une union,une concatenation ou alors
  un kleene.
*/
void scanner(list** my_list, char* str){
  //On parcours la liste chaine
  for (int i = 0; i < strlen(str); i++){
    //initialisation d'une liste temporaire pour ensuite la passer dans la liste
    //de liste passe en parametre de la fonction.
    list* unit_lex = malloc(sizeof(list));
    if (unit_lex == NULL){
      printf("Erreur allocation\n");
      exit(-1);
    }
    if (isalpha(str[i]) != 0){
      unit_lex->type = "CHAR";
      unit_lex->car = str[i];
    }else if (str[i] == '.'){
      unit_lex->type = "CONCAT";
      unit_lex->car = str[i];
    }else if (str[i] == '*'){
      unit_lex->type = "KLEENE";
      unit_lex->car = str[i];
    }else if (str[i] == '+'){
      unit_lex->type = "UNION";
      unit_lex->car = str[i];
    }else if (str[i] == '('){
      unit_lex->type = "PAR_OUV";
      unit_lex->car = str[i];
    }else if (str[i] == ')'){
      unit_lex->type = "PAR_FER";
      unit_lex->car = str[i];
    }
    ajouter(my_list, unit_lex->type, unit_lex->car);
    free(unit_lex);
  }

}

/*
Cette fonctions sert à ajouter dans une liste à la fin, les paramètre qu'on lui passe
*/
void ajouter(list** head, char* ptype, char pcar){
  if (*head == NULL){
    *head = calloc(1, sizeof(list));
    (*head)->type = ptype;
    (*head)->car = pcar;
    (*head)->svt = NULL;

    return;
  }
  list* tmp = *head;

  while (tmp->svt != NULL){
    tmp = tmp->svt;
  }

  list* new = malloc(sizeof(list));

  if (!new){
    printf("Erreur malloc in ajouter.\n");
    exit(-1);
  }
  new->type = ptype;
  new->car = pcar;
  new->svt = NULL;

  tmp->svt = new;

}

//Fonction qui affiche une liste
void view(list* my_list){
  while (my_list != NULL){
    printf("%s %c\n", my_list->type, my_list->car);
    my_list = my_list->svt;
  }
}


/*-------------------------------------------

Exp -> Terme Reste1
Reste1 -> + Terme Reste1 | e
Terme -> Facteur Reste2
Reste2 -> . Facteur Reste2 | e
Facteur -> Facteur2* | Facteur2
Facteur2 -> Char | (Exp)
*/

list* postfix = NULL;

list* new_post = NULL;
/*
On initialise une variable id_parenth à 0 qui va s'incrementer
lorsqu'une parenthese s'ouvre et se decrementer lorsqu'on referme
une parenthese.
*/

int expr(){
  if (terme()){
    if (reste1()){
      return 1;
    }
  }
  return 0;
}


int reste1(){
  if (postfix->svt != NULL){
    if (postfix->type == "UNION"){
      if (postfix->car == '+'){
        postfix=postfix->svt;
        if (terme()){
          ajouter( &new_post , "UNION" , '+' );
          return reste1();
        }
      }else{
        return 1;
      }
    }else{
      return 1;
    }
  }else{
    return 1;
  }
}

int terme(){
  if (facteur()){
    if (reste2()){
      return 1;
    }
  }else{
    return 1;
  }
}

int reste2(){
    if (postfix->type == "CONCAT"){
      if (postfix->car == '.'){
        if (postfix->svt != NULL) postfix=postfix->svt;

        if (facteur()){
          ajouter( &new_post , "CONCAT" , '.' );
          return reste2();
        }
      } else{
        return 1;
      }
    } else{
      return 1;
    }
}


int facteur(){
  if (facteur2()){
    if ( postfix->type == "KLEENE"){
      if ( postfix->car == '*'){
        ajouter( &new_post , "KLEENE" , '*' );
        if (postfix->svt != NULL)postfix=postfix->svt;
        return 1;
      }
    } else {
      return 1;
    }
  } else {
    return 1;
  }
}

int facteur2(){
  if (postfix->type == "PAR_OUV" ){
    //on ouvre une parenthese donc on incremente id_parenth de 1
    postfix = postfix->svt;
    if (expr()){
      if (postfix->svt != NULL ){
        if (postfix->type == "PAR_FER"){
          ///ici on referme une parenthese donc on decremente id_parenthse de 1
          postfix = postfix->svt;
          return 1;
        } else{
          ///si on a pas de parenthèse fermante apres une parenthese ouvrante le programme affiche un message d'erreur et exit
          printf("Il n'y a pas de parenthese.\n");
          exit(-1);
        }
      } else{
        return 1;
      }
    } else{
      return 1;
    }
  } else if (postfix->type == "CHAR"){
    ajouter(&new_post, "CHAR", postfix->car );
    if (postfix->svt != NULL) postfix = postfix->svt;
    return 1;
  }
}

list* parser(list* unit_lex){
  postfix = unit_lex;
  if (expr()){
    return new_post;
  } else{
    printf("Erreur dans la regex.\n");
    exit(-1);
  }
}

/*---------------------------------------- */
/* Codegen */

void empiler(pile* p, int i){
  pile tmp;
  tmp = malloc(sizeof(pile));
  tmp->index = i;
  tmp->svt = *p;
  *p = tmp;
}

int depiler(pile* p){
  if (*p == NULL){
    printf("Erreur pile vide.\n");
    exit(-1);
  }
  int res;
  pile tmp;
  tmp = *p;
  res = tmp->index;
  *p = tmp->svt;
  free(tmp);
  return res;
}

void freepile(pile* p)
{
  while(*p)
    depiler(p);
}

void codegen(list* my_postfix, char* str, int debug){
  /*
  Cette fonction va servir à créer un fichier compilo.c qui sera utilisé
  pour générer un AFD avec tout les AFN utilent pour son fonctionnement

  @arg1: liste chaine postfixé de l'expression
  @arg2: la chaine a tester
  @arg3: argument si on est en mode debug
  */

  pile* my_pile = malloc(sizeof(pile));

  char* fl = "compilo.c";
  FILE* file = NULL;
  file = fopen(fl, "w");

  if (file == NULL){
    printf("Erreur création fichier\n");
    exit(-1);
  }
  /*
  Ici on inclut les headers utils au fonctionnement du fichier
  */
  fputs("#include <stdio.h>\n", file);
  fputs("#include <stdlib.h>\n\n", file);
  fputs("#include \"afn.h\"\n", file);
  fputs("#include \"afd.h\"\n", file);
  fputs("#include \"compregex.h\"\n\n\n", file);
  fputs("int main(int argc, char* argv[]){\n", file);

  int i = 1; // Utilisé pour la pile

  while (my_postfix != NULL){

    if (my_postfix->type == "CHAR"){
      /*
        Dans cette partie on traitre la création d'un automate
      */
      fprintf(file, "\tafn automate_%d;\n", i); // On crée un automate
      fprintf(file, "\tafn_char(&automate_%d, '%c');\n", i, my_postfix->car);
      empiler(my_pile, i); // On empile l'indice du nouvel automate pour plus tard
      i = i + 1;

      fputs("\n", file);

    }else if (my_postfix->type == "UNION"){
      /*
        Dans cette partir on gère l'union entre 2 automates dont le resultat
        se fera dans un nouvel automate crée
      */

      int tmp1 = depiler(my_pile); // On dépile pour avoir accès au dernier automate crée
      int tmp2 = depiler(my_pile);

      fprintf(file, "\tafn automate_%d;\n", i);
      fprintf(file, "\tafn_union(&automate_%d, automate_%d, automate_%d);\n", i, tmp2, tmp1);
      if (debug){
        fprintf(file, "\tprintf(\"Union des automates %d et %d: \");\n", tmp2, tmp1);
        fprintf(file, "\tafn_print(automate_%d);\n", i);
      }

      fputs("\t//On free les automates qui sont utilisés\n", file);
      fprintf(file, "\tafn_free(&automate_%d);\n", tmp2);
      fprintf(file, "\tafn_free(&automate_%d);\n", tmp1);
      empiler(my_pile, i);
      i = i + 1;

      fputs("\n", file);

    }else if (my_postfix->type == "CONCAT"){
      /*
        Dans cette partir on gère la concaténation entre 2 automates dont le resultat
        se fera dans un nouvel automate crée
      */
      int tmp1 = depiler(my_pile);
      int tmp2 = depiler(my_pile);


      fprintf(file, "\tafn automate_%d;\n", i);
      fprintf(file, "\tafn_concat(&automate_%d, automate_%d, automate_%d);\n", i, tmp2, tmp1);
      if (debug){
        fprintf(file, "\tprintf(\"Concatenation des automates %d et %d: \");\n", tmp2, tmp1);
        fprintf(file, "\tafn_print(automate_%d);\n", i);
      }

      fputs("\t//On free les automates qui sont utilisés\n", file);
      fprintf(file, "\tafn_free(&automate_%d);\n", tmp2);
      fprintf(file, "\tafn_free(&automate_%d);\n", tmp1);
      empiler(my_pile, i);
      i = i + 1;

      fputs("\n", file);

    }else if (my_postfix->type == "KLEENE"){
      /*
        Dans cette partir on gère la fermeture de kleene d'un automate dont le resultat
        se fera dans un nouvel automate crée
      */
      int tmp1 = depiler(my_pile);


      fprintf(file, "\tafn automate_%d;\n", i);
      fprintf(file, "\tafn_kleene(&automate_%d, automate_%d);\n", i, tmp1);
      if (debug){
        fprintf(file, "\tprintf(\"Fermeture de Kleene de l'automate %d: \");\n", tmp1);
        fprintf(file, "\tafn_print(automate_%d);\n", i);
      }

      fputs("\t//On free l'automate qui est utilisé\n", file);
      fprintf(file, "\tafn_free(&automate_%d);\n", tmp1);
      empiler(my_pile, i);
      i = i + 1;

      fputs("\n", file);
    }
    my_postfix = my_postfix->svt;
  }
  /*
  Ici on va créer l'AFD
  */
  int tmp_i = depiler(my_pile);
  fputs("\tafd A;\n", file);
  fprintf(file, "\tafn_determinisation(automate_%d, &A);\n", tmp_i);
  fprintf(file, "\tafd_simul(\"%s\", A);\n", str);

  fputs("\t//On libère l'espace mémoire de l'AFD\n", file);
  fputs("\tafd_free(&A);\n", file);

  fputs("}\n", file);


  fclose(file);
}
