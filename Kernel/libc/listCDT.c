#include "../include/listADT.h"
//@todo: CAMMELLLL CASEEE
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

listADT newList(compare cmp) {
    listADT aux = my_malloc(sizeof(struct listCDT));
    aux->cmp = cmp;
    return aux;
}

static void freeListRec(TList list) {
    if (list==NULL) {
        return;
    }
    freeListRec(list->tail);
    my_free(list);
}

void add(listADT list, void * elem) {
    TList aux = my_malloc(sizeof(node));
    aux->tail = NULL;
    aux->head = elem;
    if(list->first == NULL) {
        list->first = aux;
        list->last = aux;
        toBegin(list);
    } else {
        list->last->tail = aux;
        list->last = aux;
    }
}

void freeList(listADT list) {
    freeListRec(list->first);
    my_free(list);
}

void toBegin(listADT list) {
    list->it = list->first;
}


uint8_t hasNext(const listADT list) {
    return list->it != NULL;
}


void * next(listADT list) {
    if(!hasNext(list)) {
        return list->first;
    }
    void * aux = list->it->head;
    list->it = list->it->tail;
    return aux;
}

void * poll(listADT list){
    void * rta = list->first->head;
    TList aux = list->first;
    list->first = list->first->tail;
    my_free(aux);
    return rta;
}

void * peek(listADT list) {
    return list->first->head;
}

void queue(listADT list , void * elem){
    TList aux = my_malloc(sizeof(node));
    aux->head = elem;
    list->last->tail = aux;
    list->last = aux;
    aux->tail = NULL;
}

void queue_first(listADT list , void * elem) {
    TList aux = my_malloc(sizeof(node));
    aux->head = elem;
    list->first = aux;
    list->last = aux;
}