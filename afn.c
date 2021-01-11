#include "afn.h"


void afn_init(afn *A, uint nbetat, char * alphabet, ullong init, ullong finals){
  A->nbetat = nbetat;

  A->alphabet = malloc(strlen(alphabet)+1);
  strcpy(A->alphabet, alphabet);
  A->nbsymb = strlen(alphabet);

  A->init = init;
  A->finals = finals;

  int i;
  int symb;

  for (i=0; i<SYMB_NB_MAX; i++)
    A->tsymb[i] = SYMB_NONE;

  for (i=0; i<A->nbsymb; i++){
    if ( (alphabet[i] < SYMB_ASCII_DEB) ||
	 (alphabet[i] > SYMB_ASCII_FIN) ){
      fprintf(stderr,"[afd_init] caractere ascii numero %d invalide\n", alphabet[i]);
      exit(-1);
    }
    symb = (int) (alphabet[i] - SYMB_ASCII_DEB);
    if ( A->tsymb[symb] != SYMB_NONE ){
      fprintf(stderr,"[afd_init] caractere <%c> deja defini (ignorer)\n",alphabet[i]);
    }
    else {
      A->tsymb[symb] = i;
    }
  }
  A->delta = calloc(nbetat, sizeof(ullong *));
  for (i=0; i<nbetat; i++){
    A->delta[i] = calloc(A->nbsymb, sizeof(ullong));
  }
}

  /*
  Ajoute la relation (<q1>, <s>, <q2>) a l'AFN <A>
*/
void afn_add_trans(afn *A, uint q1, uint s, uint q2){
  uint symb = A->tsymb[s-SYMB_ASCII_DEB];

  if (symb == SYMB_NONE){
    fprintf(stderr, "[add_trans] %u -- %c --> %u symbole inconnu\n", q1,s,q2);
    exit(-1);
  }

  if ( (q1<0) || (q1>=A->nbetat) ){
   fprintf(stderr, "[add_trans] etat <%d> non reconnu\n", q1);
    exit(-1);
  }
  if ( (q2<0) || (q2>=A->nbetat) ){
   fprintf(stderr, "[add_trans] etat <%d> non reconnu\n", q2);
    exit(-1);
  }

  A->delta[q1][symb] |= INT_ETAT(q2);
}


/*
  Libere la memoire de l'AFN <A>
*/
void afn_free(afn *A){
  free(A->alphabet);

  int i;
  for (i=0; i < A->nbetat; i++)
    free(A->delta[i]);
  free(A->delta);
}


/*
  Affiche l'AFN <A>
*/
void afn_print(afn A){
  uint q,q1,s;
  printf("\ninit:");
  for (q=0; q<A.nbetat; q++){
    if IN(q,A.init) printf(" %d",q);
  }
  printf("\n");
  printf("finals:");
  for (q=0; q<A.nbetat; q++){
    if IN(q,A.finals) printf(" %d",q);
  }
  printf("\n");
  for (q=0; q<A.nbetat; q++){
    for (s=0; s<A.nbsymb; s++){
      if (A.delta[q][s]!=0){
      	printf("%d -- %c --> {", q, A.alphabet[s]);
      	for (q1=0; q1<A.nbetat; q1++){
          if (IN(q1, A.delta[q][s])) printf("%d,",q1);
      	}
      	printf("\b}\n");
      }
    }
  }
}


/*
  Initialise l'AFN <A> a partir des donnees du fichier <nomfichier> au
  format:

  nb_etats alphabet
  etats_initiaux etats_finals
  etat_i_11 symb_j_1 etat_i_12
  etat_i_21 symb_j_2 etat_i_22
  .
  .
  .
  etat_i_n1 symb_j_n etat_i_n2

*/
void afn_finit(afn *A,  char * nomfichier){
  FILE* fd = fopen(nomfichier, "r");

  if (fd != NULL){
    uint nbetat, q1, q2;
    ullong init, finals;
    char alphabet[SYMB_NB_MAX], s;

    if (fscanf(fd, "%u %s", &nbetat, alphabet) != 2){
      printf("Erreur lecture fichier ligne 1\n");
      exit(1);
    }

    if (fscanf(fd, "%llu %llu", &init, &finals) != 2){
      printf("Erreur lecture fichier ligne 2\n");
      exit(1);
    }

    afn_init(A, nbetat, alphabet, init, finals);

    while (fscanf(fd, "%u %c %u", &q1, &s, &q2) == 3)
      afn_add_trans(A, q1, s, q2);

    printf("\n");
    if (feof(fd) == 0){
      printf("Erreur fin de fichier\n");
      exit(1);
    }
    fclose(fd);
  }else{
    printf("Erreur ouverture fichier\n");
    exit(1);
  }
}

/*
  Retourne l'epsilon fermeture de l'ensemble d'états <R> par
  l'automate <A>
*/
ullong afn_epsilon_fermeture(afn A, ullong R){
  uint i, cpt = 0;
  ullong* pile = malloc(sizeof(ullong) * A.finals);
  ullong ferm = R, q, q1;
  for (i = 0; i < A.nbetat; i++){
    if (IN(i, R) == 1){
      pile[cpt++] = i;
    }
  }
  while (cpt > 0){
    q = pile[--cpt];
    for (q1 = 0; q1 < A.nbetat; q1++){
      if (IN(q1, A.delta[q][A.tsymb['&' - SYMB_ASCII_DEB]])){
        ferm |= INT_ETAT(q1);
        pile[cpt++] = q1;
      }
    }
  }
  free(pile);
  return ferm;
}

/*
  Calcule un automate deterministe equivalent à <A> et affecte le
  resultat a <D>.char* alphabet_c = A.alphabet Les etats de l'afd sont renumerotés à partir de 1
*/

void afn_determinisation(afn A, afd *D){

  uint taille_pile = 1<<10;
  uint taille_delta = 1<<10;
  uint taille_final = 1<<10;
  uint taille_bloc = 1<<10;

  uint s, nbetat = 0, nbfinal =0, p;
  int q_index;

  char * alphabet;
  uint nbsymb;

  /*On retire epsilon de l'alphabet du determinisé*/
  nbsymb = A.nbsymb-1;
  alphabet = malloc(sizeof(char)*A.nbsymb);
  strcpy(alphabet, A.alphabet+1);


  ullong *pile = malloc(taille_pile*sizeof(ullong));
  long long int sommet = 0;

  uint *finals = malloc(sizeof(uint)*taille_final);
  uint **delta = malloc(sizeof(uint *)*taille_delta);

  ullong q = afn_epsilon_fermeture(A,A.init), ferm;
  ensemble Q=NULL,R=NULL;
  pile[sommet++] = q;
  ens_ajouter(&Q, q, nbetat);
  delta[nbetat++] = calloc(nbsymb, sizeof(uint));

  while (sommet != 0){
    q = pile[--sommet];
    R = ens_rechercher(Q,q);

    q_index = R->index;
    /*On suppose toujours que '&' est le premier caractère de l'alphabet*/
    for (s=1; s<1+nbsymb; s++){
      ferm = 0ULL;
      for (p=0; p<A.nbetat; p++) {
	if (IN(p,q)) ferm |= afn_epsilon_fermeture(A, A.delta[p][s]);
      }
      if ((R=ens_rechercher(Q,ferm)) == NULL){
	/*si <ferm> contient un état final on le rajoute*/
	if (ferm & A.finals){
	  finals[nbfinal++] = nbetat;
	  if (nbfinal >= taille_final){
	    taille_final += taille_bloc;
	    finals = realloc(finals,taille_final*(sizeof(uint)));
	  }
	}
	/*on ajoute <ferm> à l'ensemble des états et a la pile de traitement*/
	ens_ajouter(&Q,ferm,nbetat);
	pile[sommet++] = ferm;
	delta[nbetat] = calloc(nbsymb, sizeof(uint));
	/*valeur de la transition*/
	delta[q_index][s-1] = nbetat++;

	/*Si la pile ou de la table est pleine on alloue plus de mémoire */
	if (sommet >=taille_pile){
	  taille_pile += taille_bloc;
	  pile = realloc(pile, taille_pile*sizeof(ullong));
	}
	if (nbetat >=taille_delta){
	  taille_delta += taille_bloc;
	  delta = realloc(delta, taille_delta*sizeof(uint *));
	}
      } else {
	/*on ajoute simplement la transition*/
	delta[q_index][s-1] = R->index;
      }
    }
  }
  afd_init(D, nbetat, alphabet, nbfinal, 0, finals);
  for (p=0; p < D->nbetat; p++){
    for (s=0; s< D->nbsymb; s++){

      afd_add_trans(D,p,D->alphabet[s],delta[p][s]);
    }
  }


  free(finals);
  free(pile);
  ens_free(Q);
  free(alphabet);
  for (q=0; q<nbetat; q++) free(delta[q]);
  free(delta);

}


/*
  Calcule l'automate qui reconnait le caractere <c> dans un alphabet a
  <nbsymb> symboles
*/

char ALL_ALPHA[64] = "&abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";


void afn_char(afn *C, char c){
  afn_init(C, 2, ALL_ALPHA, 1ULL, 2ULL);

  afn_add_trans(C, 0, c, 1);
}

/*
  Calcule un automate qui reconnait l'union de <A> et <B>
*/
void afn_union(afn *C, afn A, afn B){
  uint nbetat = A.nbetat + B.nbetat + 1;
  ullong init = (A.init << 1) | (B.init << (A.nbetat + 1));
  ullong finals = (A.finals << 1) | (B.finals << (A.nbetat + 1));

  afn_init(C, nbetat, ALL_ALPHA, 1ULL, finals);

  C->delta[0][0] = init;
  for (int q = 0; q < A.nbetat; q++){
    for (int s = 0; s < 63; s++){
      C->delta[q+1][s] = A.delta[q][s] << 1;
    }
  }
  for (int q = 0; q < B.nbetat; q++){
    for(int s = 0; s < 63; s++){
      C->delta[q+A.nbetat+1][s] = B.delta[q][s] << (A.nbetat + 1);
    }
  }
}

/*
  Calcule un automate qui reconnait la concatenation de <A> et <B>
*/
void afn_concat(afn* C, afn A, afn B){
  uint nbetat = A.nbetat + B.nbetat;
  ullong init = A.init;
  ullong finals = B.finals << (A.nbetat);
  afn_init(C, nbetat, ALL_ALPHA, init, finals);

  for (int q = 0; q < A.nbetat; q++){
    for (int s = 0; s < 63; s++){
      C->delta[q][s] = A.delta[q][s];
    }
  }
  for (int q = 0; q < A.nbetat; q++){
    if (IN(q, A.finals))
      afn_add_trans(C, q, '&', A.nbetat);
  }

  for (int q = 0; q < B.nbetat; q++){
    for (int s = 0; s < 63; s++){
      C->delta[q+A.nbetat][s] = B.delta[q][s] << A.nbetat;
    }
  }
}

/*
  Calcule un automate qui reconnait la fermeture de Kleene de <A>
*/
void afn_kleene(afn* C, afn A){
  uint nbetat = A.nbetat + 1;
  ullong init = 1ULL;
  ullong finals = (A.finals << 1) | 1ULL;

  afn_init(C, nbetat, ALL_ALPHA, init, finals);

  for (int q = 0; q < A.nbetat; q++){
    for (int s = 0; s < 63; s++){
      C->delta[q+1][s] = A.delta[q][s] << 1;
    }
  }

  for (int q = 0; q < A.nbetat + 1; q++){
    if (IN(q, finals))
      afn_add_trans(C, q, '&', 1);
  }
}
