
/**auxiliary library base lines with support for string operations*/
#ifndef __MY_STR_DEF__
#define __MY_STR_DEF__
#include <stdlib.h>
#include <stdio.h>
typedef struct string string_t;
struct string
{
    long full_size;
    long size;
    short use_char_type;
    char* data;
};

void string_construct(struct string * str);
char* string_begin(struct string* str);
void string_big_push(struct string* str, char* m_push,long size);
void string_push_back(struct string* str, char c);
void string_pop_back(struct string* str);
void string_insert(struct string* str, char c, long index);
void string_assign(struct string* str, char c, long index);
void string_remove(struct string* str, long index);
void string_cpy(struct string * source, struct string * dest, long index);
void string_print(FILE * fout,struct string* str);
void string_delete(struct string* str);
char * string_replace(string * source_str,long size,string * replace_str,long position);
#endif
