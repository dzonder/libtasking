#ifndef _TASK_STRUCTS_H_
#define _TASK_STRUCTS_H_

#include "task.h"

struct task_run_info {
	task_t task;
	void *arg;
	uint8_t priority;
};

bool task_next(struct task_run_info *run_info);

#endif /* _TASK_STRUCTS_H_ */
