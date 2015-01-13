/*
 * Round Robin Scheduler. Like FCFS but with preemption.
 */

#include "sched/rr.h"

#include "sched/fcfs.h"

static void * rr_init(void *user_data)
{
	void *desc = fcfs_scheduler.init(NULL);

	rr_scheduler.free = fcfs_scheduler.free;
	rr_scheduler.enqueue = fcfs_scheduler.enqueue;
	rr_scheduler.dequeue = fcfs_scheduler.dequeue;

	return desc;
}

static bool rr_preempt(void *desc, struct task_info *task_info)
{
	return true;
}

struct scheduler rr_scheduler = {
	.init		= rr_init,
	.free		= NULL,
	.enqueue	= NULL,
	.dequeue	= NULL,
	.preempt	= rr_preempt,
};
