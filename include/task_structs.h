#ifndef _TASK_STRUCTS_H_
#define _TASK_STRUCTS_H_

#include "task.h"

extern void task_switch(void);
extern void task_run(void);

enum task_state {
	TASK_STATE_UNUSED,
	TASK_STATE_SPAWNED,
	TASK_STATE_SLEEPING,
	TASK_STATE_RUNNING,
	TASK_STATE_TERMINATED,
};

struct task_info {
	enum task_state state;

	/* Options set on spawning */
	task_t task;
	void *arg;
	struct task_opt opt;

	uint32_t *stack;	/* This is stack limiting address.
				   As supplied by user or allocated. */
	uint32_t *stack_top;	/* Last known top of the stack
				   (valid for non-running tasks). */

	struct task_info *list_next;
};

#endif /* _TASK_STRUCTS_H_ */
