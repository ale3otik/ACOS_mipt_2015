#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "my_types.h"

void string_construct(struct string* _str)
{
	if(_str->data != NULL)
    {
       /*free(_str->data);*/
    }
    if(!(_str->data = (char*)malloc(START_STRING_SIZE * sizeof(char))))
    {
        fprintf(stderr,"malloc_error");
        exit(1);/*default data array with 32 empty elements*/
    }
    _str->size = 0;
    _str->data[0] = 0;
    _str->capacity = START_STRING_SIZE;/**the real number of allocated memory*/
}

void string_big_push(struct string* _str, char* push_str,long size)
{
    if(size!= 0 && push_str[size - 1] == '\n')
    { 
        size--;
    }
    _str->size += size;
    /*printf("\n_str->size: %d <%s>\n",_str->size,push_str);*/
    if(_str->size >= _str->capacity)
    {
        while(_str->size >= _str->capacity)
        { 
            if(_str->capacity == 0) _str->capacity = 1;
            _str->capacity*= 2;
        }
        
        if(!(_str->data = (char*)realloc(_str->data, _str->capacity * sizeof(char))))
        {
            printf("\nERROR:realloc_error %ld\n",_str->capacity);
            exit(2);
        }
    }
    strncat(_str->data,push_str,size);
}

void string_push_back(string * _str, char chr)
{
    /**unusual case of adding the element*/
    ++_str->size;
    if(_str->size >= _str->capacity)
    {
    /**reallocation of memory when storage is full**/
        _str->capacity *= 2;
        if(!(_str->data = (char*)realloc(_str->data, _str->capacity * sizeof(char))))
        {
            printf("\nrealloc_error %ld\n",_str->capacity);
            exit(2);
        }
    }
    /**insert last element**/
    *(_str->data + _str->size-1) = chr;
    *(_str->data + _str->size) = 0;
}

void string_assign(struct string* _str, char c, long index)/**edit element in index position*/
{
    if(index>=_str->size)
    {
        fprintf(stderr,"\nstring assign wrong index %lu\n",index);
        exit(1);
    }
    *(_str->data + index) = c;
}

void string_insert(struct string* _str, char c, long index)
{
    long i = _str->size;
    string_push_back(_str, '0');
    for(;i > index; --i)/**shift of elements from [index,size) in right*/
        *(_str->data + i) = *(_str->data + i - 1);
    
    string_assign(_str, c, index);
}

void string_pop_back(struct string* _str)
{
/**reallocation of memory when size less than quarter of capacity*/
    if(_str->size * 4 <= _str->capacity && _str->capacity > START_STRING_SIZE)/**but capacity can not to be less then 32*/
    {
        _str->capacity /= 2;
        if(!(_str->data = (char*)realloc(_str->data, _str->capacity * sizeof(char))))
        {
            printf("\nERROR:realloc_error %ld",_str->capacity);
            exit(2);
        }
    }
        --_str->size;/**decreasing size*/
        _str->data[_str->size] = 0; /* MAY BE WRONG*/
}

void string_remove(struct string* _str, long index)
{
    long i = index + 1;
    for(;i < _str->size;++i)
        *(_str->data + i - 1) = *(_str->data + i);/**shift of elements from [index,size) in left*/

    string_pop_back(_str);
}
void string_cpy(struct string * source, struct string * dest, long left,long right)
{
    source->data = NULL;
    string_construct(source);

    for(;left<=right;++left)
    {
        string_push_back(source, dest->data[left]);   
    }
}

char * string_replace(string * source_str,long size,string * replace_str,long position)
{
    long i,k;
    string new_string;
    if(replace_str == NULL)
    {   
        new_string.data = NULL;
        string_construct(&new_string);
        replace_str = &new_string;
    }
    if(size > replace_str->size)
    {
        for(i = position,k = 0;i<position + replace_str->size;++i,++k)
        {
            source_str->data[i] = replace_str->data[k];
        }
        for(;i<source_str->size;++i)
        {
            source_str->data[i] = source_str->data[i+size - replace_str->size];
        }
        for(i = replace_str->size; i <size;++i)
        {
            string_pop_back(source_str);
        }
    }
    else
    {
        for(i = 0; i< replace_str->size - size;++i)
        {
            string_push_back(source_str, '*');
        }
        for(i = source_str->size - 1;i>= position + replace_str->size;--i)
        {
            source_str->data[i] = source_str->data[i - (replace_str->size - size)]; 
        }
        for(i = position,k = 0;i<position + replace_str->size;++i,++k)
        {
            source_str->data[i] = replace_str->data[k];
        }
    }
    return source_str->data + i;
}

void string_print(FILE * fout,struct string* _str)
{
    if(_str == NULL)
    {
        return;
    }
    fprintf(fout,"%s",_str->data);
    fprintf(fout,"\n");
}

void string_delete(struct string* str)
{
    if(str->data != NULL)
        free(str->data);
}

