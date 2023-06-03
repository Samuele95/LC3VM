#include <stdlib.h>
#include <string.h>
#include "lc3parsetree.h"



node_t* new_node(char* token) {
        node_t* newnode = (node_t* ) malloc(sizeof(node_t));
        newnode->left = newnode->right = NULL;
        newnode->token = strdup(token);
        return newnode;
}

node_t* insert(node_t* rootnode, char* token) {
    if (rootnode == NULL) return new_node(token);
    rootnode->left = insert(rootnode->left, token);
    return rootnode;
}

int deletenode(node_t* node) {
    if (node == NULL) return 0;
    free(node->token);
    deletenode(node->left);
    deletenode(node->right);
    free(node);
}

void printtree(node_t *tree) {
    if (tree != NULL) {
        printtree(tree->left);
        printf("%s\n", tree->token);
        printtree(tree->right);
    }
}
