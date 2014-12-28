#ifndef _TASK_STRUCTS_H_
#define _TASK_STRUCTS_H_

#include "task.h"

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

	uint32_t *stack_top;

	struct task_info *list_next;
};

#endif /* _TASK_STRUCTS_H_ */
