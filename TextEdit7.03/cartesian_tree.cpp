
#include "myTypes.h"
void tree_print(FILE * fp ,cartesian_tree_t* tree)
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
void cart_tree_construct(cartesian_tree_t** tree,string_t* str)
{
    cartesian_tree_t* new_tree = (cartesian_tree_t * )malloc(sizeof(cartesian_tree_t));/** = {NULL,NULL,s,rand(),s};**/
    string* insert_str = (string *)malloc(sizeof(string_t));
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
void cart_tree_merge(cartesian_tree_t** merged_tree, cartesian_tree_t** left_tree, cartesian_tree_t** right_tree)
{
        cartesian_tree_t* res = NULL;
        cartesian_tree_t* buff_ptr;
        cartesian_tree_t* help_ptr;
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
void cart_tree_split(cartesian_tree_t** splited_tree_ptr, cartesian_tree_t** left_tree_ptr, cartesian_tree_t** right_tree_ptr, size_t index)
{
    struct cartesian_tree* splited_tree;
	struct cartesian_tree* left_tree;
	struct cartesian_tree* right_tree;
	int left_ind = index;

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
void cart_tree_insert(cartesian_tree_t** tree, string_t* s, size_t index)
{
    cartesian_tree_t** left_tree;
    cartesian_tree_t** right_tree;
    cartesian_tree_t** insert_tree;
    cartesian_tree_t* left_pointer = NULL;
    cartesian_tree_t* right_pointer = NULL;
    cartesian_tree_t* insert_pointer = NULL;
    cartesian_tree_t* left_merge_ptr = NULL;
    /**case if tree was empty, new tree has one node**/
    if (*tree == NULL)
    {
        cart_tree_construct(tree, s);
        return;
    }
    /**init auxiliary buff trees**/
    left_tree = &left_pointer;
    right_tree = &right_pointer;

    cart_tree_split(tree, left_tree, right_tree, index);

    insert_tree = &insert_pointer;

    /**create new one-node tree**/
    cart_tree_construct(insert_tree, s);
    /**full merge**/
    cart_tree_merge(&left_merge_ptr, left_tree, insert_tree);
    cart_tree_merge(tree, &left_merge_ptr, right_tree);
}

/**recalc correct params**/
void recalc_params(cartesian_tree_t* tree)
{
    cartesian_tree_t* check_left;
    cartesian_tree_t* check_right;
    if (tree == NULL)
        return;
    check_left = tree->left;
    check_right = tree->right;
    /**update subtrees size: it's new sum sizes of left and right trees + 1 **/
    tree->size = ((check_left == NULL)? 0 : check_left->size) + ((check_right == NULL) ? 0 : check_right->size) + 1;
}

/**print all strings from struct DFS IN-order**/
void print_tree(FILE* fp,cartesian_tree_t* tree)
{
     string_t* out_str;
	if (tree == NULL)
	{
        fprintf(stderr,"List of strings is empty.\n");
		return;
	}

	if (tree->left != NULL)
        print_tree(fp,tree->left);

    out_str = tree->text;
    fprintf(fp, "%s\n", out_str->data);
	/**string_print(tree->text, fp);**/

	if (tree->right != NULL)
		print_tree(fp,tree->right);
}

int smart_print_tree(FILE * stdout, data_container * data, winsize * size_of_window) /**/
{
    return 0;
}

/**clear memory from tree**/
void tree_delete(cartesian_tree_t* tree)
{
    if (!tree)
        return;

    if(tree->left)
        tree_delete(tree->left);

    if(tree->right)
        tree_delete(tree->right);
    /**clear memory in current string**/
    string_delete(tree->text);
}

void cart_tree_remove(cartesian_tree_t** tree, size_t left_ind, size_t right_ind)
{
    cartesian_tree_t* left_tree_ptr = NULL;
    cartesian_tree_t* mid_tree_ptr = NULL;
    cartesian_tree_t* mid_left_tree_ptr = NULL;
    cartesian_tree_t* right_tree_ptr = NULL;
    cartesian_tree_t** left_tree = &left_tree_ptr;
    cartesian_tree_t** mid_tree = &mid_tree_ptr;
    cartesian_tree_t** mid_left_tree = &mid_left_tree_ptr;
    cartesian_tree_t** right_tree = &right_tree_ptr;

    cart_tree_split(tree, mid_left_tree, right_tree, right_ind);
    cart_tree_split(mid_left_tree, left_tree, mid_tree, left_ind - 1);
    cart_tree_merge(tree, left_tree, right_tree);

    /*print_tree(*mid_tree, );*/
    tree_delete(*mid_tree);
}
/****find****/
void cart_tree_access(cartesian_tree_t** tree_ptr, size_t index, string_t** s)
{
    int left_ind = index;
    cartesian_tree_t* tree;

    if (*tree_ptr == NULL)
        return;

    tree = *tree_ptr;
    /**sign of left_ind shows direction of new iteration**/
    if (tree->left != NULL)
    {
        cartesian_tree_t* left_pointer;
        left_pointer = tree->left;
        left_ind -= left_pointer->size;
    }


    if (left_ind < 0)
    {
        cart_tree_access(&(tree->left), index, s);
        return;
    }

    if (!left_ind)
    {
        *s = tree->text;
        return;
    }

    if (left_ind > 0)
    {
        cart_tree_access(&(tree->right), (size_t)(left_ind - 1), s);
        return;
    }   

}
/*cartesian_tree_t * cart_tree_get_and_remove(cartesian_tree_t** tree, size_t left_ind, size_t right_ind)
{
    cartesian_tree_t* left_tree_ptr = NULL;
    cartesian_tree_t* mid_tree_ptr = NULL;
    cartesian_tree_t* mid_left_tree_ptr = NULL;
    cartesian_tree_t* right_tree_ptr = NULL;
    cartesian_tree_t** left_tree = &left_tree_ptr;
    cartesian_tree_t** mid_tree = &mid_tree_ptr;
    cartesian_tree_t** mid_left_tree = &mid_left_tree_ptr;
    cartesian_tree_t** right_tree = &right_tree_ptr;

    cart_tree_split(tree, mid_left_tree, right_tree, right_ind);
    cart_tree_split(mid_left_tree, left_tree, mid_tree, left_ind - 1);
    cart_tree_merge(tree, left_tree, right_tree);

    return *mid_tree;
}
cartesian_tree_t * cart_tree_inserting_tree(cartesian_tree_t** source_tree, cartesian_tree_t ** dest_tree, size_t index)
{

}
cartesian_tree_t * cart_tree_get(cartesian_tree_t** tree, size_t left_ind, size_t right_ind);
{
    cartesian_tree_t * get_tree = cart_tree_get_and_remove(tree, left_ind, right_ind);
    return  get_tree;
}

*/


