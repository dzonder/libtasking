#include "task.h"

#include "arch.h"
#include "task_structs.h"

enum task_state {
	TASK_STATE_UNUSED,
	TASK_STATE_SPAWNED,
};

struct task_info {
	enum task_state state;
	struct task_run_info run_info;
	struct task_info *list_next;
};

struct task_descs {
	struct task_info *list_head_unused;

	struct task_info *list_head_spawn;
	struct task_info *list_tail_spawn;
};

static struct task_descs task_descs;
static struct task_info task_infos[TASK_LIMIT_SPAWNED];

static struct scheduler *scheduler = NULL;

static struct task_opt default_task_opt = {
	.priority	= TASK_DEFAULT_PRIORITY,
	.stack_size	= TASK_DEFAULT_STACK_SIZE,
	.stack_user	= NULL,
};

void task_init(struct scheduler *_scheduler)
{
	scheduler = _scheduler;

	/* Initialize task_info pool as 'unused' list */
	for (uint16_t i = 0U; i < TASK_LIMIT_SPAWNED; ++i) {
		struct task_info *task_info = task_infos + i;

		task_info->state = TASK_STATE_UNUSED;
		task_info->list_next = task_info + 1;
	}
	task_infos[TASK_LIMIT_SPAWNED - 1].list_next = NULL;

	task_descs.list_head_unused = task_infos;

	/* Initialize 'spawn' list */
	task_descs.list_head_spawn = NULL;
	task_descs.list_tail_spawn = NULL;
}

void task_spawn_opt(task_t task, void *arg, struct task_opt *task_opt)
{
	struct task_info *task_info = task_descs.list_head_unused;

	assert(task_info != NULL);
	assert(task_info->state == TASK_STATE_UNUSED);

	task_descs.list_head_unused = task_info->list_next;

	task_info->state = TASK_STATE_SPAWNED;

	task_info->run_info.task = task;
	task_info->run_info.arg = arg;
	task_info->run_info.priority = task_opt->priority;

	/* Append to the end of the 'spawn' list */
	task_info->list_next = NULL;
	if (task_descs.list_tail_spawn != NULL) {
		task_descs.list_tail_spawn->list_next = task_info;
	} else {
		task_descs.list_head_spawn = task_info;
	}
	task_descs.list_tail_spawn = task_info;
}

void task_spawn(task_t task, void *arg)
{
	task_spawn_opt(task, arg, &default_task_opt);
}

bool task_next(struct task_run_info *run_info)
{
	assert(run_info != NULL);

	struct task_info *task_info = task_descs.list_head_spawn;

	if (task_info == NULL)
		return false; /* Nothing queued in 'spawn' list */

	assert(task_info->state == TASK_STATE_SPAWNED);

	/* Remove from 'spawn' list */
	if (task_descs.list_head_spawn == task_descs.list_tail_spawn)
		task_descs.list_tail_spawn = NULL;
	task_descs.list_head_spawn = task_info->list_next;

	*run_info = task_info->run_info;

	/* Mark as unused. Prepend to 'unused' list */
	task_info->state = TASK_STATE_UNUSED;
	task_info->list_next = task_descs.list_head_unused;
	task_descs.list_head_unused = task_info;

	return true;
}

void task_yield(void)
{
	assert(false); // TODO yield task
}

void task_schedule(void)
{
	assert(scheduler != NULL);
	assert(scheduler->schedule != NULL);

	if (scheduler->init != NULL)
		scheduler->init();

	for (;;)
		scheduler->schedule();
}
