#include "../include/string_arrayADT.h"
#include "../include/syscall.h"
#include "../include/libc.h"


struct string_arrayCDT {
    char * array;
    uint64_t written;
    uint64_t max_size;
    uint16_t * offsets;
    uint32_t count;
    int64_t it;
};

string_arrayADT start_string_array(uint64_t max_size) {
    if (!max_size) {
        return (void*)0;
    }
    
    string_arrayADT rta = my_malloc(sizeof(struct string_arrayCDT));
    rta->written = 0;
    rta->max_size = max_size;
    rta->array = my_malloc(max_size * sizeof(char));
    rta->array[0] = 0;
    rta->offsets = my_malloc(max_size * sizeof(uint16_t));
    rta->count = 0;
    rta->offsets[0] = 0;
    return rta;
}

int8_t add(string_arrayADT arr, char * string, uint32_t len) {
    if (arr->written + len >= arr->max_size) {
        return -1;
    }
    uint32_t count = strcpy(&arr->array[arr->offsets[arr->count++]], string);
    /* char * dest = &arr->array[arr->offsets[arr->count++]], * source = string;
    uint32_t count = 0;
    while (source[count]) {
        dest[count] = source[count];
        count++;
    }
    dest[count] = 0; */
    arr->written += count + 1;
    arr->array[arr->written] = 0; //el ultimo debe apuntar a un string vacío
    arr->offsets[arr->count] = arr->written;
    return len - count;
}

int8_t to_begin(string_arrayADT arr) {
    if (arr->count == 0) {
        return -1;
    }
    arr->it = arr->count - 1;
    return 1;
}

int8_t has_next(string_arrayADT arr) {
    return arr->it < arr->count;
}

char * next(string_arrayADT arr) {
    if (!has_next(arr)){
        return (void*) 0;
    }
    arr->it++;
    return &arr->array[arr->offsets[arr->it - 1]];
}

int8_t has_previous(string_arrayADT arr) {
    return arr->it > 0;
}

char * previous(string_arrayADT arr) {
     if (!has_previous(arr)){
        return (void*) 0;
    }
    return &arr->array[arr->offsets[arr->it--]];
}

void free_string_array(string_arrayADT arr) {
    my_free(arr->offsets);
    my_free(arr->array);
    my_free(arr);
}

void reset_string_array(string_arrayADT rta) {
    rta->written = 0;
    rta->array[0] = 0;
    rta->count = 0;
    rta->offsets[0] = 0;    
}

int test() {
    string_arrayADT test = start_string_array(4096);
    uint8_t count = 10;
    char * h = "hola";
    for (uint8_t i = 0; i < count; i++) {
        add(test, h, 4);
    }
    to_begin(test);
    char * aux;
    while(has_previous(test)){
        aux = previous(test);
        printf(aux);
    }
    while (has_next(test)) {
        aux = next(test);
        printf(aux);
    }
    
    free_string_array(test);
    return 0;
}