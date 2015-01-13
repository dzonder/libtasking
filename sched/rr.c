/*
 * Round Robin Scheduler. Like FCFS but with preemption.
 */

#include "sched/rr.h"

#include "arch.h"
#include "sched/fcfs.h"
#include "task_conf.h"

static void rr_init(void *user_data)
{
	fcfs_scheduler.init(NULL);

	rr_scheduler.enqueue = fcfs_scheduler.enqueue;
	rr_scheduler.dequeue = fcfs_scheduler.dequeue;
}

static bool rr_preempt(struct task_info *task_info)
{
	return true;
}

struct scheduler rr_scheduler = {
	.init		= rr_init,
	.enqueue	= NULL,
	.dequeue	= NULL,
	.preempt	= rr_preempt,
};
