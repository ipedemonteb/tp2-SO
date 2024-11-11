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
    uint64_t size;
};

listADT new_list(compare cmp) {
    listADT aux = my_malloc(sizeof(struct listCDT));
    aux->cmp = cmp;
    aux->first = NULL;
    aux->last = NULL;
    aux->it = NULL;
    aux->size = 0;
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
    list->size++;
}



void remove(listADT list, void * elem) {
    if (list == NULL || list->first == NULL) {
        return;
    }

    TList current = list->first;
    TList previous = NULL;

    while (current != NULL) {
        if (list->cmp(current->head, elem) == 0) { 
            if (previous == NULL) {
                list->first = current->tail;
                if (list->first == NULL) {
                    list->last = NULL;
                }
            } else {
                previous->tail = current->tail;
                if (previous->tail == NULL) {
                    list->last = previous;
                }
            }
            my_free(current);
            list->size--;    
            return;
        }
        previous = current;
        current = current->tail;
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
  if(list->first == NULL) {
    return NULL;
  }
  return list->first->head;
}

void * poll(listADT list){
    TList aux = list->first;
    list->first = list->first->tail;
    my_free(aux);
    list->size--;
    return aux->head;
}

void queue(listADT list , void * elem){
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
    list->size++;
}

uint8_t is_empty(listADT list) {
    return list->size <= 0;
}