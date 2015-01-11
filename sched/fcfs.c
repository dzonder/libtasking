/*
 * FCFS Scheduler. The simplest strategy:
 * - spawned tasks are queued in a FIFO fashion
 * - current task has to finish before the next spawned task can be executed
 *   (tasks are not preempted)
 * - priority is currently ignored (TODO)
 */

#include "sched/fcfs.h"

#include "task_structs.h"

static struct task_info *list_head_queued_tasks;
static struct task_info *list_tail_queued_tasks;

static void fcfs_init(void)
{
	list_head_queued_tasks = NULL;
	list_tail_queued_tasks = NULL;
}

static void fcfs_enqueue(struct task_info *task_info)
{
	assert(task_info != NULL);
	assert(task_info->list_next == NULL);

	if (list_tail_queued_tasks == NULL) {
		list_head_queued_tasks = task_info;
	} else {
		list_tail_queued_tasks->list_next = task_info;
	}
	list_tail_queued_tasks = task_info;
}

static struct task_info * fcfs_dequeue(void)
{
	struct task_info *task_info = list_head_queued_tasks;

	assert(task_info != NULL);

	list_head_queued_tasks = task_info->list_next;
	if (list_head_queued_tasks == NULL)
		list_tail_queued_tasks = NULL;

	task_info->list_next = NULL;

	return task_info;
}

struct scheduler fcfs_scheduler = {
	.init		= fcfs_init,
	.enqueue	= fcfs_enqueue,
	.dequeue	= fcfs_dequeue,
};
