#ifndef STRING_ARRAY_H
#define STRING_ARRAY_H

#include <stdint.h>

typedef struct string_arrayCDT *string_arrayADT;
string_arrayADT start_string_array(uint64_t max_size);
int8_t add(string_arrayADT arr, char *string, uint32_t len);
int8_t to_begin(string_arrayADT arr);
int8_t has_next(string_arrayADT arr);
char *next(string_arrayADT arr, uint16_t *len);
int8_t has_previous(string_arrayADT arr);
char *previous(string_arrayADT arr, uint16_t *len);
void free_string_array(string_arrayADT arr);
void reset_string_array(string_arrayADT arr);

#endif