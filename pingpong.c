//
// Created by jarvis on 3/21/17.
//

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/time.h>
#include <signal.h>
#include "datatypes.h"
#include "pingpong.h"
#include "queue.h"

#define STACK_SIZE 32768

task_t *current_task;
task_t *previous_task;

task_t main_task;
task_t dispatcher;

task_t *ready_queue;
task_t *suspended_queue;

struct itimerval tick_interval;
struct sigaction tick_action;

const int quantum = 20;

unsigned int m_system_time = 0;

int queue_contains(queue_t *queue, queue_t *elem);

void dispatcher_body(void *);

task_t *scheduler_selector(task_t *queue, int aging_factor);

task_t *scheduler();

void handler(int signum) {
    m_system_time++;
    current_task->cpu_time++;
    if (current_task->category == USER) {
        current_task->ticks--;
        if (current_task->ticks == 0) {
            task_yield();
        }
    }
}

void pingpong_init() {
/* desativa o buffer da saida padrao (stdout), usado pela função printf */
    setvbuf(stdout, 0, _IONBF, 0);

    task_create(&dispatcher, dispatcher_body, NULL);
    dispatcher.category = SYSTEM;
    dispatcher.exit_to = &main_task;

    task_create(&main_task, NULL, NULL);
    current_task = &main_task;

    tick_action.sa_handler = handler;
    sigemptyset(&tick_action.sa_mask);
    tick_action.sa_flags = 0;
    sigaction(SIGALRM, &tick_action, NULL);

    tick_interval.it_value.tv_usec = 1000;
    tick_interval.it_interval.tv_usec = 1000;
    setitimer(ITIMER_REAL, &tick_interval,
              NULL); //ITIMER_REAL trabalha no tempo "normal", e não no tempo de CPU. Dispara um SIGALRM;
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

        task->id = task->prev->id + 1;

        task->category = USER;
        task->status = RUNNING;

        task->waiting_on = NULL;
        task->exit_to = &dispatcher;

        task->exit_code = 0;

        task->ticks = quantum;
        task->created_at = systime();
        task->cpu_time = 0;
        task->activations = 0;
    }

#ifdef DEBUG
    printf ("\ntask_create: Task %d created at %d ms", task->id, task->created_at) ;
#endif
    return task->id;
}

int task_switch(task_t *task) {
    previous_task = current_task;
    current_task = task;
    current_task->activations++;
#ifdef DEBUG
    printf ("\ntask_switch: trocando contexto %d -> %d", previous_task->tid, current_task->tid) ;
#endif
    int i = swapcontext(&previous_task->context, &current_task->context);

    return i;
}

void task_exit(int exitCode) {
//#ifdef DEBUG
    printf("\ntask_exit: Task %d exit: execution time %d ms, processor time %d ms, %d activations", current_task->id,
           systime() - current_task->created_at, current_task->cpu_time, current_task->activations);
//#endif
    current_task->status = COMPLETED;
    current_task->exit_code = exitCode;

    //resume todas as tarefas esperando por current_task;
    task_t *iterator = suspended_queue;
    if(iterator->waiting_on == current_task)
        task_resume(iterator);
    for(iterator = iterator->next; iterator != suspended_queue; iterator = iterator->next) {
        if (iterator->waiting_on == current_task) {
            task_t *suspended_next = iterator->next;
            task_resume(iterator);
            if(suspended_next != NULL)
                iterator = suspended_next;
        }
    }

    queue_remove((queue_t **) &ready_queue, (queue_t *) current_task);
}

void task_suspend(task_t *task, task_t **queue) {
    task->status = SUSPENDED;
    queue_remove((queue_t **) &ready_queue, (queue_t *) task);
    queue_append((queue_t **) queue, (queue_t *) task);
    task_yield();
}

void task_resume(task_t *task) {
    task->status = RUNNING;
    task->waiting_on = NULL;
    queue_remove((queue_t **) &suspended_queue, (queue_t *) task);
    queue_append((queue_t **) &ready_queue, (queue_t *) task);
}

int task_id() {
    return current_task->id;
}

void task_yield() {
    if (current_task != &dispatcher || current_task->status == COMPLETED)
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

unsigned int systime() {
    return m_system_time;
}

int task_join(task_t *task) {
    if (task == NULL || task->status == COMPLETED || !queue_contains((queue_t *) ready_queue, (queue_t *) task))
        return -1;

    current_task->waiting_on = task;
    task_suspend(current_task, &suspended_queue);

    return task->exit_code;
}

void dispatcher_body(void *arg) // dispatcher é uma tarefa
{
    while (queue_size((queue_t *) ready_queue) > 0) {
        task_t *next = scheduler(); // scheduler é uma função
        if (next) {
            next->ticks = quantum;
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
        if (iterator != to_exclude)
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
    if (selected != NULL) {
        selected->dynamic_priority = selected->priority;
        update_dynamic_priority(queue, selected, aging_factor);
    }
    return selected;
}




