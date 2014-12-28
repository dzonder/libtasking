#include "sched.h"

#include "arch.h"
#include "task_structs.h"

/*
 * FIFO Scheduler. The simplest strategy:
 * - spawned tasks are queued in a FIFO fashion
 * - current task has to finish before the next spawned task can be executed
 *   (tasks are not preempted)
 * - priority is currently ignored (TODO)
 */

static void fifo_init(void)
{
}

static void fifo_enqueue(struct task_info *task_info)
{
}

static struct task_info * fifo_dequeue(void)
{
}

struct scheduler fifo_scheduler = {
	.init		= fifo_init,
	.enqueue	= fifo_enqueue,
	.dequeue	= fifo_dequeue,
};
