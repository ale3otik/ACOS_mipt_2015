#include <time.h>
#include <stdlib.h>
#include <libc.h>

#include "myTypes.h"

/*void tree_print(struct cartesian_tree* _tree)
{
    if(_tree == NULL)
    {
        printf("tree is null\n");
        return;
    }
    printf("tree->text: ");
    string_print(_tree->text);
    printf("tree->size: %lu\n",_tree->size);
    printf("tree->left: %p\n", (void*)_tree->left);
    printf("tree->right: %p\n",(void*) _tree->right);
    printf("tree->priority: %d\n", _tree->priority);
    printf("\n");
}*/

void cart_tree_construct(struct cartesian_tree** _tree,struct string* _str)
{
    struct cartesian_tree* new_tree = (struct cartesian_tree *)malloc(sizeof(struct cartesian_tree));
    struct string* insert_str = (struct string *)malloc(sizeof(struct string));
    insert_str->data = NULL;
    string_construct(insert_str);
    string_big_push(insert_str, _str->data, _str->size);
    new_tree->text = insert_str;
    new_tree->size = 1;
    new_tree->left = NULL;
    new_tree->right = NULL;
    new_tree->priority = rand();
    *_tree = new_tree;
}
/**base constructor for cartesian_tree**/

/**tree construct is OK*/
/**basic Merge*/
void cart_tree_merge(struct cartesian_tree** merged_tree, struct cartesian_tree** left_tree, struct cartesian_tree** right_tree)
{

	if (*left_tree == NULL)
	{
        *merged_tree = *right_tree;
        return;
    }
	if (*right_tree == NULL)
	{
        *merged_tree = *left_tree;
        return;
	}

	if ((*left_tree)->priority > (*right_tree)->priority)
	{
        struct cartesian_tree* res = NULL;
		*merged_tree = *left_tree;
		cart_tree_merge(&res, &((*left_tree)->right), right_tree);
		(*merged_tree)->right = res;
	}
	else
	{
        struct cartesian_tree* res = NULL;
		*merged_tree = *right_tree;
		cart_tree_merge(&res, left_tree, &((*right_tree)->left));
		(*merged_tree)->left = res;
	}

	recalc_params(*merged_tree);
}

void cart_tree_split(struct cartesian_tree** splited_tree, struct cartesian_tree** left_tree, struct cartesian_tree** right_tree, size_t index)
{
    int left_ind = index;
    
    
	if (*splited_tree == NULL)
		return;

    if((*splited_tree)->left != NULL)
    {
        struct cartesian_tree* left_pointer;
        left_pointer = (*splited_tree)->left;
        left_ind -= left_pointer->size;
    }

	if (left_ind < 0)
	{
		*right_tree = *splited_tree;
		cart_tree_split(&((*right_tree)->left), left_tree, &((*right_tree)->left), index);
		recalc_params(*right_tree);
	}
	else if (!left_ind)
	{
		(*left_tree) = (*splited_tree)->left;
		(*right_tree) = *splited_tree;
		(*right_tree)->left = NULL;
		recalc_params(*right_tree);
		recalc_params(*left_tree);
	}
	else if (left_ind > 0)
	{
		(*left_tree) = *splited_tree;
		cart_tree_split(&((*left_tree)->right), &((*left_tree)->right), right_tree, left_ind - 1);
		recalc_params(*left_tree);
	}
}

void cart_tree_insert(struct cartesian_tree** _tree, struct string* _str, size_t index)
{

	struct cartesian_tree** left_tree;
	struct cartesian_tree** right_tree;
	struct cartesian_tree* left_pointer;
	struct cartesian_tree* right_pointer;

	struct cartesian_tree** insert_tree;
	struct cartesian_tree* insert_pointer;
    struct cartesian_tree* left_merge_ptr = NULL;
	if (*_tree == NULL)
	{

		cart_tree_construct(_tree, _str);
		return;
	}

	left_pointer = right_pointer = NULL;
	left_tree = &left_pointer;
	right_tree = &right_pointer;

	cart_tree_split(_tree, left_tree, right_tree, index);

	insert_pointer = NULL;
	insert_tree = &insert_pointer;
    cart_tree_construct(insert_tree, _str);
	cart_tree_merge(&left_merge_ptr, left_tree, insert_tree);
	cart_tree_merge(_tree, &left_merge_ptr, right_tree);
}

/**recalc correct params*/
void recalc_params(struct cartesian_tree* _tree)
{
    struct cartesian_tree* check_left = _tree->left;
    struct cartesian_tree* check_right = _tree->right;
    
	if (_tree == NULL)
		return;
        
	_tree->size = ((check_left == NULL)? 0 : check_left->size) + ((check_right == NULL) ? 0 : check_right->size) + 1;
}

void print_tree(FILE *fout,struct cartesian_tree* _tree)
{
	if (_tree == NULL || _tree->size == 0)
	{
		fprintf(stderr, "\nTREE IS EMPTY");
		return;
	}
	if (_tree->left != NULL)
        print_tree(fout,_tree->left);

	string_print(fout,_tree->text);

	if (_tree->right != NULL)
		print_tree(fout,_tree->right);
}

/**clear memory from tree**/
void tree_delete(struct cartesian_tree* _tree)
{
    if (!_tree)
        return;

    if(_tree->left)
        tree_delete(_tree->left);

    if(_tree->right)
        tree_delete(_tree->right);

    string_delete(_tree->text);
}

