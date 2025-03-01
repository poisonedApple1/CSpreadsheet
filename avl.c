#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdbool.h>
#include "avl.h"
#include  "utils.h"



int height(avl_node *node) {
    return node ? node->height : 0;
}

avl_tree *avl_create() {
    avl_tree *tree = NULL;
    if((tree = malloc(sizeof(avl_tree))) == NULL) {
        return NULL;
    }
    tree->root = NULL;
    return tree;
} 
avl_node *avl_create_node(int value) {
    avl_node *node = NULL;
    if((node = malloc(sizeof(avl_node))) == NULL) {
        return NULL;
    }
    node->data = value;
    node->left = node->right = NULL;
    node->height = 1;  
    node->indegree = 1;
    return node;
}

// Right rotation.
avl_node *rotateRight(avl_node *y) {
    avl_node *x = y->left;
    avl_node *T2 = x->right;
    // Perform rotation.
    x->right = y;
    y->left = T2;
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;
    return x;
}

// Left rotation.
avl_node *rotateLeft(avl_node *x) {
    avl_node *y = x->right;
    avl_node *T2 = y->left;
    // Perform rotation.
    y->left = x;
    x->right = T2;
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;
    return y;
}
 
int getBalance(avl_node *node) {
    return node ? height(node->left) - height(node->right) : 0;
}
 
avl_node *avl_insert_node(avl_node *node, int value) { 
    if (node == NULL)               return avl_create_node(value);
    
    if (value < node->data)        node->left = avl_insert_node(node->left, value);
    else if (value > node->data)   node->right = avl_insert_node(node->right, value);
    else {                           
    //already present
    node->indegree++;
    return node;
    }
    node->height = max(height(node->left), height(node->right)) + 1;
    int balance = getBalance(node);
     
    // Left Left Case.
    if (balance > 1 && value < node->left->data)   return rotateRight(node);
    
    // Right Right Case.
    if (balance < -1 && value > node->right->data) return rotateLeft(node);
    
    // Left Right Case.
    if (balance > 1 && value > node->left->data) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    
    // Right Left Case.
    if (balance < -1 && value < node->right->data) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    
    return node;
}
 
void avl_insert(avl_tree *tree, int value) {
    tree->root = avl_insert_node(tree->root, value);
}

//Find the node with the minimum value.
avl_node *min_val_node(avl_node *node) {
    avl_node *current = node;
    while (current && current->left != NULL)
        current = current->left;
    return current;
}
 
avl_node *avl_delete_node(avl_node *root, int value) { 
    if (root == NULL)               return root;
    
    if (value < root->data)        root->left = avl_delete_node(root->left, value);
    else if (value > root->data)   root->right = avl_delete_node(root->right, value);
    else {
        // Delete the node
        if(root->left==NULL && root->right==NULL){
            free(root);
            return NULL;
        }
        else if(root->left==NULL){
            avl_node *temp = root->right;
            root->right = NULL;
            free(root);
            return temp;
        }
        else if(root->right==NULL){
            avl_node *temp = root->left;
            root->left = NULL;
            free(root);
            return temp;
        }
        else{
            avl_node *temp = min_val_node(root->right);
            root->data = temp->data;
            root->right = avl_delete_node(root->right, temp->data);
        } 
    }
     
    if (root == NULL) return root;
     
    root->height = max(height(root->left), height(root->right)) + 1;
    int balance = getBalance(root);
    
    // Left Left Case.
    if (balance > 1 && getBalance(root->left) >= 0)
        return rotateRight(root);
    
    // Left Right Case.
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }
    
    // Right Right Case.
    if (balance < -1 && getBalance(root->right) <= 0)
        return rotateLeft(root);
    
    // Right Left Case.
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }
    return root;
}
 
void avl_delete(avl_tree *tree, int value) {
    tree->root = avl_delete_node(tree->root, value);
}

avl_node *avl_find(avl_tree *tree, int value){
    avl_node *cur = tree->root;
    while (cur) {
        if (value > cur->data)
            cur = cur->right;
        else if (value < cur->data)
            cur = cur->left;
        else
            return cur;
    }
    return NULL;
}



// just for better printing the tree in the console
void print_tree_helper(avl_node *node, const char *prefix, int isLeft) {
    if (node != NULL) {
        printf("%s", prefix);
        printf(isLeft ? "+-- " : "`-- ");
        printf("%d(%d)\n", node->data,node->indegree);
        char newPrefix[256];
        snprintf(newPrefix, sizeof(newPrefix), "%s%s", prefix, isLeft ? "|   " : "    ");
        print_tree_helper(node->left, newPrefix, 1);
        print_tree_helper(node->right, newPrefix, 0);
    }
}
void pretty_print(avl_tree *tree) {
    if (tree->root == NULL) {
        printf("Tree is empty.\n");
        return;
    }
    printf("%d(%d)\n", tree->root->data,tree->root->indegree);
    printf("|\n");
    print_tree_helper(tree->root->left, "", 1);
    print_tree_helper(tree->root->right, "", 0);
}

 
// int main() {
//     avl_tree *tree = avl_create(); 
//     avl_insert(tree,1);
//     pretty_print(tree);
//     avl_insert(tree,2);
//     pretty_print(tree );
//     avl_insert(tree,3);
//     pretty_print(tree );
//     avl_insert(tree,4);
//     pretty_print(tree );
//     avl_insert(tree,5);
//     pretty_print(tree );
//     avl_insert(tree,6);
//     pretty_print(tree );
//     avl_insert(tree,7);
    
//     avl_insert(tree,8);
//     pretty_print(tree );
//     avl_insert(tree,9);
//     pretty_print(tree );
//     avl_insert(tree,10);
//     pretty_print(tree);
//     avl_insert(tree,11);
//     pretty_print(tree );
//     avl_insert(tree,12);
//     pretty_print(tree);
//     avl_insert(tree,13);
//     pretty_print(tree );
//     avl_insert(tree,14);
//     pretty_print(tree ); 
//     avl_insert(tree,5);
//     pretty_print(tree );
//     avl_insert(tree,-1);
//     printf("After inserting -1\n");
//     pretty_print(tree );
//     avl_delete(tree,0);
//     printf("After deleting 0\n");
//     pretty_print(tree );
//     avl_delete(tree,1);
//     printf("After deleting 1\n");
//     pretty_print(tree );
//     avl_delete(tree,2);
//     printf("After deleting 2\n");
//     pretty_print(tree );

    
//     return 0;
// }
