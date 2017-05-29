// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DAINF UTFPR
// Versão 1.0 -- Março de 2015
//
// Estruturas de dados internas do sistema operacional

#ifndef __DATATYPES__
#define __DATATYPES__

#include <ucontext.h>

typedef enum {
    SUSPENDED, RUNNING, COMPLETED
} status_t;

typedef enum {
    SYSTEM, USER
} task_category_t;
// Estrutura que define uma tarefa
typedef struct task_t {
    // preencher quando necessário
    struct task_t *prev, *next;
    int id;
    ucontext_t context;
    struct task_t *exit_to;
    int priority;
    int dynamic_priority;
    task_category_t category;
    unsigned int ticks;
    unsigned int activations;
    unsigned int cpu_time;
    unsigned int created_at;
    status_t status;
    int exit_code;
    struct task_t *waiting_on;
} task_t;

// estrutura que define um semáforo
typedef struct {
    // preencher quando necessário
} semaphore_t;

// estrutura que define um mutex
typedef struct {
    // preencher quando necessário
} mutex_t;

// estrutura que define uma barreira
typedef struct {
    // preencher quando necessário
} barrier_t;

// estrutura que define uma fila de mensagens
typedef struct {
    // preencher quando necessário
} mqueue_t;

#endif
