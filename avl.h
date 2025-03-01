#ifndef AVL_H
#define AVL_H

typedef struct avl_node {
    struct avl_node *left;
    struct avl_node *right;
    int data;
    int height;
    int indegree;  
}avl_node;

typedef struct avl_tree {
    avl_node *root;
}avl_tree;


avl_tree *avl_create();
void avl_insert(avl_tree *tree, int value);
void avl_delete(avl_tree *tree, int value);
avl_node *avl_find(avl_tree *tree, int value);
void pretty_print(avl_tree *tree);


#endif

