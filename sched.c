#include "sched.h"

#include "arch.h"
#include "task_structs.h"

/*
 * The simplest strategy:
 * - spawned tasks are queued in a FIFO fashion
 * - current task has to finish before the next spawned task can be executed
 *   (tasks are not preempted)
 * - priority is currently ignored (TODO)
 */
static void fifo_schedule(struct task_descs *task_descs)
{
	struct task_run_info run_info;

	if (!task_next(&run_info))
		return; /* No new tasks spawned */

	/* Run */
	run_info.task(run_info.arg);
}

struct scheduler fifo_scheduler = {
	.init		= NULL,
	.schedule	= fifo_schedule,
};
