#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include "myTypes.h"

int parse_command(string,size_t *);

void editor_print_pages(data_container * data);
void editor_print_range(string * command_str,size_t offset, data_container ** data);
void editor_insert_after(string* command,size_t offset,data_container ** data);
void editor_edit_string(string * command,size_t offset,data_container ** data);
void editor_replace();
void editor_delete_range(string * command_str,size_t offset, data_container ** data);
void editor_delete_braces();
int special_read(string * command_str,size_t offset,data_container ** data,params_of_openfile * params, char * cur_filen);
void special_write(string * command_str,size_t,char *cur_file_name,data_container * data,params_of_openfile * params);
void special_exit();
void special_help();

int min(int a,int b)
{
    if(a<b) return a;
    return b;
}

int general_function(char* cur_file_name,data_container * data,params_of_openfile * params)
{
    string command_str;
    int entered_command;
    size_t offset;
    command_str.data = NULL;
    system("clear"); /*clear screen*/
    /*print_tree(stdout,data);*/  
    while(1)
    {
        string_construct(&command_str);
        printf("\n\n>>editor:");

        string_get(stdin,&command_str); /*read command*/

        string_print(stdout,&command_str);
        
        offset= 0;
        entered_command = parse_command(command_str,&offset);
        
        switch (entered_command)
        {
            case PRINT_PAGES:
                editor_print_pages(data);
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
                editor_replace();
                break;
            case DELETE_RANGE :
                editor_delete_range(&command_str,offset,&data);
                break;
            case DELETE_BRACES :
                editor_delete_braces();
                break;
            
            case READ :
                special_read(&command_str,offset,&data,NULL,NULL);
                break;
            case OPEN :
                special_read(&command_str,offset, &data,params,cur_file_name);
                break;
            case WRITE :
                special_write(&command_str,offset,cur_file_name,data,params);
                break;
            case EXIT :
                special_exit();
                return EXIT;
                break;
            case HELP :
                special_help();
                break;
            default:
                fprintf(stderr,"unknown command: \"%s\"\n",command_str.data);
        }
    }
    return 0;
}

void editor_print_pages(data_container * data)
{
    struct termios old_term,new_term;
    char ch;
    winsize window_size;  /* to get buffer size */
    int check = 0;

    print_tree(stdout,data);

    ioctl(1,TIOCGWINSZ,&window_size);

    tcgetattr( STDIN_FILENO, &old_term);
    new_term = old_term;
    new_term.c_lflag &= ~( ICANON | ECHO );
    while(1)
    {
        /*check = smart_print_tree(stdout, *data, &window_size);*/
        if(check == 1)
        {
        tcsetattr( STDIN_FILENO, TCSANOW, &new_term );
            while(1)
            {
                ch = getchar();
                if(ch == 'Q' || ch == 'q')
                {
                    tcsetattr( STDIN_FILENO, TCSANOW, &old_term );
                    system("clear");
                    return;
                }
                if(ch == ' ') 
                {
                    tcsetattr( STDIN_FILENO, TCSANOW, &old_term );
                    break;
                }
            }
        }
        else
        {
            tcsetattr( STDIN_FILENO, TCSANOW, &old_term );
            return;
        }
    }
}
void editor_print_range(string * command_str,size_t offset, data_container ** data)
{

}
void editor_insert_after(string* command,size_t offset,data_container ** data)
{
    int position = -1;
    int count_strings = 0;
    offset = string_get_next_position(command,offset);
    position = string_get_number(command, &offset);
    if(position == -1) 
        {
            position = cartesian_size(*data);
        }
    if(position < 0 || position > cartesian_size(*data))
    {
        fprintf(stderr, "\nERROR: wrong position %d ",position);
        return;
    }

    offset = string_get_next_position(command,offset);
    if(command->data[offset] != '\"')
    {
        fprintf(stderr,"\nNo strings added. Use \"YOUR_ADDED_STR\" to insert str");
        return;
    }

    string  inserting_string;
    inserting_string.data = NULL;
    string_construct(&inserting_string);

    ++offset;
    if(command->data[offset] == '\"')
    {
        ++offset;
        if(command->data[offset] == '\"') /* insert several strings*/
        {
            ++offset;
            while(1)
            {
                smart_get_string(&inserting_string, command, &offset);
                if(command->data[offset] == '\0')
                {
                    cart_tree_insert(data,&inserting_string,position);

                    string_construct(command);
                    string_construct(&inserting_string);
                    
                    offset = 0;
                    ++count_strings;
                    ++position;
                    fprintf(stdout,"\nstr %d: ",count_strings+1);
                    string_get(stdin,command); /*read command*/

                    continue;
                }
                else
                {
                    if(command->data[offset] == '\"')
                    {
                        ++offset;
                        if(command->data[offset] == '\"')
                        {
                            ++offset;
                            if(command->data[offset] == '\"')
                            {
                                cart_tree_insert(data,&inserting_string,position);
                                break;
                            }
                            string_push_back(&inserting_string,'\"');
                        }
                        string_push_back(&inserting_string,'\"');
                    }
                }
            }
            string_delete(&inserting_string);

            fprintf(stderr,"\ninserted %d succes",count_strings+1);
            return;
        }
        --offset;
    }

    while(1)
    {
        smart_get_string(&inserting_string, command,&offset);
        cart_tree_insert(data, &inserting_string, position);
        if(command->data[offset] == 'n')
        {       
            ++position;
            string_construct(&inserting_string);
            ++offset;
            ++count_strings;

            continue;
        }
        string_delete(&inserting_string);

        fprintf(stderr,"\ninserted %d succes",count_strings+1);
        return;
    }
}
void editor_edit_string(string * command,size_t offset, data_container ** data)
{
    #define _EDIT_INSERT 1
    #define _EDIT_DELETE 2
    #define _EDIT_REPLACE 3

    int array_position = -1;
    int string_position = -1;
    char * name_commad = NULL;
    int number_of_operation = ERROR;

    string inserting_string;
    string * finding_string = NULL;

    inserting_string.data = NULL;
    string_construct(&inserting_string);

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
        string_delete(&inserting_string);
        return;
    }

    if(array_position < 0 || array_position > cartesian_size(*data))
    {
        fprintf(stderr,"\nERROR:wrong array_position %d ", array_position);
        string_delete(&inserting_string);
        return;
    }

    cart_tree_access(data, array_position, &finding_string);
    if(string_position <  0)
    {
        string_position = finding_string->size;
    }

    offset = string_get_next_position(command, offset);
    if(command->data[offset] != '\"')
    {
        fprintf(stderr,"\nERROR: use \"YOUR_INSERTING_STRING\"");
        string_delete(&inserting_string);
        return;
    }
    smart_get_string(&inserting_string, command, &offset);
    if(command->data[offset] == '\0')
    {
        fprintf(stderr,"\nERROR: use \"YOUR_INSERTING_STRING\"");
    }
    else
    {
        int index = 0;
        string string_to_push_in_arr;
        string_to_push_in_arr.data = NULL;
        string_construct(&string_to_push_in_arr);
        while(inserting_string.data[index] != 0)
        {
            string_insert(finding_string, inserting_string.data[index],string_position);
            ++index;
            ++string_position;
        }
        if(command->data[offset] != '\"')
        {
            /*string_cpy();*/
        }

    }
    string_delete(&inserting_string);   
    /**/

}   
void editor_replace()
{
    
}
void editor_delete_range(string * command_str,size_t offset, data_container ** data)
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


    if(range[0] > range[1] || range[0] < 0 || range[1] <0 || range[1] > cartesian_size(*data))
    {
        fprintf(stderr,"\nERROR: wrong range %ld %ld",range[0],range[1]);
        return;
    }
    
    cart_tree_remove(data,range[0], range[1]);

    fprintf(stderr,"\n Removed succes : %ld - %ld ",range[0],range[1]);
}
void editor_delete_braces()
{
    
}
int special_read(string * command_str,size_t offset, data_container ** data, params_of_openfile *cur_params,char * cur_file_name)
{
    FILE * fp = NULL;
    char file_name[MAX_NAME_LENGTH] = "";
    int file_check = 1;
    params_of_openfile params;
    initialization_params(&params); /* default settings*/

    offset = string_get_next_position(command_str,offset);

    if(offset < command_str->size && command_str->data[offset] != '#')
    {
        while(command_str->data[offset] != 0 && command_str->data[offset] !=  '#')
        {
            if(command_str->data[offset] == '-')
            {
                parse_key(command_str->data+offset,&params);
            }
            else
            { 
                if(file_check != 0)
                {   
                    smart_get_filename(file_name,command_str,&offset); 

                    fp = open_file(file_name,&params,F_TO_READ);
                    file_check = 0;
                    continue;
                }
                break;
            }
            
        }
    }
    else
    {
        fprintf(stderr,"\nERROR: no file chosen");
        return ERROR;
    }
    if(read_file(fp, data, &params) == SUCCES)
    {
        fprintf(stderr,"\nsucces reading");
        if(cur_file_name != NULL && cur_params != NULL)
        {
            strncpy(cur_file_name,file_name,MAX_NAME_LENGTH);
            *cur_params = params;
        }
        return SUCCES;
    }
    else
    {
        return ERROR;
        fprintf(stderr,"\nfailure reading");
    }
}
void special_write(string * command_str,size_t offset,char * cur_file_name,data_container * data,params_of_openfile * params)
{
    char symb;
    int check = ERROR;
    offset = string_get_next_position(command_str,offset);
    if(offset<command_str->size && command_str->data[offset]!= '#')
    {   
        char file_name[MAX_NAME_LENGTH] = "";
        smart_get_filename(file_name,command_str, &offset);
        
        /**printf("\n%d",offset);
        printf("\nfile_name:%s",file_name); */ /*debug*/
        
        check = write_to_file(file_name,params,data);
      
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
      
        check = write_to_file(cur_file_name,params,data);
    }
    
    if(check == ERROR)
    {
        fprintf(stderr,"\ncan't open \"%s\" to write",cur_file_name);
        return;
    }

}

void special_exit()
{
    /* Maby safe?*/
}
void special_help( )
{
    
}
/****************************************/
int parse_command(string command, size_t * offset)
{
    char next_chr;
    *offset = string_get_next_position(&command,0);
    /*1 word command*/
     /* special commans*/
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
