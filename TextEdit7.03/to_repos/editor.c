#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include "my_types.h"

int parse_command(string,long *);

void editor_print_pages(data_container ** data,long start_index);
void editor_print_range(string * command_str,long offset, data_container ** data);
void editor_insert_after(string* command,long offset,data_container ** data);
void editor_edit_string(string * command,long offset,data_container ** data);
void editor_replace(string * command_str,long offset, data_container ** data);
void editor_delete_range(string * command_str,long offset, data_container ** data);
void editor_delete_braces(string * command_str, long offset, data_container ** data);
int  special_read(string * command_str,long offset,data_container ** data, char * cur_filen);
void special_write(string * command_str,long,char *cur_file_name,data_container * data);
void special_exit();
void special_help();


int general_function(char* cur_file_name,data_container * data)
{
    string command_str;
    int entered_command = ERROR;
    long offset;
    command_str.data = NULL;

    if(entered_command != NO_ACTIONS_NEED)
    {
        system("clear"); /*clear screen*/
    }

    while(TRUE)
    {
        string_construct(&command_str);
        printf("\n>>editor:");
       /* tree_print(stderr,data);*/
        if(!string_get(stdin,&command_str)) /*read command*/
        { 
            entered_command = EXIT;
        }
        else 
        {
            offset= 0;
            entered_command = parse_command(command_str,&offset);
        }
        switch (entered_command)
        {
            case PRINT_PAGES:
                editor_print_pages(&data,1);
                break;
            case PRINT_RANGE :
                editor_print_range(&command_str,offset,&data);
                break;
            case INSERT_AFTER :
             editor_insert_after(&command_str, offset, &data);
                break;
            case EDIT_STRING :
                editor_edit_string(&command_str,offset,&data);
                break;
            case REPLACE :
                editor_replace(&command_str,offset, &data);
                break;
            case DELETE_RANGE :
                editor_delete_range(&command_str,offset,&data);
                break;
            case DELETE_BRACES :
                editor_delete_braces(&command_str,offset, & data);
                break;
            
            case READ :
                special_read(&command_str,offset,&data,NULL);
                break;
            case OPEN :
                special_read(&command_str,offset, &data,cur_file_name);
                break;
            case WRITE :
                special_write(&command_str,offset,cur_file_name,data);
                break;
            case EXIT :
                special_exit();
                return EXIT;
                break;
            case HELP :
                special_help();
                break;
            case NO_ACTIONS_NEED:
                break;
            default:
                fprintf(stderr,"unknown command: \"%s\"\n",command_str.data);
        }
    }
    return 0;
}

void editor_print_pages(data_container ** data, long start_index)
{
    struct termios old_term,new_term;
    char ch;
    winsize window_size;  
    int is_exit = FALSE;
    long count_printed_real_strings = start_index;
    int balance = 0;
    int is_need_new_str = FALSE;
    cartesian_tree * list = NULL;
    cartesian_tree * end_list = NULL;
    cartesian_tree * begin_list = NULL;

    ioctl(1,TIOCGWINSZ,&window_size);
    tcgetattr( STDIN_FILENO, &old_term);
    new_term = old_term;
    new_term.c_lflag &= ~( ICANON | ECHO );
    
    if(cartesian_size(*data) == 0)
    {
        fprintf(stderr, "\nhave no strings");
    }
    else
    {
        conv_tree_to_list(&begin_list, &end_list, data);
        list = begin_list;

        while(!is_exit)
        {   
            system("clear");
            balance = window_size.ws_row ;
            while(balance > 0 && list != NULL)
            {
                is_need_new_str = super_print_string(list->text,&window_size,&count_printed_real_strings, &balance);
                if(is_need_new_str)
                {
                    list = list->right;
                }
            }
            if(list == NULL) is_exit = TRUE;
            tcsetattr( STDIN_FILENO, TCSANOW, &new_term );
            while(TRUE)
            {
                ch = getchar();
                if(ch == ' ')
                {
                    tcsetattr( STDIN_FILENO, TCSANOW, &old_term );
                    break;
                }
                if(ch == 'q' || ch == 'Q')
                {
                    tcsetattr( STDIN_FILENO, TCSANOW, &old_term );
                    is_exit = TRUE;
                    break;
                }  
            }
        }
        conv_list_to_tree(&begin_list, &end_list, data);
    }
}

void editor_print_range(string * command_str,long offset, data_container ** data)
{
    long range[2];
    cartesian_tree * printed_tree = NULL;
    range[0] = -1;
    range[1] = -1;
    offset = string_get_next_position(command_str,offset);

    range[0] = string_get_number(command_str,&offset);
    offset = string_get_next_position(command_str,offset);
    if(command_str->data[offset] != '\0')
    {
        range[1] = string_get_number(command_str, &offset);
    }
    else
    {
        range[1] = cartesian_size(*data) ;
    }

    if(range[0] > range[1] || range[0] <= 0 || range[1] <0 || range[1] > cartesian_size(*data))
    {
        fprintf(stderr,"\nERROR: wrong range %ld %ld",range[0],range[1]);
        return;
    }
    range[0] -- ;

    printed_tree = cart_tree_remove(data, range[0], range[1], FALSE);
    editor_print_pages(&printed_tree,range[0]+1);
    cart_tree_insert_tree(data, &printed_tree, range[0]);
}

void editor_insert_after(string* command,long offset,data_container ** data)
{
    long position = -1;
    offset = string_get_next_position(command,offset);
    position = string_get_number(command, &offset);
    if(position == -1) 
        {
            position = cartesian_size(*data);
        }
    if(position < 0 || position > cartesian_size(*data))
    {
        fprintf(stderr, "\nERROR: wrong position %ld ",position);
        return;
    }

    insert_after_special_function(command,offset, data,position);
}
void editor_edit_string(string * command,long offset, data_container ** data)
{
    #define _EDIT_INSERT 1
    #define _EDIT_DELETE 2
    #define _EDIT_REPLACE 3

    long array_position = -1;
    long string_position = -1;
    char * name_commad = NULL;
    int number_of_operation = ERROR;

    string * finding_string = NULL;

    offset = string_get_next_position(command,offset);
    name_commad = get_next_word(command,&offset);

    offset = string_get_next_position(command, offset);
    array_position = string_get_number(command, &offset);

    offset = string_get_next_position(command, offset);
    string_position = string_get_number(command, &offset);

    /*cath the command*/
    if( ! strncmp(name_commad,"insert",MAX_NAME_LENGTH))
    {   
        number_of_operation = _EDIT_INSERT;

    }
    else if( ! strncmp(name_commad,"delete",MAX_NAME_LENGTH))
    {
        number_of_operation = _EDIT_DELETE;

    }
    else if( ! strncmp(name_commad,"replace",MAX_NAME_LENGTH))
    {
        number_of_operation = _EDIT_REPLACE;

    }

    if(number_of_operation == ERROR) 
    {
        fprintf(stderr,"\n unknow edit_string command : \"%s\" ",name_commad);
        return;
    }

    --array_position;
    if(array_position < 0 || array_position >= cartesian_size(*data))
    {
        fprintf(stderr,"\nERROR:wrong array_position %ld ", array_position);
        return;
    }

    offset = string_get_next_position(command, offset);

    /* insert string*/
    if(number_of_operation == _EDIT_INSERT)
    {
        special_edit_insert_function(command,offset,data,array_position,string_position);
        fprintf(stderr, "\nSucces insert ");
        return;
    }

    cart_tree_access(data, array_position, &finding_string);
    if(string_position <  0)
    {
        string_position = finding_string->size;
    }

    if(number_of_operation == _EDIT_DELETE)
    {
        string_position--;
        if(string_position >= finding_string->size || string_position < 0)
        {
            fprintf(stderr, "\nERROR:wrong number");
            return;
        }
        string_remove(finding_string, string_position);
        fprintf(stderr, "\nSucces delete");
    }
    if(number_of_operation == _EDIT_REPLACE)
    {
        char inserting_char;

        string_position--;
        if(string_position >= finding_string->size || string_position < 0)
        {
            fprintf(stderr, "\nERROR:wrong number");
            return;
        }

        offset = string_get_next_position(finding_string, offset);
        if( command->data[offset] == 0)
        {
            fprintf(stderr,"\nERROR: empty symb ");
            return;
        }

        inserting_char = command->data[offset];
        ++offset;
        offset = string_get_next_position(finding_string,offset);
        if(command->data[offset] != 0 && command->data[offset] != '#')
        {
            fprintf(stderr, "\nERROR: wring format of str");
            return;
        }

        finding_string->data[string_position] = inserting_char;
        
        fprintf(stderr, "\nSucces replace ");
        return;
    }
        /**/
}
   

void editor_replace(string * command_str,long offset, data_container ** data)
{
    string searching_str; /* that need to be replaced*/
    string replacing_str; /* that will insert*/
    cartesian_tree * get_tree = NULL;
    cartesian_tree * get_list = NULL;
    cartesian_tree * old_get_list = NULL;
    cartesian_tree * end_list = NULL;
    long  range[2];
    short replace_mode = NORMAL_REPLACE_MODE;

    offset = get_ranges(range, command_str, *data,offset);
    if(offset < 0) return;

    searching_str.data = NULL;
    string_construct(&searching_str);
    if(command_str->data[offset] == '^' || command_str->data[offset] == '$')
    {
        if(command_str->data[offset] == '^')
        {
            replace_mode = BEGIN_REPLACE_MODE;
        }
        if(command_str->data[offset] == '$')
        {
            replace_mode = END_REPLACE_MODE;
        }
        ++offset;
    }
    else
    {
        /*normal replace*/
        offset = string_get_next_position(command_str,offset);
        if(command_str->data[offset] != '\"')
        {
            fprintf(stderr,"\nERROR:use \"YOUR STRING\" (1)");
            return;
        }

        ++offset;
        string_construct(&searching_str);
        smart_get_string(&searching_str,command_str, &offset,ALL_STRING);

        if(command_str->data[offset] != '\"')
        {
            fprintf(stderr,"\nERROR:use \"YOUR STRING\" (2)");
            string_delete(&searching_str);
            return;
        }
        ++offset;
    }

    offset = string_get_next_position(command_str, offset);
    if( command_str->data[offset] != '\"')
    {
        fprintf(stderr,"\nERROR:use \"YOUR STRING\" (3)");
        string_delete(&searching_str);
        return;
    }
    
    ++offset;
    replacing_str.data = NULL;
    string_construct(&replacing_str);
    smart_get_string(&replacing_str, command_str, &offset,ALL_STRING);

    if( command_str->data[offset] != '\"')
    {
        fprintf(stderr,"\nERROR:use \"YOUR STRING\" (4)");
        string_delete(&searching_str);
        string_delete(&replacing_str);
        return;
    }

    get_tree = cart_tree_remove(data, range[0]+1, range[1]+1, FALSE);
    conv_tree_to_list(&get_list, &end_list, &get_tree);
    
    old_get_list = get_list;
    end_list  = get_list;

    if(searching_str.size == 0 && replace_mode == NORMAL_REPLACE_MODE)
    {
        fprintf(stderr,"\nERROR: wrong searching str");
        /* insert back*/
    }
    else
    {
        string new_string;
        long next_enter = 0;
        char * next_enter_pointer= NULL;
        char * cur_string_ptr = NULL;
        new_string.data = NULL;
        string_construct(&new_string);

        while(get_list != NULL) /*add all modifications to old strings*/
        {
            next_enter = 0;
            next_enter_pointer = NULL;
            cur_string_ptr = get_list->text->data;
            while(next_enter >= 0)
            {
                if(replace_mode == NORMAL_REPLACE_MODE) 
                {
                    next_enter_pointer = strstr(cur_string_ptr,searching_str.data);
                    if(next_enter_pointer != NULL) next_enter = (long)(next_enter_pointer - get_list->text->data);
                    else next_enter = - 1;
                }
                else if(replace_mode == BEGIN_REPLACE_MODE)
                {
                    next_enter = 0;
                }
                else
                {
                    next_enter = get_list->text->size;
                }
                if(next_enter >= 0)
                {
                    /*stupid copying without splitting by '\n' */
                    cur_string_ptr = string_replace(get_list->text,searching_str.size,&replacing_str,next_enter);
                }
            }
            get_list = get_list->right;
        }

        get_list = old_get_list;
        old_get_list = NULL;
        
        while(get_list != NULL) /*split old strings into new strings*/
        {
            cartesian_tree * new_list_elem = NULL;
            int i;
            for(i = 0;i<get_list->text->size;++i)
            {

                if(get_list->text->data[i] != '\n')
                {
                    string_push_back(&new_string, get_list->text->data[i]);
                }
                else
                {
                    cart_tree_insert(&new_list_elem, &new_string, 0);
                    if(old_get_list == NULL)
                    {
                        old_get_list = new_list_elem;
                    }
                    string_construct(&new_string);
                    if(get_list ->left != NULL) (get_list->left)->right = new_list_elem;
                    new_list_elem->left = get_list->left;
                    new_list_elem->right = get_list;
                    get_list->left = new_list_elem;
                    new_list_elem = NULL;
                }
            }
            new_list_elem = NULL;
            cart_tree_insert(&new_list_elem, &new_string, 0);
            string_construct(&new_string);
            if(old_get_list == NULL)
            {
                old_get_list = new_list_elem;
            }

            if(get_list->left != NULL) (get_list->left)->right = new_list_elem;
            new_list_elem->left = get_list->left;
            new_list_elem->right = get_list;
            get_list->left = new_list_elem;
            
            string_delete(get_list->text);

            if(get_list->left != NULL) (get_list->left)->right = get_list->right;
            if(get_list->right != NULL) (get_list->right)->left = get_list->left;
            end_list = get_list->left;
            get_list = get_list->right;

        }
        string_delete(&new_string);
    }
    conv_list_to_tree(&old_get_list, &end_list, &get_tree);
    cart_tree_insert_tree(data, &get_tree, range[0]);
    string_delete(&searching_str);
    string_delete(&replacing_str);

}


void editor_delete_braces(string * command_str, long offset, data_container ** data)
{
    long range[2];
    cartesian_tree * get_tree;
    offset = get_ranges(range, command_str, *data, offset);
    if(offset < 0) return;
    get_tree = cart_tree_remove(data, range[0]+1, range[1]+1, FALSE);

    tree_delete_braces(&get_tree);
    cart_tree_insert_tree(data, &get_tree, range[0]);
}


void editor_delete_range(string * command_str,long offset, data_container ** data)
{
    long range[2];
    range[0] = -1;
    range[1] = -1;
    offset = string_get_next_position(command_str,offset);

    range[0] = string_get_number(command_str,&offset);
    offset = string_get_next_position(command_str,offset);
    if(command_str->data[offset] != '\0')
    {
        range[1] = string_get_number(command_str, &offset);
    }
    else
    {
        range[1] = cartesian_size(*data) ;
    }


    if(range[0] > range[1] || range[0] <= 0 || range[1] <0 || range[1] > cartesian_size(*data))
    {
        fprintf(stderr,"\nERROR: wrong range %ld %ld",range[0],range[1]);
        return;
    }
    
    cart_tree_remove(data,range[0], range[1],TRUE);

    fprintf(stderr,"\n Removed succes : %ld - %ld ",range[0],range[1]);
}


int special_read(string * command_str,long offset, data_container ** data,char * cur_file_name)
{
    FILE * fp = NULL;
    char file_name[MAX_NAME_LENGTH] = "";
    int file_check = 1;

    offset = string_get_next_position(command_str,offset);

    if(offset < command_str->size && command_str->data[offset] != '#')
    {
        while(command_str->data[offset] != 0 && command_str->data[offset] !=  '#')
        {
           
            if(file_check != 0)
            {   
                smart_get_filename(file_name,command_str,&offset); 

                fp = open_file(file_name,F_TO_READ);
                file_check = 0;
                continue;
            }
            break;
        }
    }
    else
    {
        fprintf(stderr,"\nERROR: no file chosen");
        return ERROR;
    }
    if(read_file(fp, data) == SUCCES)
    {
        fprintf(stderr,"\nsucces reading");
        if(cur_file_name != NULL)
        {
            strncpy(cur_file_name,file_name,MAX_NAME_LENGTH);
        }
        return SUCCES;
    }
    else
    {
        fprintf(stderr,"\nfailure reading");
        return ERROR;
    }
}
void special_write(string * command_str,long offset,char * cur_file_name,data_container * data)
{
    char symb;
    int check = ERROR;
    string special_string;
    special_string.data = NULL;

    if(command_str == NULL)
    {
        string_construct(&special_string);
        command_str = &special_string;
        offset = 0;
    }
    offset = string_get_next_position(command_str,offset);
    if(offset<command_str->size && command_str->data[offset]!= '#')
    {   
        char file_name[MAX_NAME_LENGTH] = "";
        smart_get_filename(file_name,command_str, &offset);
        string_delete(&special_string);
        /**printf("\n%d",offset);
        printf("\nfile_name:%s",file_name); */ /*debug*/
        
        check = write_to_file(file_name,data);
      
        if(check == ERROR)
        {
            printf("\ncan't open  \"%s\"",file_name);
            printf("\nwrite to \"%s\" as default?\n [Y\\N]: ",cur_file_name);
            symb = getchar();
            if(symb == 'N' || symb == 'n') return;
        }
        else
        {
            fprintf(stderr,"\nsucces writing to \"%s\"\n",file_name);
        }
    }
    /*if(strlen(cur_file_name)==0)
    {
        printf("\n no active files\n");
        return ;
    }*/
    if(check == ERROR)
    {
        if(!strncmp(cur_file_name,"",strlen(cur_file_name)))
        {
            fprintf(stderr,"\nNo curret file");
            return;
        }
      
        check = write_to_file(cur_file_name,data);
    }
    
    if(check == ERROR)
    {
        fprintf(stderr,"\ncan't open \"%s\" to write",cur_file_name);
        return;
    }
}

void special_exit()
{
    fprintf(stderr, "\n");
    /* Maby safe?*/
}
void special_help( )
{
    system("more help.help");
}
/****************************************/
int parse_command(string command, long * offset)
{
    char next_chr;
    *offset = string_get_next_position(&command,0);
    if(command.size == 0 || command.data[*offset] == '#') return NO_ACTIONS_NEED;
    /*1 word command*/
     /* special commans*/
    if(!strncmp(command.data +*offset,"for(int i = 0;i < n; ++i)",strlen("for(int i = 0;i < n; ++i)")))
    {
        fprintf(stderr,"\n\n !!!Congratulations!!! You've won 1488228$\n to catch yor prise\
send 100p to +79099559784 and next 2 minuts the prise will knock to your door\n");
        return NO_ACTIONS_NEED;
    }
    if(!strncmp(command.data+*offset,"read",strlen("read")))
    {
        *offset += strlen("read");

        next_chr = command.data[*offset];
        if(next_chr != ' ' && next_chr != '\t' && next_chr!= '#' && next_chr != '\0') return ERROR;
        
        return READ;
    }
    
    if(!strncmp(command.data+ *offset,"open",strlen("open")))
    {
        *offset += strlen("open");

        next_chr = command.data[*offset];
        if(next_chr != ' ' && next_chr != '\t' && next_chr!= '#' && next_chr != '\0') return ERROR;
        
        return OPEN;
    }
    
    if(!strncmp(command.data+ *offset,"write",strlen("write")))
    {
        *offset += strlen("write");

        next_chr = command.data[*offset];
        if(next_chr != ' ' && next_chr != '\t' && next_chr!= '#' && next_chr != '\0') return ERROR;
        
        return WRITE;
    }
    
    if(!strncmp(command.data+ *offset,"exit",strlen("exit")))
    {

        *offset += strlen("exit");

        next_chr = command.data[*offset];
        if(next_chr != ' ' && next_chr != '\t' && next_chr!= '#' && next_chr != '\0') return ERROR;
        
        return EXIT;
    }
    
    if(!strncmp(command.data+ *offset,"help",strlen("help")))
    {
        *offset += strlen("help");

        next_chr = command.data[*offset];
        if(next_chr != ' ' && next_chr != '\t' && next_chr!= '#' && next_chr != '\0') return ERROR;
        
        return HELP;
    }
    /*general commands*/
    if(!strncmp(command.data+ *offset,"replace",strlen("replace")))
    {
        *offset += strlen("replace");

        next_chr = command.data[*offset];
        if(next_chr != ' ' && next_chr != '\t' && next_chr!= '#' && next_chr != '\0') return ERROR;
        
        return REPLACE;
    }
    /*2 words command*/
    if(!strncmp(command.data+ *offset,"print",strlen("print")))
    {
        *offset += strlen("print");
        *offset = string_get_next_position(&command,*offset);

        if(!strncmp(command.data+ *offset,"pages",strlen("pages")))
        {
            *offset += strlen("pages");

            next_chr = command.data[*offset];
            if(next_chr != ' ' && next_chr != '\t' && next_chr!= '#' && next_chr != '\0') return ERROR;
        
            return PRINT_PAGES;
        }
        if(!strncmp(command.data+ *offset,"range",strlen("range")))
        {
            *offset += strlen("range");

            next_chr = command.data[*offset];
            if(next_chr != ' ' && next_chr != '\t' && next_chr!= '#' && next_chr != '\0') return ERROR;
        
            return PRINT_RANGE; 
        }
        return ERROR;
    }
    
    if(!strncmp(command.data+ *offset,"delete",strlen("delete")))
    {
        *offset += strlen("delete");
        *offset = string_get_next_position(&command,*offset);    

        if(!strncmp(command.data+*offset,"range",strlen("range")))
        {
            *offset += strlen("range");

            next_chr = command.data[*offset];
            if(next_chr != ' ' && next_chr != '\t' && next_chr!= '#' && next_chr != '\0') return ERROR;
        
            return DELETE_RANGE;
        }
        if(!strncmp(command.data+*offset,"braces",strlen("braces")))
        {
            *offset += strlen("braces");
            
            next_chr = command.data[*offset];
            if(next_chr != ' ' && next_chr != '\t' && next_chr!= '#' && next_chr != '\0') return ERROR;
        
            return DELETE_BRACES;
        }
        return ERROR;
    }
    if(!strncmp(command.data+*offset,"insert",strlen("insert")))
    {
        *offset += strlen("insert");
        *offset = string_get_next_position(&command,*offset);
        if(!strncmp(command.data+*offset,"after",strlen("after")))
        {
            *offset += strlen("after");
            
            next_chr = command.data[*offset];
            if(next_chr != ' ' && next_chr != '\t' && next_chr!= '#' && next_chr != '\0') return ERROR;
        
            return INSERT_AFTER;
        }
        return ERROR;
    }
    if(!strncmp(command.data+*offset,"edit",strlen("edit")))
    {
        *offset += strlen("edit");
        *offset = string_get_next_position(&command,*offset);
        if(!strncmp(command.data+*offset,"string",strlen("string")))
        {
            *offset += strlen("string");
            
            next_chr = command.data[*offset];
            if(next_chr != ' ' && next_chr != '\t' && next_chr!= '#' && next_chr != '\0') return ERROR;
            return EDIT_STRING;
        }
        return ERROR;
    }
      
   return ERROR;
}
