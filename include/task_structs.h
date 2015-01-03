#ifndef _TASK_STRUCTS_H_
#define _TASK_STRUCTS_H_

#include "task.h"

enum task_state {
	TASK_STATE_UNUSED,
	TASK_STATE_RUNNABLE,
	TASK_STATE_WAITING,
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

	uint8_t fp_used;

	struct task_info *list_next;
};

extern struct task_info *task_main;

extern void task_switch(void);
extern void task_run(struct task_info *);

#endif /* _TASK_STRUCTS_H_ */
