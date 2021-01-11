###### Grammaire ######

Exp -> Terme Reste1
Reste1 -> + Terme Reste1 | e
Terme -> Facteur Reste2
Reste2 -> . Facteur Reste2 | e
Facteur -> Facteur2* | Facteur2
Facteur2 -> Char | (Exp)


###### Les principaux fichiers et leurs fonctions ######


AFD.H:
Ce fichier contient tout les prototypes des fonctions nécessaires aux
fonctionnement des AFD.

afd_simul:
  Cette fonction prends en paramètre une chaine de caractère et un AFD.
  Elle sert à valider cette chaine à travers l'automate.
  Une chaine est validée si elle s'arrête sur un état final de l'AFD


AFN.H
Ce fichier contient tout les prototypes des fonctions nécessaires aux
fonctionnement des AFN.

afn_epsilon_fermeture:
  Cette fonction sert à calculer la fermeture epsilon de l'automate passé
  en paramètre

afn_determinisation:
  Cette fonction sert à transformer un AFN en un AFD.

kleene:
  Cette fonction prends en paramètre deux AFN.
  Le premier étant celui qui va contenir fermeture de Kleene (C), et le deuxième
  l'automate auquel on l'applique (A).
  en paramètre. La fermeture de Kleene étant définit par ce quintuplet:
  Q_c = Q_a U {0}
  I_c = {0}
  F_c = F_a U {0}
  D_c = D_a U {(0, &, I_a)} U {(q, &, I_a), tq q appartient à F_a}


ABR.H:
Ce fichier contient des fonctions qui vont servir pour l'algorithme de
déterminisation. Ce fichier définit une structure d'ensemble équivalent
à un arbre binaire de recherche. Chaque noeud ne possédant que 2 fils maximum
et dont le fils gauche est <= noeud parent, et le fils droit > noeud parent


###### Fonctionnement du programme #######

Toutes les fonctions ci-dessous sont présentes dans le fichier compregex.h

Pour illustrer le fonctionnement du programme, on va prendre:
la regex suivante: 'a.(b+c)' et la chaine 'ab'


Première étape: scanner

Le passage dans scanner va servir à transformer une la regex en une
liste d'unité lexical.
Pour notre chaîne, cela va renvoyer la liste suivante:
CHAR a
CONCAT .
PAR_OUV (
CHAR b
UNION +
CHAR c
PAR_FER )

Deuxième étape: parser
Le passage dans parser va servir à transformer cette liste d'unité lexical en une liste
postfixé respectant l'ordre de priorité de la grammaire. Avec notre liste précédentes, cela
nous donne la liste postfixé suivante:
CHAR a
CHAR b
CHAR c
UNION +
CONCAT .

Ce qui se lit (en ordre de priorité), on effectue l'union de B et C, puis on concatène
cette union avec A


Troisième étape: codegen
Le passage dans codegen va servir à créer un fichier 'compilo.c' à l'aide de la
liste postfixé calculé ci-dessus, de la chaine de caractère ('ab' ici) et d'un autre paramètre
utilisé pour le débuggage et l'affiches des fonctions annexes

Le fichier compilo.c va donc contenir chaque AFN de la regex, puis va le déterminiser
et enfin va simuler son éxecution à l'aide de la fonction afd_simul décrité en haut.
Ainsi, cela donne pour notre regex et notre chaine:

Création AFN A reconnaissant a
Création AFN B reconnaissant b
Création AFN C reconnaissant c
Création AFN D calculant l'Union de B et C
Création AFN E calculant la Concaténation de A et D

Création AFD G qui va contenir la déterminisation de l'automate E
Execution de la chaine 'ab' avec l'automate E

En sortie on à donc: Chaine Valide


###### Explication règle makefile ######

Dans le makefile, il y a 2 règles annexes supplémentaires.

La première étant 'example_with_debug'
Cette regèle va éxécuter le programme avec une regex et une chaine par défaut,
et en mode debug (option 'd')
Cette règle va donc afficher l'éxecution étape par étape du programme:
Affichage liste unité lexicale
Affiche liste postfixée
Affichage de chaque automate crée


La deuxième règle étant l'inverse de la première sans affichage supplémentaires.


###################
# Baptiste LEROUX #
# Yoann LE POLLES #
###################
