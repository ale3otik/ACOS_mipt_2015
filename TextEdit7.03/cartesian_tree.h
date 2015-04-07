/*#include "string_for_text_editor.h"*/


#ifndef __CARTESIAN_TREE_DEF__
#define __CARTESIAN_TREE_DEF__

#include <stdlib.h>
#include <unistd.h>
#include "MyTypes.h"

struct cartesian_tree
{
    int priority;
    size_t size;
    struct cartesian_tree* left;
    struct cartesian_tree* right;
    struct string* text;
};

void cart_tree_construct(struct cartesian_tree** tree, struct string* str);
void cart_tree_merge(struct  cartesian_tree** merged_tree, struct cartesian_tree** tree1,struct cartesian_tree** tree2);
void cart_tree_split(struct cartesian_tree** splited_tree, struct cartesian_tree** left_tree, struct cartesian_tree** right_tree,size_t index);
void cart_tree_insert(struct cartesian_tree** tree, struct string* str, size_t index);
void recalc_params(struct cartesian_tree* tree);
void print_tree(FILE* fout,struct cartesian_tree* tree);
/*void tree_print(struct cartesian_tree* tree);*/
void tree_delete(struct cartesian_tree* tree);
#else
#endif
