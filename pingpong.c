//
// Created by jarvis on 3/21/17.
//

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "datatypes.h"
#include "pingpong.h"
#include "queue.h"

#define STACK_SIZE 32768

task_t *current_task;
task_t *previous_task;

task_t main_task;
task_t dispatcher;

task_t *ready_queue;

void dispatcher_body(void *);

task_t *scheduler_selector(task_t *queue, int aging_factor);

task_t *scheduler();

void pingpong_init() {
/* desativa o buffer da saida padrao (stdout), usado pela função printf */
    setvbuf(stdout, 0, _IONBF, 0);
    getcontext(&main_task.context);
    main_task.id = 0;
    task_create(&dispatcher, dispatcher_body, NULL);
    dispatcher.parent = &main_task;

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

    if (task != &dispatcher) {
        queue_append((queue_t **) &ready_queue, (queue_t *) task);
        task->parent = &dispatcher;
        task->id = task->prev->id + 1;
    }

#ifdef DEBUG
    printf ("\ntask_create: criou tarefa %d", task->tid) ;
#endif
    return task->id;
}

int task_switch(task_t *task) {
    previous_task = current_task;
    current_task = task;
#ifdef DEBUG
    printf ("\ntask_switch: trocando contexto %d -> %d", previous_task->tid, current_task->tid) ;
#endif
    int i = swapcontext(&previous_task->context, &current_task->context);

    return i;
}

void task_exit(int exitCode) {
#ifdef DEBUG
    printf("\ntask_exit: tarefa %d sendo encerrada", current_task->tid);
#endif
    task_switch(current_task->parent);
}

int task_id() {
    return current_task->id;
}

void task_yield() {
    if (current_task != &main_task)
        queue_append((queue_t **) &ready_queue, (queue_t *) current_task);
    task_switch(&dispatcher);
}

int task_getprio(task_t *task) {
    if (task == NULL)
        task = current_task;
    return task->priority;
}

void task_setprio(task_t *task, int prio) {
    if (task == NULL)
        task = current_task;
    task->priority = prio;
    task->dynamic_priority = prio;
}

void dispatcher_body(void *arg) // dispatcher é uma tarefa
{
    while (queue_size((queue_t *) ready_queue) > 0) {
        task_t *next = scheduler(); // scheduler é uma função
        if (next) {
            task_switch(next); // transfere controle para a tarefa "next"
        }
    }
    task_exit(0); // encerra a tarefa dispatcher
}

task_t *scheduler() {
    task_t *highest_priority_task = scheduler_selector(ready_queue, -1);
    return (task_t *) queue_remove((queue_t **) &ready_queue, (queue_t *) highest_priority_task);
}

void update_dynamic_priority(task_t *queue, task_t *to_exclude, int aging_factor) {
    task_t *iterator;
    for (iterator = queue; iterator != NULL;) {
        if(iterator != to_exclude)
            iterator->dynamic_priority += aging_factor;
        iterator = iterator->next;
        if (iterator == queue)
            break;
    }
}

task_t *scheduler_selector(task_t *queue, int aging_factor) {
    task_t *iterator;
    task_t *selected = NULL;
    for (iterator = queue; iterator != NULL;) {
        if (selected == NULL || iterator->dynamic_priority < selected->dynamic_priority)
            selected = iterator;

        iterator = iterator->next;
        if (iterator == queue)
            break;
    }
    selected->dynamic_priority = selected->priority;
    update_dynamic_priority(queue, selected, aging_factor);
    return selected;
}




