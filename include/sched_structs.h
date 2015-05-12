#ifndef _SCHED_STRUCTS_H_
#define _SCHED_STRUCTS_H_

#include "arch.h"

struct task_info;

struct scheduler {
	void * (*init)(void *conf);
	void (*free)(void *desc);
	void (*enqueue)(void *desc, struct task_info *);
	struct task_info * (*dequeue)(void *desc);
	bool (*preempt)(void *desc, struct task_info *task_info);
};

#endif /* _SCHED_STRUCTS_H_ */
