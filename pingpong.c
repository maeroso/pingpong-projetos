//
// Created by jarvis on 3/21/17.
//

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "datatypes.h"
#include "queue.h"

#define STACK_SIZE 32768

task_t *current_task;
task_t *previous_task;

task_t main_task;

int task_count = 0;

void pingpong_init() {
/* desativa o buffer da saida padrao (stdout), usado pela função printf */
    setvbuf(stdout, 0, _IONBF, 0);
    getcontext(&main_task.context);
    main_task.id = task_count;
    current_task = &main_task;
}

int task_create(task_t *task, void (*start_func)(void *), void *arg) {
    getcontext(&task->context);
    char *stack = malloc(STACK_SIZE);
    if (stack) {
        task->context.uc_stack.ss_sp = stack;
        task->context.uc_stack.ss_size = STACK_SIZE;
        task->context.uc_link = NULL;
    }
    makecontext(&task->context, (void (*)(void)) start_func, 1, arg);

    task->id = ++task_count;

#ifdef DEBUG
    printf ("\ntask_create: criou tarefa %d", task->id) ;
#endif
    return task->id;
}

int task_switch(task_t *task) {
    previous_task = current_task;
    current_task = task;

#ifdef DEBUG
    printf ("\ntask_switch: trocando contexto %d -> %d", previous_task->id, current_task->id) ;
#endif
    int i = swapcontext(&previous_task->context, &current_task->context);

    return i;
}

void task_exit(int exitCode) {
#ifdef DEBUG
    printf("\ntask_exit: tarefa %d sendo encerrada", current_task->id);
#endif
    task_switch(&main_task);
}

int task_id() {
    return current_task->id;
}


