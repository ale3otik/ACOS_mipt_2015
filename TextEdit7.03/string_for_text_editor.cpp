#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "myTypes.h"

const size_t MIN_FULL_SIZE_OF_STRING = 32;

void string_construct(struct string* _str)
{
	if(_str->data != NULL)
    {
       /* free(_str->data);*/
    }
    if(!(_str->data = (char*)malloc(MIN_FULL_SIZE_OF_STRING * sizeof(char))))
    {
        printf("malloc_error");
        exit(1);/**default data array with 32 empty elements*/
    }
    _str->size = 0;
    _str->data[0] = 0;
    _str->full_size = MIN_FULL_SIZE_OF_STRING;/**the real number of allocated memory*/
    /**printf("%lu %lu\n",s->size,s->full_size);*/
}

void string_big_push(struct string* _str, char* m_push,size_t size)
{
    if(size!= 0 && m_push[size - 1] == '\n')
    { 
   /*     printf("!!!YEA");*/
        size--;
    }
    _str->size += size;
    /*printf("\n_str->size: %d <%s>\n",_str->size,m_push);*/
    if(_str->size >= _str->full_size)
    {
        while(_str->size >= _str->full_size)
        { 
            if(_str->full_size == 0) _str->full_size = 1;
            _str->full_size*= 2;
        }
        
        if(!(_str->data = (char*)realloc(_str->data, _str->full_size)))
        {
            printf("\nrealloc_error %ld\n",_str->full_size);
            exit(2);
        }
    }
    strncat(_str->data,m_push,size);
}

void string_push_back(string * _str, char chr)
{
    /**unusual case of adding the element*/
    ++_str->size;
    if(_str->size >= _str->full_size)
    {
    /**reallocation of memory when storage is full**/
        _str->full_size *= 2;
        if(!(_str->data = (char*)realloc(_str->data, _str->full_size)))
        {
            printf("\nrealloc_error %ld\n",_str->full_size);
            exit(2);
        }
    }
    /**insert last element**/
    *(_str->data + _str->size-1) = chr;
    *(_str->data + _str->size) = 0;
}
void HIDEstring_push_back(struct string* _str, char c)
{
    if(_str->size < _str->full_size) /**usual case of adding the element*/
    {
        if(_str->size!= 0 && _str->data[_str->size - 1] == 0) _str->size--;
        *(_str->data + _str->size) = c;
    }
    else/**reallocation of memory when storage is full*/
    {
        _str->full_size *= 2;
        if(!(_str->data = (char*)realloc(_str->data, _str->full_size)))
        {
            printf("\nrealloc_error %ld\n",_str->full_size);
            exit(2);
        }
    }

    ++_str->size;/**increasing size*/
}

void string_assign(struct string* _str, char c, size_t index)/**edit element in index position*/
{
    if(index>=_str->size)
    {
        fprintf(stderr,"\nstring assign wrong index %lu\n",index);
        exit(1);
    }
    *(_str->data + index) = c;
}

void string_insert(struct string* _str, char c, size_t index)
{
    size_t i = _str->size;
    string_push_back(_str, '0');
    for(;i > index; --i)/**shift of elements from [index,size) in right*/
        *(_str->data + i) = *(_str->data + i - 1);
    
    string_assign(_str, c, index);
}

void string_pop_back(struct string* _str)
{
/**reallocation of memory when size less than quarter of full_size*/
    if(_str->size * 4 <= _str->full_size && _str->full_size > MIN_FULL_SIZE_OF_STRING)/**but full_size can not to be less then 32*/
    {
        _str->full_size /= 2;
        if(!(_str->data = (char*)realloc(_str->data, _str->full_size)))
        {
            printf("realloc_error %ld",_str->full_size);
            exit(2);
        }
    }
        --_str->size;/**decreasing size*/
        _str->data[_str->size] = 0; /* MAY BE WRONG*/
}

void string_remove(struct string* _str, size_t index)
{
    size_t i = index + 1;
    for(;i < _str->size;++i)
        *(_str->data + i - 1) = *(_str->data + i);/**shift of elements from [index,size) in left*/

    string_pop_back(_str);
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
    free(str->data);
}

