#ifndef _TASK_H_
#define _TASK_H_

#include <stdint.h>
#include "sched_structs.h"

#define TASK_MAX_TASKS		16

enum {
	TASK_PRIORITY_NORMAL = 10,
};

typedef void (*task_t)(void *arg);

/* Should be executed before using any other task functions */
void task_init(struct scheduler *scheduler);

/* Adds a task to the scheduler queue */
void task_spawn(task_t task, void *arg, int8_t priority);

/* Running task can use yield to relinquish MCU */
void task_yield(void);

/* Should be executed in order to start scheduling tasks */
void task_schedule(void);

#endif /* _TASK_H_ */
