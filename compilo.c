#include <stdio.h>
#include <stdlib.h>

#include "afn.h"
#include "afd.h"
#include "compregex.h"


int main(int argc, char* argv[]){
	afn automate_1;
	afn_char(&automate_1, 'a');

	afn automate_2;
	afn_char(&automate_2, 'b');

	afn automate_3;
	afn_union(&automate_3, automate_1, automate_2);
	printf("Union des automates 1 et 2: ");
	afn_print(automate_3);
	//On free les automates qui sont utilisés
	afn_free(&automate_1);
	afn_free(&automate_2);

	afn automate_4;
	afn_char(&automate_4, 'c');

	afn automate_5;
	afn_concat(&automate_5, automate_3, automate_4);
	printf("Concatenation des automates 3 et 4: ");
	afn_print(automate_5);
	//On free les automates qui sont utilisés
	afn_free(&automate_3);
	afn_free(&automate_4);

	afd A;
	afn_determinisation(automate_5, &A);
	afd_simul("bc", A);
	//On libère l'espace mémoire de l'AFD
	afd_free(&A);
}
