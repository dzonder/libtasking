#ifndef _TASK_H_
#define _TASK_H_

#include "arch.h"
#include "sched_structs.h"
#include "task_conf.h"

struct task_opt {
	uint8_t priority;
	uint32_t stack_size;
	uint32_t *user_stack;
};

/* Task function signature */
typedef void (*task_t)(void *arg);

/* Should be executed before using any other task functions */
void task_init(struct scheduler *scheduler);

/* Adds a task to the scheduler queue */
void task_spawn(task_t task, void *arg);

/* Adds a task to the scheduler queue with specific options */
void task_spawn_opt(task_t task, void *arg, struct task_opt *opt);

/* Running task can use yield to relinquish MCU */
void task_yield(void);

#endif /* _TASK_H_ */
