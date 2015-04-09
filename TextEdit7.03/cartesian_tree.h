

/**simple version of cartesian tree for fast access to saved strings*/
#ifndef __CARTESIAN_TREE__
#define __CARTESIAN_TREE__
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "MyTypes.h"
/**struct is realized treap on implicit key**/
struct cartesian_tree
{
    int priority;
    size_t size;
    struct cartesian_tree* left;
    struct cartesian_tree* right;
    string_t* text;
};

typedef struct cartesian_tree cartesian_tree_t;

void cart_tree_construct(cartesian_tree_t** tree, string_t* str);
void cart_tree_merge(cartesian_tree_t** merged_tree, cartesian_tree_t** tree1,cartesian_tree_t** tree2);
void cart_tree_split(cartesian_tree_t** splited_tree, cartesian_tree_t** left_tree, cartesian_tree_t** right_tree,size_t index);
void cart_tree_insert(cartesian_tree_t** tree, string_t* str, size_t index);
void recalc_params(cartesian_tree_t* tree);
void print_tree(FILE * ,cartesian_tree_t* tree);
int smart_print_tree(FILE * stdout, data_container * data, winsize * size_of_window);
void tree_print(cartesian_tree_t* tree);
void tree_delete(cartesian_tree_t* tree);
void cart_tree_remove(cartesian_tree_t** tree, size_t left_ind, size_t right_ind);

void cart_tree_access(cartesian_tree_t** tree_ptr, size_t index, string_t** s);
/**void cart_tree_assign(cartesian_tree_t* tree, char* str, size_t sz, size_t index);
void cart_tree_remove(size_t index);**/

cartesian_tree_t * cart_tree_get_and_remove(cartesian_tree_t** tree, size_t left_ind, size_t right_ind);
cartesian_tree_t * cart_tree_get(cartesian_tree_t** tree, size_t left_ind, size_t right_ind);

#endif
