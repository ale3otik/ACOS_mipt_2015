

/**simple version of cartesian tree for fast access to saved strings*/
#ifndef __CARTESIAN_TREE__
#define __CARTESIAN_TREE__
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "my_types.h"
/**struct is realized treap on implicit key**/
struct cartesian_tree
{
    int priority;
    long size;
    struct cartesian_tree* left;
    struct cartesian_tree* right;
    string* text;
};

typedef struct cartesian_tree cartesian_tree;

void cart_tree_construct(cartesian_tree** tree, string* str);
void cart_tree_merge(cartesian_tree** merged_tree, cartesian_tree** tree1,cartesian_tree** tree2);
void cart_tree_split(cartesian_tree** splited_tree, cartesian_tree** left_tree, cartesian_tree** right_tree,long index);
void cart_tree_insert(cartesian_tree** tree, string* str, long index);
void recalc_params(cartesian_tree* tree);
void print_tree(FILE * ,cartesian_tree* tree);
void debug_list_print(cartesian_tree * list);
int smart_print_tree(FILE * stdout, data_container * data, winsize * size_of_window);
void tree_print(FILE * fp,cartesian_tree* tree);
void tree_delete(cartesian_tree* tree);
cartesian_tree * cart_tree_remove(cartesian_tree** tree, long left_ind, long right_ind, int need_delete);
void cart_tree_access(cartesian_tree** tree_ptr, long index, string** s);
/**void cart_tree_assign(cartesian_tree* tree, char* str, long sz, long index);
void cart_tree_remove(long index);**/

long cartesian_size(struct cartesian_tree * _tree);

cartesian_tree * cart_tree_get_and_remove(cartesian_tree** tree, long left_ind, long right_ind);
cartesian_tree * cart_tree_get(cartesian_tree** tree, long left_ind, long right_ind);

void conv_tree_to_list(cartesian_tree** list_ptr, cartesian_tree** end_list_ptr, cartesian_tree** tree_ptr);
void conv_list_to_tree(cartesian_tree** list_ptr, cartesian_tree** end_list_ptr, cartesian_tree** tree_ptr);

void cart_tree_insert_tree(cartesian_tree** tree_ptr, cartesian_tree** inserted_tree_ptr, long index);

long cartesian_size(struct cartesian_tree * _tree);

#endif
