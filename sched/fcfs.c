/*
Copyright (c) 2015 Michal Olech.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/* FCFS scheduler.

   The simplest strategy:
   - spawned tasks are queued in a FIFO fashion
   - current task has to finish before the next spawned task can be executed
     (tasks are not preempted)
   - priority is ignored */

#include "sched/fcfs.h"

#include "task_structs.h"

struct fcfs_desc {
	struct task_info *list_head_queued_tasks;
	struct task_info *list_tail_queued_tasks;
};

static void * fcfs_init(void *conf)
{
	struct fcfs_desc *desc = malloc(sizeof(*desc));
	assert(desc != NULL);

	desc->list_head_queued_tasks = NULL;
	desc->list_tail_queued_tasks = NULL;

	return desc;
}

static void fcfs_free(void *desc)
{
	assert(desc != NULL);

	free(desc);
}

static void fcfs_enqueue(void *desc, struct task_info *task_info)
{
	assert(desc != NULL);

	struct fcfs_desc *fcfs_desc = (struct fcfs_desc *)desc;

	assert(task_info != NULL);
	assert(task_info->list_next == NULL);

	if (fcfs_desc->list_tail_queued_tasks == NULL) {
		fcfs_desc->list_head_queued_tasks = task_info;
	} else {
		fcfs_desc->list_tail_queued_tasks->list_next = task_info;
	}
	fcfs_desc->list_tail_queued_tasks = task_info;
}

static struct task_info * fcfs_dequeue(void *desc)
{
	assert(desc != NULL);

	struct fcfs_desc *fcfs_desc = (struct fcfs_desc *)desc;

	struct task_info *task_info = fcfs_desc->list_head_queued_tasks;

	if (task_info == NULL)
		return NULL;

	fcfs_desc->list_head_queued_tasks = task_info->list_next;
	if (fcfs_desc->list_head_queued_tasks == NULL)
		fcfs_desc->list_tail_queued_tasks = NULL;

	task_info->list_next = NULL;

	return task_info;
}

static bool fcfs_preempt(void *desc, struct task_info *task_info)
{
	return false;
}

struct scheduler fcfs_scheduler = {
	.init		= fcfs_init,
	.free		= fcfs_free,
	.enqueue	= fcfs_enqueue,
	.dequeue	= fcfs_dequeue,
	.preempt	= fcfs_preempt,
};
