#include "abr.h"


void ens_ajouter(ensemble* node, ullong value, uint index){
  if (*node == NULL){
    *node = calloc(1, sizeof(struct ensemble));
    (*node)->key = value;
    (*node)->index = index;
    (*node)->left = NULL;
    (*node)->right = NULL;

    return;
  }

  if (value <= (*node)->key){
    ens_ajouter(&((*node)->left), value, index);
  }else if(value > (*node)->key){
    ens_ajouter(&((*node)->right), value, index);
  }
}

ensemble ens_rechercher(ensemble arbre, ullong key){
  if (arbre == NULL)
    return NULL;

  if (key == arbre->key){
    return arbre;
  }

  if (key < arbre->key)
    return ens_rechercher(arbre->left, key);
  else
    return ens_rechercher(arbre->right, key);
}

void ens_free(ensemble arbre){
  if (arbre == NULL){
    return;
  }
  ens_free(arbre->left);
  ens_free(arbre->right);
  free (arbre);
}
