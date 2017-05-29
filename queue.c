//
// Created by a1660098 on 17/03/17.
//

#include <stdio.h>
#include "queue.h"

int queue_contains(queue_t *queue, queue_t *elem);

//------------------------------------------------------------------------------
// Insere um elemento no final da fila.
// Condicoes a verificar, gerando msgs de erro:
// - a fila deve existir
// - o elemento deve existir
// - o elemento nao deve estar em outra fila
void queue_append(queue_t **queue, queue_t *elem) {
    if (elem == NULL)
        return;

    if (!(elem->next == NULL && elem->prev == NULL))
        return;

    if (*queue == NULL) {
        *queue = elem;
        (*queue)->prev = *queue;
        (*queue)->next = *queue;
        return;
    } else {
        elem->next = (*queue);
        elem->prev = (*queue)->prev;
        elem->prev->next = elem;
        (*queue)->prev = elem;
    }
}

//------------------------------------------------------------------------------
// Remove o elemento indicado da fila, sem o destruir.
// Condicoes a verificar, gerando msgs de erro:
// - a fila deve existir
// - a fila nao deve estar vazia
// - o elemento deve existir
// - o elemento deve pertencer a fila indicada
// Retorno: apontador para o elemento removido, ou NULL se erro

queue_t *queue_remove(queue_t **queue, queue_t *elem) {
    if (queue == NULL || elem == NULL)
        return NULL;
    if (queue_size(*queue) == 0)
        return NULL;
    if (queue_contains(*queue, elem) == 0)
        return NULL;

    queue_t *next_e = elem->next;
    queue_t *prev_e = elem->prev;
    next_e->prev = prev_e;
    prev_e->next = next_e;

//    Se for o unico elemento da fila
    if (elem->next == elem && elem->prev == elem)
        *queue = NULL;

//    Se o elemento procurado for o primeiro da fila
    if (*queue == elem)
        *queue = next_e;


    elem->next = NULL;
    elem->prev = NULL;
    return elem;
}

int queue_contains(queue_t *queue, queue_t *elem) {
    queue_t *iterator = queue;
    if (iterator == elem)
        return 1;

    for (iterator = iterator->next; iterator != queue; iterator = iterator->next) {
        if (iterator == elem)
            return 1;
    }
    return 0;
}

//------------------------------------------------------------------------------
// Conta o numero de elementos na fila
// Retorno: numero de elementos na fila

int queue_size(queue_t *queue) {
    if (queue == NULL)
        return 0;

    queue_t *iterator = queue;
    int i = 1;

    for (iterator = iterator->next; iterator != queue; iterator = iterator->next) {
        i++;
    }
    return i;
}

void queue_print(char *name, queue_t *queue, void (*print_elem)(void *)) {
    queue_t *iterator;
    printf("%s", name);
    printf("[");
    for (iterator = queue; iterator != NULL;) {
        print_elem(iterator);
        iterator = iterator->next;
        if (iterator == queue)
            break;
        else
            printf(" ");
    }
    printf("]\n");
}
