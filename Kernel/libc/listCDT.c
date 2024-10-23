#include "../include/listADT.h"
//@todo: switch case type
typedef struct node {
    void * head;
    struct node * tail;
} node;

typedef node * TList;

struct listCDT {
    TList first;
    TList last;
    compare cmp;
    TList it;
};

listADT new_list(compare cmp) {
    listADT aux = my_malloc(sizeof(struct listCDT));
    aux->cmp = cmp;
    aux->first = NULL;
    aux->last = NULL;
    aux->it = NULL;
    return aux;
}

static void free_list_rec(TList list) {
    if (list==NULL) {
        return;
    }
    free_list_rec(list->tail);
    my_free(list);
}

void add(listADT list, void * elem) {
    TList aux = my_malloc(sizeof(node));
    aux->tail = NULL;
    aux->head = elem;
    if(list->first == NULL) {
        list->first = aux;
        list->last = aux;
        to_begin(list);
    } else {
        list->last->tail = aux;
        list->last = aux;
    }
}

void free_list(listADT list) {
    free_list_rec(list->first);
    my_free(list);
}

void to_begin(listADT list) {
    list->it = list->first;
}

uint8_t has_next(const listADT list) {
    return list->it != NULL;
}

void * next(listADT list) {
    if(!has_next(list)) {
        return list->first;
    }
    void * aux = list->it->head;
    list->it = list->it->tail;
    return aux;
}

void * peek(listADT list) {
    return list->it->head;
}

void * poll(listADT list){
    TList aux = list->first;
    list->first = list->first->tail;
    my_free(aux);
    return aux->head;
}

void queue(listADT list , void * elem){
    TList aux = my_malloc(sizeof(node));
    aux->head = elem;
    if(list->first == NULL) {
        list->first = aux;
        list->last = aux;
        to_begin(list);
    } else {
        list->last->tail = aux;
        list->last = aux;
    }
}