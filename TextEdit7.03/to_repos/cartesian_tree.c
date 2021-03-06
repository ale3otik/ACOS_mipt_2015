#include "my_types.h"
static int TREE_ALLOCATED_MEMORY = 0;

void tree_print(FILE * fp ,cartesian_tree* tree)
{
    if(tree == NULL)
    {
        printf("tree is null\n");
        return;
    }
    printf("tree->text: ");
    string_print(fp,tree->text);
    printf("tree->size: %lu\n", tree->size);
    printf("tree->left: %p\n",(void*) tree->left);
    printf("tree->right: %p\n",(void*) tree->right);
    printf("tree->priority: %d\n", tree->priority);
    printf("\n");
}

/**base constructor for cartesian_tree**/
void cart_tree_construct(cartesian_tree** tree,string* str)
{
    cartesian_tree* new_tree = (cartesian_tree * )malloc(sizeof(cartesian_tree));/** = {NULL,NULL,s,rand(),s};**/
    string* insert_str = (string *)malloc(sizeof(string));


    TREE_ALLOCATED_MEMORY += 2;
   /* fprintf(stdout,"\nTREE_ALLOCATED_MEMORY::%d", TREE_ALLOCATED_MEMORY);*/

    insert_str->data = NULL;
    string_construct(insert_str);
    string_big_push(insert_str, str->data, str->size);
    new_tree->text = insert_str;
    new_tree->size = 1;
    new_tree->left = NULL;
    new_tree->right = NULL;
    new_tree->priority = rand();
    *tree = new_tree;
}

/**basic merge of two lists of strings that relized in the form of cartesian_tree*/
void cart_tree_merge(cartesian_tree** merged_tree, cartesian_tree** left_tree, cartesian_tree** right_tree)
{
    cartesian_tree* res = NULL;
    cartesian_tree* buff_ptr;
    cartesian_tree* help_ptr;
    /**cases if one of merged tree is NULL**/
	if (*left_tree == NULL)
	{
        *merged_tree = *right_tree;
        recalc_params(*merged_tree);
        return;
    }
    /**/
	if (*right_tree == NULL)
	{
        *merged_tree = *left_tree;
        recalc_params(*merged_tree);
        return;
	}

	/**new root node is root node one of merged tree with max priority**/
	if ((*left_tree)->priority > (*right_tree)->priority)
	{
		*merged_tree = *left_tree;

		help_ptr = *left_tree;
		buff_ptr = help_ptr->right;
		cart_tree_merge(&res, &buff_ptr, right_tree);
		(*merged_tree)->right = res;
	}
	else
	{
		*merged_tree = *right_tree;

		help_ptr = *right_tree;
		buff_ptr = help_ptr->left;
		cart_tree_merge(&res, left_tree, &buff_ptr);
		(*merged_tree)->left = res;
	}

	recalc_params(*merged_tree);
}

/**basic split of list of strings in two lists: [0,index) and [index,size)
 *for correct work of split subtrees' sizes were used
 *also checking to null pointers was needed
 **/
void cart_tree_split(cartesian_tree** splited_tree_ptr, cartesian_tree** left_tree_ptr, cartesian_tree** right_tree_ptr, long index)
{
    struct cartesian_tree* splited_tree;
	struct cartesian_tree* left_tree;
	struct cartesian_tree* right_tree;
	long left_ind = index;

	if (*splited_tree_ptr == NULL)
		return;

	splited_tree = *splited_tree_ptr;
	left_tree = NULL;
	right_tree = NULL;

    /**sign of left_ind shows direction of new iteration**/
	if (splited_tree->left != NULL)
	{
		struct cartesian_tree* left_pointer;
		left_pointer = splited_tree->left;
		left_ind -= left_pointer->size;
	}


	if (left_ind < 0)
	{
		right_tree = splited_tree;
		cart_tree_split(&(right_tree->left), &left_tree, &(right_tree->left), index);
		recalc_params(left_tree);
		recalc_params(right_tree);
		*left_tree_ptr = left_tree;
		*right_tree_ptr = right_tree;
		return;
	}

	if (!left_ind)
	{
		left_tree = splited_tree->left;
		right_tree = splited_tree;
		right_tree->left = NULL;
		recalc_params(right_tree);
		recalc_params(left_tree);
		*left_tree_ptr = left_tree;
		*right_tree_ptr = right_tree;
		return;
	}

	if (left_ind > 0)
	{
		left_tree = splited_tree;
		cart_tree_split(&(left_tree->right), &(left_tree->right), &right_tree, left_ind - 1);
		recalc_params(right_tree);
		recalc_params(left_tree);
		*left_tree_ptr = left_tree;
		*right_tree_ptr = right_tree;
		return;
	}
}

/**insert new string in list in index position**/
void cart_tree_insert(cartesian_tree** tree, string* _str, long index)
{
    cartesian_tree** left_tree;
    cartesian_tree** right_tree;
    cartesian_tree** insert_tree;
    cartesian_tree* left_pointer = NULL;
    cartesian_tree* right_pointer = NULL;
    cartesian_tree* insert_pointer = NULL;
    cartesian_tree* left_merge_ptr = NULL;
    /**case if tree was empty, new tree has one node**/
    if (*tree == NULL)
    {
        cart_tree_construct(tree, _str);
        return;
    }
    /**init auxiliary buff trees**/
    left_tree = &left_pointer;
    right_tree = &right_pointer;

    cart_tree_split(tree, left_tree, right_tree, index);

    insert_tree = &insert_pointer;

    /**create new one-node tree**/
    cart_tree_construct(insert_tree, _str);
    /**full merge**/
    cart_tree_merge(&left_merge_ptr, left_tree, insert_tree);
    cart_tree_merge(tree, &left_merge_ptr, right_tree);
}

/**recalc correct params**/
void recalc_params(cartesian_tree* tree)
{
    cartesian_tree* check_left;
    cartesian_tree* check_right;
    if (tree == NULL)
        return;
    check_left = tree->left;
    check_right = tree->right;
    /**update subtrees size: it's new sum sizes of left and right trees + 1 **/
    tree->size = ((check_left == NULL)? 0 : check_left->size) + ((check_right == NULL) ? 0 : check_right->size) + 1;
}

/**print all strings from struct DFS IN-order**/
static long _cartesian_tree_counter = 0;
void print_tree(FILE* fp,cartesian_tree* tree)
{
    string * out_str;

    if (tree == NULL)
    {
        fprintf(stdout,"List of strings is empty.\n");
        return;
    }
    
    if(_cartesian_tree_counter == 0)
    {
        _cartesian_tree_counter = tree->size - 1;
    }

	if(tree->left != NULL)
        print_tree(fp,tree->left);

    out_str = tree->text;
    fprintf(fp, "%s", out_str->data);
    if(_cartesian_tree_counter > 0) 
    {
        fprintf(fp, "\n");
        _cartesian_tree_counter--;
    }
	/**string_print(tree->text, fp);**/

	if (tree->right != NULL)
		print_tree(fp,tree->right);
}

void debug_list_print(cartesian_tree * list)
{
    while (list != NULL)
    {
        printf("\n%s",list->text->data);
        list = list->right;
    }
}
/**clear memory from tree**/
void tree_delete(cartesian_tree* tree)
{
    if (tree == NULL)
        return;

    if(tree->left)
        tree_delete(tree->left);

    if(tree->right)
        tree_delete(tree->right);

    /**clear memory in current string**/
    string_delete(tree->text);
    free(tree->text);
    free(tree);
    TREE_ALLOCATED_MEMORY -= 2;
   /* fprintf(stdout,"\nTREE_ALLOCATED_MEMORY:: %d", TREE_ALLOCATED_MEMORY);*/
}

cartesian_tree * cart_tree_remove(cartesian_tree** tree, long left_ind, long right_ind,int need_delete)
{
    cartesian_tree* left_tree_ptr = NULL;
    cartesian_tree* mid_tree_ptr = NULL;
    cartesian_tree* mid_left_tree_ptr = NULL;
    cartesian_tree* right_tree_ptr = NULL;
    cartesian_tree** left_tree = &left_tree_ptr;
    cartesian_tree** mid_tree = &mid_tree_ptr;
    cartesian_tree** mid_left_tree = &mid_left_tree_ptr;
    cartesian_tree** right_tree = &right_tree_ptr;

    cart_tree_split(tree, mid_left_tree, right_tree, right_ind);
    cart_tree_split(mid_left_tree, left_tree, mid_tree, left_ind - 1);
    cart_tree_merge(tree, left_tree, right_tree);

    /*print_tree(*mid_tree, );*/
    if(need_delete == TRUE)
    {
        tree_delete(*mid_tree);
        return NULL;
    }
    else 
    {
        return *mid_tree;
    }
}
/****find****/
void cart_tree_access(cartesian_tree** tree_ptr, long index, string** _returned_str)
{
    long left_ind = index;
    cartesian_tree* tree;

    if (*tree_ptr == NULL)
        return;

    tree = *tree_ptr;
    /**sign of left_ind shows direction of new iteration**/
    if (tree->left != NULL)
    {
        cartesian_tree* left_pointer;
        left_pointer = tree->left;
        left_ind -= left_pointer->size;
    }


    if (left_ind < 0)
    {
        cart_tree_access(&(tree->left), index, _returned_str);
        return;
    }

    if (!left_ind)
    {
        *_returned_str = tree->text;
        return;
    }

    if (left_ind > 0)
    {
        cart_tree_access(&(tree->right), (long)(left_ind - 1), _returned_str);
        return;
    }   

}

long cartesian_size(struct cartesian_tree * _tree)
{
    if(_tree == NULL) return 0;
    return (long)_tree->size;
}

/*convert cartesian_tree in double-linked list, getting begin and end of list*/
void conv_tree_to_list(cartesian_tree** list_ptr, cartesian_tree** end_list_ptr, cartesian_tree** tree_ptr)
{
    cartesian_tree* list = *list_ptr;
    cartesian_tree* end_list = *end_list_ptr;
    cartesian_tree* tree = *tree_ptr;
    cartesian_tree* right_list = NULL;
    cartesian_tree* left_end_list = NULL;

    if(tree->left == NULL)
        list = tree;
    else
    {
        conv_tree_to_list(&list, &left_end_list, &(tree->left));
        left_end_list->right = tree;
        tree->left = left_end_list;
    }

    if(tree->right == NULL)
        end_list = tree;
    else
    {
        conv_tree_to_list(&right_list, &end_list, &(tree->right));
        right_list->left = tree;
        tree->right = right_list;
    }

    *list_ptr = list;
    *end_list_ptr = end_list;
    tree = NULL;
    *tree_ptr = tree;
}

void conv_list_to_tree(cartesian_tree** list_ptr, cartesian_tree** end_list_ptr, cartesian_tree** tree_ptr)
{
    cartesian_tree* list = *list_ptr;
    cartesian_tree* end_list = *end_list_ptr;
    cartesian_tree* tree = *tree_ptr;
    cartesian_tree* ins_tree = NULL;
    cartesian_tree* buff_tree = NULL;

    while(list != NULL)
    {
        ins_tree = list;

        if(list != end_list)
            list = list->right;
        else
            list = NULL;

        ins_tree->right = NULL;
        ins_tree->left = NULL;
        cart_tree_merge(&buff_tree, &tree, &ins_tree);
        tree = buff_tree;
    }
    *tree_ptr = tree;
}

/**insert new list in current list of strings in index position**/
void cart_tree_insert_tree(cartesian_tree** tree_ptr, cartesian_tree** inserted_tree_ptr, long index)
{
cartesian_tree* left_tree = NULL;
cartesian_tree* right_tree = NULL;
cartesian_tree* sub_tree = NULL;
cartesian_tree* inserted_tree = *inserted_tree_ptr;
cartesian_tree* tree = *tree_ptr;

cart_tree_split(&tree, &left_tree, &right_tree, index);
cart_tree_merge(&sub_tree, &left_tree, &inserted_tree);
cart_tree_merge(&tree, &sub_tree, &right_tree);

*tree_ptr = tree;
}


