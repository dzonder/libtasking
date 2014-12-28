#ifndef _TASK_H_
#define _TASK_H_

#include <stdint.h>
#include "sched_structs.h"

enum {
	TASK_LIMIT_SPAWNED		= 16,
	TASK_DEFAULT_PRIORITY		= 10,
	TASK_DEFAULT_STACK_SIZE		= 1024,
};

struct task_opt {
	uint8_t priority;
	uint32_t stack_size;
	uint32_t *stack;
};

typedef void (*task_t)(void *arg);

/* Should be executed before using any other task functions */
void task_init(struct scheduler *scheduler);

/* Adds a task to the scheduler queue */
void task_spawn(task_t task, void *arg);
void task_spawn_opt(task_t task, void *arg, struct task_opt *opt);

/* Running task can use yield to relinquish MCU */
void task_yield(void);

/* Should be executed in order to start scheduling tasks */
void task_schedule(void);

#endif /* _TASK_H_ */
