
/**auxiliary library base lines with support for string operations*/
#ifndef __MY_STR_DEF__
#define __MY_STR_DEF__
#include <stdlib.h>
#include <stdio.h>
typedef struct string string_t;
struct string
{
    size_t full_size;
    size_t size;
    short use_char_type;
    char* data;
};

void string_construct(struct string * str);
char* string_begin(struct string* str);
void string_big_push(struct string* str, char* m_push,size_t size);
void string_push_back(struct string* str, char c);
void string_pop_back(struct string* str);
void string_insert(struct string* str, char c, size_t index);
void string_assign(struct string* str, char c, size_t index);
void string_remove(struct string* str, size_t index);
void string_print(FILE * fout,struct string* str);
void string_delete(struct string* str);

#endif
